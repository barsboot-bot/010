// ============================================================================
//  MusicRadio - Server
//  MR_ServerMission.c : boots the engine and wires the RPC / lifecycle hooks.
//  Layer: 5_Mission
//
//  This is the ONLY server mod file that must be present for the server to
//  work. It is never installed on clients, so a client cannot read the server
//  config or the audit log.
// ============================================================================

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();

		if (!GetGame().IsServer())
			return;

		MR_ServerEngine.Init();

		// Connect / disconnect is where a late joiner receives the config,
		// the catalogue and whatever is already playing around it.
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DelayedJoinSync, 5000, false);
	}

	// MissionServer has no per-player join event that is reliable across builds,
	// so the roster is polled once shortly after start and then on a slow
	// timer; OnClientPreparePlayer etc. differ between versions.
	protected ref map<string, bool> m_MRSynced;

	void DelayedJoinSync()
	{
		if (!m_MRSynced)
			m_MRSynced = new map<string, bool>();

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;

			string pid = pb.GetIdentity().GetId();
			if (pid == string.Empty)
				continue;

			if (!m_MRSynced.Contains(pid))
			{
				m_MRSynced.Set(pid, true);
				MR_ServerEngine.OnPlayerJoined(pb);
				MR.Log("synced new player: " + pb.GetIdentity().GetName());
			}
		}

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DelayedJoinSync, 5000, false);
	}

	override void OnMissionFinish()
	{
		MR_ServerEngine.Shutdown();
		super.OnMissionFinish();
	}
}

// ---------------------------------------------------------------------------
//  The RPC entry point is not here: client requests are sent with target = the
//  player object, so they are received in PlayerBase.OnRPC (see
//  MR_ServerHooks.c). Keeping it there means @MusicRadioCore and
//  @MusicRadioServer never override the same engine method - which is what
//  breaks when two mods both patch DayZGame.OnRPC.
// ---------------------------------------------------------------------------
