// ============================================================================
//  MusicRadio - Core
//  MR_ClientRPC.c : client side RPC handling + outgoing requests.
//  Layer: 5_Mission
//
//  Flow
//  ----
//  client PLAY click  ->  MR_ClientRPC.SendRequest()  ->  server validates
//  server             ->  MR.RPC_PLAY / RPC_STOP / RPC_REPLY  ->  clients
//  client RPC_PLAY    ->  resolve locator -> SoundSet -> host.MR_AudioStart()
//
//  The client never decides *whether* it may play: it only executes what the
//  server approved. A hacked client can at most annoy itself.
// ============================================================================

class MR_ClientRPC
{
	// Подставить клиентский обработчик в мост 4_World (PlayerBase.OnRPC).
	// Вызывается один раз из MissionGameplay.OnInit.
	static void Install()
	{
		if (MR_MissionBridge.ClientRpc)
			return;
		MR_MissionBridge.ClientRpc = ScriptCaller.Create(Handle);
	}

	// ------------------------------------------------------------- receiving
	// Входящие RPC принимает PlayerBase.OnRPC (MR_PlayerRPC.c, 4_World): сервер
	// адресует пакет объекту игрока, и на сервере ровно так же приходят наши
	// запросы (MR_ServerHooks). Mission для этого не годится - у неё нет OnRPC.
	// Прежние попытки и их ошибки:
	//   modded class DayZGame            -> Unknown type 'DayZGame'
	//   MissionGameplay.override OnRPC   -> no function with this name in base

	// --------------------------------------------------------------- outgoing
	static void SendRequest(MR_Request req)
	{
		if (!req)
			return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		// target = the player object: the packet is delivered to
		// PlayerBase.OnRPC on the server (patched by @MusicRadioServer only).
		// Client -> server RPCs pass recipient = null.
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(req.Serialize());
		rpc.Send(player, MR.RPC_REQUEST, true, null);
	}

	// Cassette / MD player: "play what is in the deck".
	// The client only names the album; the server checks the physical item
	// (battery, memory card, inserted carrier) before anything is played.
	static void RequestPlayCarrier(MR_DeviceBase host, MR_HostType type, int idLow, int idHigh, string albumId)
	{
		if (albumId == string.Empty)
		{
			MR_ClientState.SetMessage("No cassette or disc in the player");
			return;
		}

		MR_Request req = new MR_Request();
		req.m_Action   = MR_Action.ACT_PLAY_ALBUM;
		req.m_HostType = type;
		req.m_HostId   = idLow;
		req.m_HostIdHigh = idHigh;
		req.m_TrackId  = albumId;          // an album id, not a track id
		req.m_Broadcast = true;
		req.m_Volume   = MR_ClientState.Settings().m_DefaultVolume;
		req.m_Radius   = MR_ClientState.Settings().m_DefaultRadius;
		SendRequest(req);
	}

	// Convenience wrapper for the UI.
	static void RequestPlay(MR_DeviceBase host, MR_HostType type, int idLow, int idHigh, string trackId, MR_MediaKind kind)
	{
		MR_Request req = new MR_Request();
		req.m_Action = MR_Action.ACT_PLAY_TRACK;
		req.m_HostType = type;
		req.m_HostId = idLow;
		req.m_HostIdHigh = idHigh;
		req.m_TrackId = trackId;
		req.m_Volume = MR_ClientState.Settings().m_DefaultVolume;
		req.m_Radius = MR_ClientState.Settings().m_DefaultRadius;
		SendRequest(req);
	}

	static void RequestStop(MR_HostType type, int idLow, int idHigh)
	{
		MR_Request req = new MR_Request();
		req.m_Action = MR_Action.ACT_STOP;
		req.m_HostType = type;
		req.m_HostId = idLow;
		req.m_HostIdHigh = idHigh;
		SendRequest(req);
	}

	static void RequestVolume(MR_HostType type, int idLow, int idHigh, int volume)
	{
		MR_Request req = new MR_Request();
		req.m_Action = MR_Action.ACT_VOLUME;
		req.m_HostType = type;
		req.m_HostId = idLow;
		req.m_HostIdHigh = idHigh;
		req.m_Volume = volume;
		SendRequest(req);
	}

