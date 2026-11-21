// ============================================================================
//  MusicRadio - Server
//  MR_ServerEngine.c : the authority. Everything a client asks for is checked
//  here before any audio is allowed to play anywhere.
//  Layer: 4_World (server process only)
//
//  Responsibilities
//  ----------------
//   1. load the config + media library
//   2. validate every play request (permissions, host, distance, cooldown)
//   3. keep a session table (who is playing what, where, how loud)
//   4. fan out PLAY / STOP / VOLUME to the clients that should hear it
//   5. advance to the next track when the client reports the media ended
//   6. audit log for moderation
// ============================================================================

class MR_ServerEngine
{
	protected static ref MR_Settings m_Settings;
	protected static ref MR_Library  m_Library;
	protected static ref map<string, ref MR_Session> m_Sessions;
	protected static ref map<string, int> m_LastRequest;   // playerId -> ms
	protected static bool m_Initialised;

	// ------------------------------------------------------------------ setup
	static void Init()
	{
		if (m_Initialised)
			return;

		if (!m_Sessions)
			m_Sessions = new map<string, ref MR_Session>();
		if (!m_LastRequest)
			m_LastRequest = new map<string, int>();

		m_Settings = MR_Config.LoadServer();
		MR_ConfigRuntime.Set(m_Settings);
		MR_Config.DumpSummary(m_Settings);

		m_Library = new MR_Library();
		m_Library.LoadAll();

		MR_AuditLog.Open(MR.DIR_MISSION + MR.LOG_FILE);

		m_Initialised = true;

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MR_ServerEngine.Tick, MR.SERVER_TICK_MS, true);
		MR.Log("Server engine ready (protocol " + MR.PROTOCOL.ToString() + ")");
	}

	static void Shutdown()
	{
		if (!m_Initialised)
			return;

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(MR_ServerEngine.Tick);
		StopAll(MR_StopReason.STOP_USER, false);
		MR_AuditLog.Close();
		m_Initialised = false;
	}

	static MR_Settings Settings()
	{
		if (!m_Settings)
			m_Settings = new MR_Settings();
		return m_Settings;
	}

	static MR_Library Library()
	{
		if (!m_Library)
		{
			m_Library = new MR_Library();
			m_Library.LoadAll();
		}
		return m_Library;
	}

	// ---------------------------------------------------------------- clients
	// ------------------------------------------------------------------ send
	// RPC идут на ОБЪЕКТ ИГРОКА, а не на объект игры.
	//
	// Причина - в клиенте: принять пакет можно только там, где есть OnRPC.
	// У объекта игры (DayZGame) наш слой его не видит, у Mission OnRPC нет
	// вообще (логи 01:12 и 01:23). А PlayerBase.OnRPC есть и работает -
	// именно так сервер получает наши запросы.
	//
	// Одно место на все отправки: 8 вызовов rpc.Send(null, ...) в этом файле
	// иначе легко забыть и оставить один "в никуда".
	static void SendTo(PlayerBase pb, int rpcType, string payload)
	{
		if (!pb || !pb.GetIdentity())
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(payload);
		rpc.Send(pb, rpcType, true, pb.GetIdentity());
	}

	static void SendToIdent(PlayerBase pb, PlayerIdentity to, int rpcType, string payload)
	{
		if (!pb || !to)
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(payload);
		rpc.Send(pb, rpcType, true, to);
	}

	static void SendConfig(PlayerIdentity to)
	{
		if (!to)
			return;

		string text;
		Settings().WriteJson(text);
		SendToIdent(PlayerBase.Cast(MR_ServerUtil.FindPlayer(to)), to, MR.RPC_CONFIG, text);
	}

	static void SendLibrary(PlayerIdentity to)
	{
		if (!to)
			return;

		// The manifest also carries the server's unix time: clients use it as the
		// broadcast clock, so every player hears the same second of a station.
		SendToIdent(PlayerBase.Cast(MR_ServerUtil.FindPlayer(to)), to, MR.RPC_LIBRARY, Library().SerializeManifest());
	}

	// Called on join / reconnect: give the client the config, the catalogue and
	// whatever is already playing close to it.
	static void OnPlayerJoined(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;

		SendConfig(player.GetIdentity());
		SendLibrary(player.GetIdentity());
		SyncActiveSessions(player);
	}

	static void OnPlayerDisconnected(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;

		string pid = player.GetIdentity().GetId();

		// Sessions owned by the leaving player keep playing only if they belong
		// to a vehicle (the car is still there); otherwise they die with them.
		TStringArray toStop = new TStringArray();
		foreach (string id, MR_Session s : m_Sessions)
		{
			if (s.m_OwnerId == pid && s.m_HostType == MR_HostType.HOST_ITEM)
				toStop.Insert(id);
		}
		foreach (string sid : toStop)
			StopSession(sid, MR_StopReason.STOP_HOST_GONE);
	}

	// ---------------------------------------------------------------- requests
	static void HandleRequest(PlayerBase player, string payload)
	{
		if (!player || !player.GetIdentity())
			return;

		MR_Request req = MR_Request.Parse(payload);
		if (req.m_Action == MR_Action.ACT_UNKNOWN)
			return;

		if (Settings().m_LogRequests)
			MR.Log("request from " + player.GetIdentity().GetName() + ": action=" + ((int)req.m_Action).ToString() + " track=" + req.m_TrackId);

		switch (req.m_Action)
		{
			case MR_Action.ACT_REQUEST_LIB:
			{
				SendLibrary(player.GetIdentity());
				return;
			}
			case MR_Action.ACT_PLAY_TRACK:
			{
				StartPlay(player, req);
				return;
			}
			case MR_Action.ACT_PLAY_ALBUM:
			{
				StartPlayAlbum(player, req);
				return;
			}
			case MR_Action.ACT_STOP:
			{
				StopByHost(req.m_HostType, req.m_HostId, req.m_HostIdHigh, MR_StopReason.STOP_USER);
				return;
			}
			case MR_Action.ACT_VOLUME:
			{
				ChangeVolume(player, req);
				return;
			}
			case MR_Action.ACT_NEXT:
			case MR_Action.ACT_PREV:
			{
				NextTrack(player, req, req.m_Action == MR_Action.ACT_NEXT);
				return;
			}
			case MR_Action.ACT_NOWPLAYING:
			{
				SendNowPlaying(player, req.m_TrackId);
				return;
			}
			case MR_Action.ACT_SEEK:
			{
				// The deck rewound / fast-forwarded. The server owns the
				// position on the carrier, so it takes the client's value,
				// clamps it and stores it - nothing is broadcast.
				MR_DeckState.SeekTo(req.m_HostType, req.m_HostId, req.m_HostIdHigh, req.m_Volume);
				return;
			}
		}
	}

	// --------------------------------------------------- play: one whole album
	// A cassette / MiniDisc is an album, so "play" means "start the album and
	// remember its id in the session". Autoplay then stays inside that album
	// (see NextTrack) - a tape never jumps to a different record.
	static void StartPlayAlbum(PlayerBase player, MR_Request req)
	{
		PlayerIdentity ident = player.GetIdentity();

		if (!Settings().m_AllowLocalTracks)
		{
			Reply(ident, false, "Music is disabled on this server");
			return;
		}

		Object hostObj = MR_Registry.Find(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		if (!hostObj)
		{
			Reply(ident, false, "Media player not found in the world");
			return;
		}

		string reason;
		if (!ValidateHost(player, hostObj, req.m_HostType, reason))
		{
			Reply(ident, false, reason);
			return;
		}

		// What is actually inserted? Never trust the id the client sent: the
		// server reads the attachment and derives the album from the class name.
		string albumId = string.Empty;

		// Car with a docked deck: the album comes from the deck on the dash.
		CarScript dockCar = CarScript.Cast(hostObj);
		if (dockCar)
			albumId = MR_MediaItems.AlbumInVehicle(dockCar);
		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(hostObj);
		if (deck)
		{
			EntityAI tape = deck.MR_GetInsertedCassette();
			if (tape)
				albumId = MR_MediaItems.AlbumOf(tape.GetType());
		}
		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(hostObj);
		if (md)
		{
			EntityAI disc = md.MR_GetInsertedDisc();
			if (disc)
				albumId = MR_MediaItems.AlbumOf(disc.GetType());
		}

		// Рация - тоже устройство: играет кассету/диск, вставленные в неё.
		PersonalRadio handheld = PersonalRadio.Cast(hostObj);
		if (handheld)
			albumId = handheld.MR_CurrentAlbum();

		if (albumId == string.Empty)
		{
			Reply(ident, false, "Nothing is inserted in the player");
			return;
		}
		if (albumId != req.m_TrackId)
		{
			MR.LogWarn("PLAY_ALBUM: client asked for '" + req.m_TrackId + "' but the deck holds '" + albumId + "' - using the deck");
		}

		array<ref MR_Track> album = Library().ByAlbum(albumId);
		if (album.Count() == 0)
		{
			Reply(ident, false, "Album '" + albumId + "' is not in the media pack");
			return;
		}

		// Does the tape remember where it stopped? Ask the CARRIER, not the
		// client: the client's idea of "the album is at 2:17" is only a hint.
		string resumeTrack = string.Empty;
		int resumeOffset = 0;
		int resumeNo = 0;
		EntityAI carrier = MR_DeckState.FindCarrier(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		MR_DeckState.ResumePoint(carrier, albumId, resumeTrack, resumeOffset);

		// The remembered track must still exist in this album (media pack may
		// have been rebuilt since the tape was last played).
		MR_Track startTrack = null;
		for (int ai = 0; ai < album.Count(); ai++)
		{
			MR_Track candidate = album.Get(ai);
			if (!candidate)
				continue;
			if (startTrack == null)
				startTrack = candidate;          // fallback: first track
			if (candidate.m_Id == resumeTrack)
			{
				startTrack = candidate;
				break;
			}
		}

		if (startTrack == null)
		{
			Reply(ident, false, "Album '" + albumId + "' has no playable tracks");
			return;
		}

		// Never resume past the end: a tape that ran on starts the next track.
		// ВАЖНО: MR_CarrierState.IsFinished(MR_CarrierMemory, int) ждёт объект
		// памяти, а не секунды. Вызов с int даёт сразу три ошибки компилятора:
		//   Only NULL assignment is allowed
		//   Cannot convert 'int' to 'MR_CarrierMemory' for argument '0'
		// Секунды сравниваем здесь.
		if (resumeOffset > 0 && startTrack.m_Duration > 0 && resumeOffset >= startTrack.m_Duration)
			resumeOffset = 0;

		// Start the remembered track through the normal path so all
		// permission / distance / cooldown rules apply unchanged.
		MR_Request play = new MR_Request();
		play.m_HostType  = req.m_HostType;
		play.m_HostId    = req.m_HostId;
		play.m_HostIdHigh = req.m_HostIdHigh;
		play.m_TrackId   = startTrack.m_Id;
		play.m_Volume    = req.m_Volume;
		play.m_Radius    = req.m_Radius;
		play.m_Broadcast = req.m_Broadcast;

		// StartPlay resets the offset to 0 by design (a fresh play button),
		// so the resume offset is carried on the request itself.
		play.m_StartOffset = resumeOffset;

		StartPlay(player, play);

		// Tag the session with the album so NextTrack keeps it inside the tape.
		string key = HostKey(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		MR_Session s = m_Sessions.Get(key);
		if (s)
		{
			s.m_AlbumId = albumId;
			BroadcastNowPlaying(s, album.Get(0));
			Reply(ident, true, "Playing album '" + albumId + "' (" + album.Count().ToString() + " tracks)");
		}
	}

	// Next (or previous) track of the same album, wrapping at the end so a tape
	// loops like it physically would.
	static MR_Track NextInAlbum(string albumId, string currentId, bool forward)
	{
		array<ref MR_Track> album = Library().ByAlbum(albumId);
		if (album.Count() == 0)
			return null;

		int index = -1;
		for (int i = 0; i < album.Count(); i++)
		{
			if (album.Get(i).m_Id == currentId)
			{
				index = i;
				break;
			}
		}

		if (index < 0)
			return album.Get(0);

		int next = index - 1;
		if (forward)
			next = index + 1;
		if (next >= album.Count())
			next = 0;
		if (next < 0)
			next = album.Count() - 1;

		return album.Get(next);
	}

	// ------------------------------------------------------------ play: checks
	static void StartPlay(PlayerBase player, MR_Request req)
	{
		PlayerIdentity ident = player.GetIdentity();

		// 1) feature flags
		if (!Settings().m_AllowLocalTracks)
		{
			Reply(ident, false, "Music is disabled on this server");
			return;
		}

		if (req.m_HostType == MR_HostType.HOST_VEHICLE && !Settings().m_EnableVehicleRadio)
		{
			Reply(ident, false, "Vehicle radio is disabled");
			return;
		}
		if (req.m_HostType == MR_HostType.HOST_ITEM && !Settings().m_EnableHandheldRadio)
		{
			Reply(ident, false, "Handheld radio is disabled");
			return;
		}

		// 2) cooldown
		string pid = ident.GetId();
		int now = GetGame().GetTime();
		int last = 0;
		if (m_LastRequest.Contains(pid))
			last = m_LastRequest.Get(pid);
		if (Settings().m_CooldownSeconds > 0 && now - last < Settings().m_CooldownSeconds * 1000)
		{
			Reply(ident, false, "Slow down a moment");
			return;
		}
		m_LastRequest.Set(pid, now);

		// 3) the track exists and is allowed
		MR_Track track = Library().GetById(req.m_TrackId);
		if (!track)
		{
			Reply(ident, false, "Unknown track: " + req.m_TrackId);
			return;
		}
		if (!track.m_Enabled)
		{
			Reply(ident, false, "That entry is disabled");
			return;
		}
		// 4) the host object
		Object hostObj = MR_Registry.Find(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		if (!hostObj)
		{
			Reply(ident, false, "Radio not found in the world");
			return;
		}

		if (vector.Distance(hostObj.GetPosition(), player.GetPosition()) > 6.0)
		{
			Reply(ident, false, "You are too far from that radio");
			return;
		}

		// 5) host specific rules
		string blockReason;
		if (!ValidateHost(player, hostObj, req.m_HostType, blockReason))
		{
			Reply(ident, false, blockReason);
			return;
		}

		// 6) one session per host: replace whatever was there
		string hostKey = HostKey(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		if (m_Sessions.Contains(hostKey))
			StopSession(hostKey, MR_StopReason.STOP_USER);

		// 7) build the session
		MR_Session s = new MR_Session();
		s.m_Id = hostKey;
		s.m_HostType = req.m_HostType;
		s.m_HostId = req.m_HostId;
		s.m_HostIdHigh = req.m_HostIdHigh;
		s.m_OwnerId = pid;
		s.m_OwnerName = ident.GetName();
		s.m_TrackId = track.m_Id;
		s.m_Provider = track.m_Provider;
		s.m_Title = track.DisplayName();
		s.m_StartTimeMs = now;
		// Resume: a request may carry an offset (a tape that was stopped at
		// 2:17 keeps going from 2:17 instead of restarting the album).
		s.m_StartOffset = MR_CarrierState.ClampPosition(req.m_StartOffset, track.m_Duration);
		s.m_Duration = track.m_Duration;
		s.m_Volume = MR.ClampInt(req.m_Volume, 0, Settings().m_MaxVolume);
		s.m_Radius = MR.ClampFloat(req.m_Radius, MR.RADIUS_MIN, Settings().m_MaxRadius);
		s.m_Loop = track.m_Loop;
		s.m_Broadcast = req.m_Broadcast && Settings().m_AllowBroadcast;

		m_Sessions.Set(hostKey, s);

		// 7b) the tape starts remembering. Only for hosts that hold a physical
		// carrier (cassette / MiniDisc in a deck or on a car dash) - a plain
		// handheld radio has nothing to write to and costs one lookup.
		MR_DeckState.TrackStart(s.m_HostType, s.m_HostId, s.m_HostIdHigh, s.m_TrackId, 0, s.m_StartOffset);

		// 8) tell the clients
		DispatchPlay(s, track, hostObj);
		Reply(ident, true, "Playing: " + s.m_Title);

		MR_AuditLog.Line("PLAY " + ident.GetName() + " (" + pid + ") host=" + hostKey + " track=" + track.m_Id + " vol=" + s.m_Volume.ToString() + " r=" + s.m_Radius.ToString());
	}

	static bool ValidateHost(PlayerBase player, Object hostObj, MR_HostType type, out string reason)
	{
		reason = string.Empty;

		if (type == MR_HostType.HOST_ITEM)
		{
			PersonalRadio radio = PersonalRadio.Cast(hostObj);
			if (!radio)
			{
				reason = "That is not a radio";
				return false;
			}
			if (radio.IsRuined())
			{
				reason = "The radio is ruined";
				return false;
			}
			if (!MR_VehicleRadio.HasPower(radio))
			{
				reason = "The radio has no working battery";
				return false;
			}

			// Единый контракт 20261174: рация играет только то, что в неё
			// вставили, и только с картой памяти - как дека.
			string notReady;
			if (Settings().m_RadioNeedsCarrier && !radio.MR_IsReady(notReady))
			{
				reason = "The radio is not ready: " + notReady;
				return false;
			}

			// The player must actually hold / carry it.
			if (!player.GetInventory().HasEntityInInventory(radio) && player.GetHumanInventory().GetEntityInHands() != radio)
			{
				reason = "You must hold the radio";
				return false;
			}
			return true;
		}

		if (type == MR_HostType.HOST_VEHICLE)
		{
			CarScript car = CarScript.Cast(hostObj);
			if (!car)
			{
				reason = "That is not a vehicle";
				return false;
			}
			if (car.IsRuined())
			{
				reason = "The vehicle is ruined";
				return false;
			}
			if (player.GetParent() != car)
			{
				reason = "You must sit in the vehicle";
				return false;
			}

			// Плеер ИЛИ кассета/диск в слоте машины заменяют правило
			// "рация в багажнике". Проверяем сам факт вложения, а не альбом:
			// положили кассету - источник уже есть, дальше решает сервер.
			bool hasDocked = (MR_MediaItems.DockedInVehicle(car) != null);
			if (!hasDocked && Settings().m_RequireRadioItemInCargo)
			{
				// Имя "radio" в этой функции уже занято PersonalRadio (ветка
				// HOST_ITEM выше). Объявлять здесь ещё одно "EntityAI radio"
				// нельзя: при этом компилятор пишет
				//   Incompatible parameter 'radio'   (лог 20:47, строка 527)
				EntityAI inCargo;
				if (!MR_VehicleRadio.HasRadioInCargo(car, inCargo))
				{
					reason = "No radio in the vehicle cargo and no music player on the dash";
					return false;
				}
				if (!MR_VehicleRadio.HasPower(inCargo))
				{
					reason = "The radio's battery is empty";
					return false;
				}
			}
			return true;
		}

		if (type == MR_HostType.HOST_PLAYER)
		{
			// Cassette deck / MD player: the physical prerequisites live in the
			// item's attachments, so they are checked here and not in the client.
			EntityAI player_item = EntityAI.Cast(hostObj);
			if (!player_item)
			{
				reason = "That is not a media player";
				return false;
			}
			if (!player.GetInventory().HasEntityInInventory(player_item) && player.GetHumanInventory().GetEntityInHands() != player_item)
			{
				reason = "You must hold the player";
				return false;
			}

			string why;
			MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(player_item);
			if (deck)
			{
				if (!deck.MR_IsReady(why))
				{
					reason = "Cassette deck: " + why;
					return false;
				}
				return true;
			}

			MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(player_item);
			if (md)
			{
				if (!md.MR_IsReady(why))
				{
					reason = "MD player: " + why;
					return false;
				}
				return true;
			}

			reason = "Unsupported media player";
			return false;
		}

		reason = "Unsupported radio type";
		return false;
	}

	// ------------------------------------------------------------- play: fanout
	static void DispatchPlay(MR_Session s, MR_Track track, Object hostObj)
	{
		MR_PlayRequest req = new MR_PlayRequest();
		req.m_HostType = s.m_HostType;
		req.m_HostId = s.m_HostId;
		req.m_HostIdHigh = s.m_HostIdHigh;
		MR_Locator loc = new MR_Locator();
		if (loc)
		{
			loc.m_Provider = s.m_Provider;
			loc.m_TrackId = s.m_TrackId;
			loc.m_Volume = s.m_Volume;
			loc.m_Seed = Math.RandomInt(0, 100000);
		}
		req.m_Locator = loc;
		// Without this every client started at 0:00 while the server believed
		// the session was at 2:17 - the deck and the server disagreed.
		req.m_StartOffset = s.m_StartOffset;
		req.m_Radius = s.m_Radius;
		req.m_Volume = s.m_Volume;
		req.m_Loop = s.m_Loop;
		req.m_Broadcast = s.m_Broadcast;

		string payload = MR_Net.EncodePlay(req);

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;

			if (!s.m_Broadcast)
			{
				// Private listening: only the owner hears it.
				if (pb.GetIdentity().GetId() != s.m_OwnerId)
					continue;
			}
			else
			{
				// Otherwise everyone inside the radio radius hears it.
				float dist = vector.Distance(pb.GetPosition(), hostObj.GetPosition());
				if (dist > s.m_Radius)
					continue;
			}

			SendTo(pb, MR.RPC_PLAY, payload);
		}

		BroadcastNowPlaying(s, track);
	}

	static void BroadcastNowPlaying(MR_Session s, MR_Track track)
	{
		string payload = MR_Net.EncodeNowPlaying(track.m_Title, track.m_Artist, track.m_Provider);

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;

			SendTo(pb, MR.RPC_NOWPLAYING, payload);
		}
	}

	// ------------------------------------------------------------------ volume
	static void ChangeVolume(PlayerBase player, MR_Request req)
	{
		string key = HostKey(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		MR_Session s = m_Sessions.Get(key);
		if (!s)
			return;

		// Only the owner may change the volume of a broadcast session; a private
		// session can be adjusted by whoever hears it.
		bool owner = (player.GetIdentity().GetId() == s.m_OwnerId);
		if (s.m_Broadcast && !owner && !IsAdmin(player.GetIdentity()))
		{
			Reply(player.GetIdentity(), false, "Only the person who started it can change the volume");
			return;
		}

		s.m_Volume = MR.ClampInt(req.m_Volume, 0, Settings().m_MaxVolume);

		string payload = MR_Net.EncodeVolume(s.m_HostType, s.m_HostId, s.m_HostIdHigh, s.m_Volume);

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;
			if (!s.m_Broadcast && pb.GetIdentity().GetId() != s.m_OwnerId)
				continue;

			SendTo(pb, MR.RPC_VOLUME, payload);
		}
	}

	// -------------------------------------------------------------- next / prev
	static void NextTrack(PlayerBase player, MR_Request req, bool forward)
	{
		string key = HostKey(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		MR_Session s = m_Sessions.Get(key);
		if (!s)
			return;

		Object hostObj = MR_Registry.Find(s.m_HostType, s.m_HostId, s.m_HostIdHigh);
		if (!hostObj)
		{
			StopSession(key, MR_StopReason.STOP_HOST_GONE);
			return;
		}

		// Manual NEXT/PREV always works. Only the automatic advance (the client
		// reports "track ended") obeys the server switch: with autoplayNext
		// disabled the radio goes quiet instead of continuing the playlist.
		if (!forward && !Settings().m_AllowRepeat)
		{
			StopSession(key, MR_StopReason.STOP_ENDED);
			return;
		}

		MR_Track current = Library().GetById(s.m_TrackId);
		string kind = "music";
		if (current)
			kind = current.m_Kind;

		// A cassette / disc session stays inside its album, in index order;
		// everything else picks a random track of the same kind.
		MR_Track next = null;
		if (s.m_AlbumId != string.Empty)
		{
			next = NextInAlbum(s.m_AlbumId, s.m_TrackId, forward);
		}
		else
		{
			next = Library().RandomTrack(kind, s.m_TrackId);
		}
		if (!next)
		{
			StopSession(key, MR_StopReason.STOP_ENDED);
			Reply(player.GetIdentity(), true, "End of the playlist");
			return;
		}

		// Reuse StartPlay via a synthetic request so every permission check
		// applies to auto-advance as well.
		MR_Request play = new MR_Request();
		play.m_Action = MR_Action.ACT_PLAY_TRACK;
		play.m_HostType = s.m_HostType;
		play.m_HostId = s.m_HostId;
		play.m_HostIdHigh = s.m_HostIdHigh;
		play.m_TrackId = next.m_Id;
		play.m_Volume = s.m_Volume;
		play.m_Radius = s.m_Radius;
		play.m_Broadcast = s.m_Broadcast;

		// Auto advance must not be blocked by the player cooldown.
		m_LastRequest.Set(player.GetIdentity().GetId(), 0);
		StartPlay(player, play);
	}

	static void SendNowPlaying(PlayerBase player, string trackId)
	{
		MR_Track t = Library().GetById(trackId);
		if (!t)
			return;

		string payload = MR_Net.EncodeNowPlaying(t.m_Title, t.m_Artist, t.m_Provider);
			SendTo(player, MR.RPC_NOWPLAYING, payload);
	}

	// -------------------------------------------------------------------- stop
	static void StopByHost(MR_HostType type, int idLow, int idHigh, MR_StopReason reason)
	{
		string key = HostKey(type, idLow, idHigh);
		if (!m_Sessions.Contains(key))
			return;
		StopSession(key, reason);
	}

	static void StopSession(string key, MR_StopReason reason)
	{
		MR_Session s = m_Sessions.Get(key);
		if (!s)
			return;

		string payload = MR_Net.EncodeStop(s.m_HostType, s.m_HostId, s.m_HostIdHigh, reason);

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;
			if (!s.m_Broadcast && pb.GetIdentity().GetId() != s.m_OwnerId)
				continue;

			SendTo(pb, MR.RPC_STOP, payload);
		}

		// Final position on the tape BEFORE the session is forgotten: this is
		// what makes "stop, pull the tape out, resume later" work.
		MR_DeckState.TrackStop(s.m_HostType, s.m_HostId, s.m_HostIdHigh);

		m_Sessions.Remove(key);

		if (reason != MR_StopReason.STOP_USER)
			MR_AuditLog.Line("STOP host=" + key + " reason=" + ((int)reason).ToString());
	}

	static void StopAll(MR_StopReason reason, bool silent = true)
	{
		TStringArray keys = new TStringArray();
		foreach (string key, MR_Session s : m_Sessions)
			keys.Insert(key);

		foreach (string k : keys)
		{
			if (silent)
				m_Sessions.Remove(k);
			else
				StopSession(k, reason);
		}
	}

	// --------------------------------------------------------------- watchdog
	// Runs once a second. Cheap checks only: the host still exists, the radio is
	// still powered, and a vehicle session does not outlive its car.
	static void Tick()
	{
		if (!m_Sessions)
			return;

		MR_Registry.Prune();

		// Cassette / disc position -> carrier. Runs on the server only and is
		// the single writer of a tape's memory (see MR_DeckState).
		MR_DeckState.Tick();

		TStringArray dead = new TStringArray();

		foreach (string key, MR_Session s : m_Sessions)
		{
			Object hostObj = MR_Registry.Find(s.m_HostType, s.m_HostId, s.m_HostIdHigh);
			if (!hostObj)
			{
				dead.Insert(key);
				continue;
			}

			if (s.m_HostType == MR_HostType.HOST_ITEM)
			{
				PersonalRadio radio = PersonalRadio.Cast(hostObj);
				if (!radio || radio.IsRuined() || !MR_VehicleRadio.HasPower(radio))
				{
					dead.Insert(key);
					continue;
				}
				// Вынули кассету или карту на ходу - музыка должна смолкнуть,
				// а не продолжать играть из пустой рации.
				string gone;
				if (Settings().m_RadioNeedsCarrier && !radio.MR_IsReady(gone))
				{
					dead.Insert(key);
					continue;
				}
			}

			if (s.m_HostType == MR_HostType.HOST_VEHICLE)
			{
				CarScript car = CarScript.Cast(hostObj);
				if (!car || car.IsRuined())
				{
					dead.Insert(key);
					continue;
				}
				if (Settings().m_RequireRadioItemInCargo)
				{
					EntityAI item;
					if (!MR_VehicleRadio.HasRadioInCargo(car, item) || !MR_VehicleRadio.HasPower(item))
					{
						dead.Insert(key);
						continue;
					}
				}
			}

			// Finite tracks are ended by the client (it owns the audio clock);
			// this is the safety net for when every listener is gone.
			if (!s.m_Loop && s.m_Duration > 0)
			{
				if (s.CurrentOffset() > s.m_Duration + 5)
					dead.Insert(key);
			}
		}

		foreach (string k : dead)
			StopSession(k, MR_StopReason.STOP_HOST_GONE);
	}

	// Late joiner: replay the sessions that should currently be audible.
	static void SyncActiveSessions(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;

		foreach (string key, MR_Session s : m_Sessions)
		{
			Object hostObj = MR_Registry.Find(s.m_HostType, s.m_HostId, s.m_HostIdHigh);
			if (!hostObj)
				continue;

			if (s.m_Broadcast && vector.Distance(player.GetPosition(), hostObj.GetPosition()) > s.m_Radius)
				continue;
			if (!s.m_Broadcast && player.GetIdentity().GetId() != s.m_OwnerId)
				continue;

			MR_Track track = Library().GetById(s.m_TrackId);
			if (!track)
				continue;

			MR_PlayRequest req = new MR_PlayRequest();
			req.m_HostType = s.m_HostType;
			req.m_HostId = s.m_HostId;
			req.m_HostIdHigh = s.m_HostIdHigh;
			MR_Locator joinLoc = new MR_Locator();
			if (joinLoc)
			{
				joinLoc.m_Provider = s.m_Provider;
				joinLoc.m_TrackId = s.m_TrackId;
			}
			req.m_Locator = joinLoc;
			req.m_StartOffset = s.CurrentOffset();   // join mid-song, synced
			req.m_Radius = s.m_Radius;
			req.m_Volume = s.m_Volume;
			req.m_Loop = s.m_Loop;
			req.m_Broadcast = s.m_Broadcast;

			SendTo(player, MR.RPC_PLAY, MR_Net.EncodePlay(req));
		}
	}

	// ----------------------------------------------------------------- helpers
	static string HostKey(MR_HostType type, int idLow, int idHigh)
	{
		return ((int)type).ToString() + ":" + idLow.ToString() + ":" + idHigh.ToString();
	}

	static void Reply(PlayerIdentity to, bool ok, string message)
	{
		if (!to)
			return;

		SendToIdent(PlayerBase.Cast(MR_ServerUtil.FindPlayer(to)), to, MR.RPC_REPLY, MR_Net.EncodeReply(ok, message));
	}

	static bool IsAdmin(PlayerIdentity ident)
	{
		if (!ident)
			return false;

		// DayZ exposes no portable "is admin" for script; servers that use a
		// permission mod can extend this one function. Default: no.
		return false;
	}

	static PlayerBase FindPlayerByIdentity(PlayerIdentity ident)
	{
		return MR_ServerUtil.FindPlayer(ident);
	}

	static int ActiveSessionCount()
	{
		if (!m_Sessions)
			return 0;
		return m_Sessions.Count();
	}
}

// ---------------------------------------------------------------------------
//  Lookup helper used by the RPC entry point.
//  (Kept in the same file so the engine stays self contained.)
// ---------------------------------------------------------------------------
class MR_ServerUtil
{
	// GetId() is not exposed on every build; UID() always is.
	static string IdOf(PlayerIdentity ident)
	{
		if (!ident)
			return string.Empty;
		return ident.GetId();
	}

	static PlayerBase FindPlayer(PlayerIdentity ident)
	{
		if (!ident)
			return null;

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;
			if (pb.GetIdentity().GetId() == MR_ServerUtil.IdOf(ident))
				return pb;
		}
		return null;
	}
}
