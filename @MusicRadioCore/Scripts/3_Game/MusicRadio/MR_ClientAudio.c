// ============================================================================
//  MusicRadio - Core
//  MR_ClientAudio.c : registry of audible hosts + per frame volume updates.
//  Layer: 3_Game   (client only in practice; cheap no-op on a dedicated server)
//
//  The RPC layer (5_Mission / MR_ClientRPC) does the talking, this class only
//  owns the "who is playing right now" bookkeeping so that
//    - the UI can list nearby radios
//    - the volume widget has something to read
//    - distance attenuation is refreshed every tick
// ============================================================================

class MR_ClientAudio
{
	protected static ref map<string, ref MR_DeviceBase> m_Hosts;
	protected static bool m_Ticking;

	// Static containers cannot be initialised at declaration in Enforce script;
	// everything goes through EnsureContainers().
	static void EnsureContainers()
	{
		if (!m_Hosts)
			m_Hosts = new map<string, ref MR_DeviceBase>();
	}

	static void Init()
	{
		EnsureContainers();
		if (m_Ticking)
			return;
		m_Ticking = true;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MR_ClientAudio.Tick, MR.TICK_MS, true);
		MR.Log("Client audio manager started");
	}

	static void Shutdown()
	{
		EnsureContainers();
		if (!m_Ticking)
			return;
		m_Ticking = false;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(MR_ClientAudio.Tick);

		foreach (string key, MR_DeviceBase dev : m_Hosts)
		{
			if (dev)
				dev.MR_AudioStop(true);
		}
		m_Hosts.Clear();
	}

	static string Key(MR_HostType type, int idLow, int idHigh)
	{
		return ((int)type).ToString() + ":" + idLow.ToString() + ":" + idHigh.ToString();
	}

	static void Register(MR_DeviceBase dev, MR_HostType type, int idLow, int idHigh)
	{
		EnsureContainers();
		if (!dev)
			return;
		string key = Key(type, idLow, idHigh);
		m_Hosts.Set(key, dev);
	}

	static void Unregister(MR_DeviceBase dev)
	{
		EnsureContainers();
		if (!dev)
			return;

		TStringArray dead = new TStringArray();
		foreach (string key, MR_DeviceBase other : m_Hosts)
		{
			if (other == dev)
				dead.Insert(key);
		}
		foreach (string k : dead)
			m_Hosts.Remove(k);
	}

	static MR_DeviceBase Find(MR_HostType type, int idLow, int idHigh)
	{
		EnsureContainers();
		return m_Hosts.Get(Key(type, idLow, idHigh));
	}

	static void Tick()
	{
		EnsureContainers();
		foreach (string key, MR_DeviceBase dev : m_Hosts)
		{
			if (!dev)
				continue;

			// A host that was deleted or dropped out of the bubble stops making
			// noise; the state itself is cleared by the owner / server.
			if (!dev.MR_IsPlaying())
			{
				dev.MR_AudioStop(true);
				continue;
			}

			dev.MR_AudioApplyVolume();

			// Natural end of a finite track: the CLIENT notices first (it is the
			// one with the audio), so it reports back and lets the server decide
			// what happens next (autoplay, next track, or nothing).
			if (dev.MR_GetDuration() > 0 && dev.MR_GetElapsedSeconds() >= dev.MR_GetDuration())
			{
				if (!dev.MR_WantsEndReport())
				{
					dev.MR_MarkEndReported();
					dev.MR_AudioStop();
					dev.MR_ClearState();
					MR_ClientAudio.ReportEnded(key);
				}
			}
		}
	}

	// key = "<type>:<idLow>:<idHigh>" as built by Key()
	protected static void ReportEnded(string key)
	{
		TStringArray parts = new TStringArray();
		key.Split(":", parts);
		if (parts.Count() < 3)
			return;

		// 3_Game cannot see MR_ClientRPC (5_Mission), so the request is built and
		// sent here directly with the same wire format.
		MR_Request req = new MR_Request();
		req.m_HostType   = MR.HostTypeFromInt(parts.Get(0).ToInt());
		req.m_HostId     = parts.Get(1).ToInt();
		req.m_HostIdHigh = parts.Get(2).ToInt();
		req.m_Action     = MR_Action.ACT_NEXT;   // "media ended, advance the playlist"

		// Man (not PlayerBase) - PlayerBase lives in 4_World and 3_Game must not
		// reference it. Identity and RPC sending are available on Man.
		Man player = GetGame().GetPlayer();
		if (!player)
			return;

		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(req.Serialize());
		rpc.Send(player, MR.RPC_REQUEST, true, null);
	}

	// Neighbourhood list for the UI: "<title> (<state>)" strings sorted by
	// distance from the local player.
	static void GetNearbyRadios(out TStringArray titles, out TIntArray distances)
	{
		EnsureContainers();
		titles = new TStringArray();
		distances = new TIntArray();

		Object player = GetGame().GetPlayer();
		if (!player)
			return;

		vector pPos = player.GetPosition();

		// Simple insertion sort: the list is tiny (a handful of radios).
		foreach (string key, MR_DeviceBase dev : m_Hosts)
		{
			if (!dev || !dev.MR_IsPlaying())
				continue;

			Object anchor = dev.MR_GetAudioObject();
			if (!anchor)
				continue;

			int dist = Math.Round(vector.Distance(pPos, anchor.GetPosition()));
			string label = dev.MR_GetTrackTitle();
			if (label == string.Empty)
				label = "radio";

			int insertAt = 0;
			while (insertAt < distances.Count() && distances.Get(insertAt) < dist)
				insertAt = insertAt + 1;

			titles.InsertAt(label, insertAt);
			distances.InsertAt(dist, insertAt);
		}
	}
}