	static void RequestNext(MR_HostType type, int idLow, int idHigh)
	{
		MR_Request req = new MR_Request();
		req.m_Action = MR_Action.ACT_NEXT;
		req.m_HostType = type;
		req.m_HostId = idLow;
		req.m_HostIdHigh = idHigh;
		SendRequest(req);
	}

	static void RequestLibrary()
	{
		MR_Request req = new MR_Request();
		req.m_Action = MR_Action.ACT_REQUEST_LIB;
		SendRequest(req);
	}

	// ----------------------------------------------------------------- incoming
	// Called from PlayerBase.OnRPC (MR_PlayerRPC.c). Returns true when the id was ours.
	// Which kind of host is this object? One rule shared by UI and RPC code.
	static MR_HostType DetectHostType(Object obj)
	{
		if (!obj)
			return MR_HostType.HOST_NONE;

		if (obj.IsInherited(PersonalRadio))
			return MR_HostType.HOST_ITEM;

		if (obj.IsInherited(CarScript))
			return MR_HostType.HOST_VEHICLE;

		return MR_HostType.HOST_NONE;
	}

	static bool Handle(int rpcType, ParamsReadContext ctx)
	{
		// ВАЖНО: в Enforce Script у case нет своей области видимости, поэтому
		// 'string payload;' в каждой ветке - это Multiple declaration of variable
		// (лог 02:00, пять строк: 167, 175, 183, 191, 206). Объявляем один раз
		// до switch - как это делают ванильные обработчики OnRPC.
		string payload;

		switch (rpcType)
		{
			case MR.RPC_CONFIG:
			{
				if (!ctx.Read(payload))
					return true;
				HandleConfig(payload);
				return true;
			}
			case MR.RPC_LIBRARY:
			{
				string manifest;
				if (!ctx.Read(manifest))
					return true;
				MR_ClientState.ApplyManifest(manifest);
				return true;
			}
			case MR.RPC_PLAY:
			{
				if (!ctx.Read(payload))
					return true;
				HandlePlay(payload);
				return true;
			}
			case MR.RPC_STOP:
			{
				if (!ctx.Read(payload))
					return true;
				HandleStop(payload);
				return true;
			}
			case MR.RPC_VOLUME:
			{
				if (!ctx.Read(payload))
					return true;
				HandleVolume(payload);
				return true;
			}
			case MR.RPC_REPLY:
			{
				if (!ctx.Read(payload))
					return true;
				bool ok;
				string message;
				if (MR_Net.DecodeReply(payload, ok, message))
				{
					MR_ClientState.SetMessage(message);
					if (!ok && GetGame().GetPlayer())
						GetGame().Chat("[Radio] " + message, "colorStatusChannel");
				}
				return true;
			}
			case MR.RPC_NOWPLAYING:
			{
				if (!ctx.Read(payload))
					return true;
				string title, artist, provider;
				if (MR_Net.DecodeNowPlaying(payload, title, artist, provider))
				{
					string line = title;
					if (artist != string.Empty && artist != "-")
						line = artist + " - " + title;
					MR_ClientState.SetMessage("Now playing: " + line);
				}
				return true;
			}
		}
		return false;
	}

	static void HandleConfig(string payload)
	{
		MR_JsonValue root = MR_Json.Parse(payload);
		if (!root)
			return;

		MR_Settings s = MR_Settings.FromJson(root);
		MR_ClientState.SetSettings(s);
		MR.Log("Config received from server (v" + s.m_Version + ")");

		// The manifest is request-on-demand for large libraries, so ask now.
		MR_ClientRPC.RequestLibrary();
	}

