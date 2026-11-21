// ============================================================================
//  MusicRadio - Core
//  MR_GameHooks.c : hotkeys, menu lifetime, client RPC dispatch.
//  Layer: 5_Mission
//
//  F7 opens the radio menu on the host the player is currently using:
//    - radio in hands   -> that radio
//    - sitting in a car -> the car stereo
//  F8 stops whatever is playing on the radio you last opened.
//  Everything else (permissions, what is allowed) is decided by the server.
// ============================================================================

modded class MissionGameplay
{
	protected ref MR_RadioMenu m_MRRadioMenu;
	protected bool m_MRMenuOpen;

	override void OnInit()
	{
		super.OnInit();

		MR_ClientAudio.Init();
		// точка входа для входящих RPC уже в 4_World (PlayerBase.OnRPC),
		// здесь только подставляем её тело - см. MR_PlayerRPC.c
		MR_ClientRPC.Install();

		// Single player / listen server: nobody will send us a manifest, so read
		// the media packs directly. On a pure client the server's manifest wins.
		if (!GetGame().IsMultiplayer() || GetGame().IsServer())
		{
			MR_ConfigRuntime.Set(MR_Config.LoadServer());
			MR_LibraryIndex.Get().LoadAll();
		}

		MR.Log("Client mission ready");

		// Ask for config + catalogue once we are in the world.
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MR_ClientRPC.RequestLibrary, 3000, false);
	}

	// --------------------------------------------------------------- hotkeys
	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);

		if (key == KeyCode.KC_F7)
		{
			ToggleRadioMenu();
			return;
		}

		if (key == KeyCode.KC_F8)
		{
			MR_StopFromHotkey();
			return;
		}
	}

	void MR_StopFromHotkey()
	{
		MR_DeviceBase host = MR_ClientState.GetOpenHost();
		if (!host || !host.MR_IsPlaying())
			return;

		Object obj = host.MR_GetAudioObject();
		if (!obj)
			return;

		int idLow; int idHigh;
		MR.NetId(obj, idLow, idHigh);
		MR_ClientRPC.RequestStop(MR_ClientRPC.DetectHostType(obj), idLow, idHigh);
	}

	void ToggleRadioMenu()
	{
		if (m_MRMenuOpen)
		{
			if (m_MRRadioMenu)
				m_MRRadioMenu.Close();
			m_MRMenuOpen = false;
			return;
		}

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		string blockReason;
		MR_DeviceBase host = player.MR_FindUsableHost(blockReason);
		if (!host)
		{
			GetGame().Chat("[Radio] " + blockReason, "colorStatusChannel");
			return;
		}

		Object hostObj = host.MR_GetAudioObject();
		if (!hostObj)
			return;

		MR_HostType type = MR_ClientRPC.DetectHostType(hostObj);

		if (!m_MRRadioMenu)
			m_MRRadioMenu = new MR_RadioMenu();

		m_MRRadioMenu.SetHost(host, type, hostObj);

		GetGame().GetUIManager().ShowScriptedMenu(m_MRRadioMenu, null);
		m_MRMenuOpen = true;
	}

	// The menu calls this when it closes itself (X button / ESC / F7).
	void MR_NotifyMenuClosed()
	{
		m_MRMenuOpen = false;
	}

	override void OnMissionFinish()
	{
		MR_ClientAudio.Shutdown();

		if (m_MRRadioMenu)
		{
			m_MRRadioMenu.Close();
			m_MRRadioMenu = null;
		}

		super.OnMissionFinish();
	}

	// Входящие RPC от сервера сюда НЕ идут: сервер адресует их объекту игрока,
	// обработчик - PlayerBase.OnRPC в MR_PlayerRPC.c (4_World). У Mission нет
	// OnRPC вообще: "Function 'OnRPC' is marked as override, but there is no
	// function with this name" (лог 01:23).
}