	static void HandlePlay(string payload)
	{
		MR_PlayRequest req = MR_Net.DecodePlay(payload);
		if (!req)
		{
			MR.LogWarn("Malformed PLAY payload");
			return;
		}

		MR_Track track = MR_ClientState.Library().GetById(req.m_Locator.m_TrackId);
		if (!track)
		{
			MR.LogWarn("PLAY for unknown track id: " + req.m_Locator.m_TrackId);
			return;
		}

		// The host object must exist locally (it may be out of the network
		// bubble - then we simply ignore it, the audio would not be audible).
		MR_DeviceBase host = MR_ClientState.GetOpenHost();
		Object hostObj = MR_Registry.Find(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		if (!hostObj)
			hostObj = MR_Registry.FindLoose(req.m_HostType, req.m_HostId);
		if (!MR_ClientRPC.IsSameHost(host, hostObj))
			host = MR_ClientRPC.ResolveHost(hostObj, req.m_HostType);

		if (!host)
		{
			MR.Log("PLAY: host not in this client's bubble, ignored");
			return;
		}

		host.MR_SetState(track.m_Id, track.DisplayName(), track.m_Provider, req.m_Volume, req.m_Radius, req.m_StartOffset);
		host.MR_SetDurationClient(track.m_Duration);

		// Resolve audio for this client: provider + track id -> SoundSet.
		string error;
		string soundSet = MR_MediaResolver.ResolveTrack(track, error);
		if (soundSet == string.Empty)
		{
			MR_ClientState.SetMessage("Cannot play: " + error);
			MR.LogWarn("PLAY: " + error);
			return;
		}

		if (!host.MR_AudioStart(soundSet, req.m_Loop, req.m_StartOffset))
		{
			MR_ClientState.SetMessage("Audio failed: " + soundSet);
			return;
		}

		MR_ClientAudio.Register(host, req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		MR_ClientAudio.Init();
	}

	static MR_DeviceBase ResolveHost(Object obj, MR_HostType type)
	{
		if (type == MR_HostType.HOST_ITEM)
		{
			PersonalRadio pr = PersonalRadio.Cast(obj);
			if (pr)
				return pr.MR_GetHost();
		}
		if (type == MR_HostType.HOST_VEHICLE)
		{
			CarScript car = CarScript.Cast(obj);
			if (car)
				return car.MR_GetRadioHost();
		}
		// A cassette deck / MD player is its own host. Before this branch a
		// deck was registered, found by the server and then silently refused
		// here - "no host in this client's bubble" for a deck lying next to you.
		if (type == MR_HostType.HOST_PLAYER)
		{
			MR_CassettePlayer deck = MR_CassettePlayer.Cast(obj);
			if (deck)
				return deck.MR_GetDeckHost();

			MR_MiniDiscPlayer md = MR_MiniDiscPlayer.Cast(obj);
			if (md)
				return md.MR_GetDeckHost();
		}
		return null;
	}

	static void HandleStop(string payload)
	{
		MR_HostType type;
		int idLow;
		int idHigh;
		int reason;

		if (!MR_Net.DecodeStop(payload, type, idLow, idHigh, reason))
			return;

		Object obj = MR_Registry.Find(type, idLow, idHigh);
		MR_DeviceBase host = MR_ClientState.GetOpenHost();

		// Find the right host instance among the registered ones.
		if (!host || !MR_ClientRPC.IsSameHost(host, obj))
			host = MR_ClientRPC.ResolveHost(obj, type);

		if (!host)
			return;

		host.MR_ClearState();
		host.MR_AudioStop();
		MR_ClientAudio.Unregister(host);

		if (reason == MR_StopReason.STOP_TOO_FAR)
			MR_ClientState.SetMessage("Radio out of range");
		else if (reason == MR_StopReason.STOP_RUINED)
			MR_ClientState.SetMessage("Radio destroyed");
	}

	static bool IsSameHost(MR_DeviceBase host, Object obj)
	{
		if (!host || !obj)
			return false;
		return (host.MR_GetAudioObject() == obj);
	}

	static void HandleVolume(string payload)
	{
		MR_HostType type;
		int idLow;
		int idHigh;
		int volume;

		if (!MR_Net.DecodeVolume(payload, type, idLow, idHigh, volume))
			return;

		Object obj = MR_Registry.Find(type, idLow, idHigh);
		MR_DeviceBase host = MR_ClientRPC.ResolveHost(obj, type);
		if (!host)
			return;

		host.MR_SetVolumeClient(volume);
	}
}
