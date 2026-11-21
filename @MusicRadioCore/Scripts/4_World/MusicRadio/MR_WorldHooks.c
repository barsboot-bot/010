// ============================================================================
//  MusicRadio - Core
//  MR_WorldHooks.c : the only place where vanilla classes are patched.
//  Layer: 4_World
//
//  Everything here is additive: no vanilla behaviour is replaced, only new
//  members and new methods are appended, so the mod stays compatible with other
//  mods that patch the same classes (Expansion, CF, ...).
// ============================================================================

// --------------------------------------------------------------- handheld ---
modded class PersonalRadio
{
	protected ref MR_ItemHost m_MRHost;

	override void EEInit()
	{
		super.EEInit();

		if (!m_MRHost)
			m_MRHost = new MR_ItemHost(this);

		// Registered on both sides: the key is built from local network ids and
		// the client needs it to find the object a PLAY RPC points at.
		MR_Registry.Register(MR_HostType.HOST_ITEM, this);
	}

	// Public accessor used by the action / UI layer.
	MR_ItemHost MR_GetHost()
	{
		if (!m_MRHost)
			m_MRHost = new MR_ItemHost(this);
		return m_MRHost;
	}

	bool MR_IsRadioPlaying()
	{
		return (m_MRHost && m_MRHost.MR_IsPlaying());
	}

	// Called by the server when the radio is destroyed or removed.
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (m_MRHost && m_MRHost.MR_IsPlaying())
		{
			m_MRHost.MR_ClearState();
			m_MRHost.MR_AudioStop(true);
		}
	}
}

// ----------------------------------------------------------------- vehicle ---
modded class CarScript
{
	protected ref MR_VehicleHost m_MRVehicleHost;

	override void EEInit()
	{
		super.EEInit();

		if (!m_MRVehicleHost)
			m_MRVehicleHost = new MR_VehicleHost(this);

		// See PersonalRadio.EEInit: the client needs this index too.
		MR_Registry.Register(MR_HostType.HOST_VEHICLE, this);
	}

	MR_VehicleHost MR_GetRadioHost()
	{
		if (!m_MRVehicleHost)
			m_MRVehicleHost = new MR_VehicleHost(this);
		return m_MRVehicleHost;
	}

	// Convenience for actions and the HUD.
	bool MR_HasWorkingRadio()
	{
		return (m_MRVehicleHost && m_MRVehicleHost.MR_CanPlay());
	}

	string MR_RadioBlockReason()
	{
		if (!m_MRVehicleHost)
			return "no radio system";
		return m_MRVehicleHost.MR_BlockReason();
	}
}

// ------------------------------------------------------------ player helper ---
modded class PlayerBase
{
	// The host the player is currently "operating" (opened the radio menu on).
	protected ref MR_DeviceBase m_MR_ActiveHost;
	protected int m_MR_LastRequestMs;

	void MR_SetActiveHost(MR_DeviceBase host)
	{
		m_MR_ActiveHost = host;
	}

	MR_DeviceBase MR_GetActiveHost()
	{
		return m_MR_ActiveHost;
	}

	// Simple flood protection: a client may not spam play requests.
	bool MR_CanSendRequest(int cooldownMs)
	{
		int now = GetGame().GetTime();
		if (now - m_MR_LastRequestMs < cooldownMs)
			return false;
		m_MR_LastRequestMs = now;
		return true;
	}

	// What is the player looking at / holding that can host a radio?
	// Returns null when nothing usable.
	MR_DeviceBase MR_FindUsableHost(out string blockReason)
	{
		blockReason = string.Empty;

		// 1) handheld radio in hands
		ItemBase inHands = ItemBase.Cast(GetHumanInventory().GetEntityInHands());
		if (inHands)
		{
			PersonalRadio pr = PersonalRadio.Cast(inHands);
			if (pr)
			{
				MR_ItemHost itemHost = pr.MR_GetHost();
				if (itemHost.MR_CanPlay())
					return itemHost;
				blockReason = itemHost.MR_BlockReason();
				return null;
			}
		}

		// 2) the vehicle the player sits in
		CarScript car = CarScript.Cast(GetParent());
		if (!car)
		{
			Object under = this;
			if (GetGame().GetPlayer())
				under = GetGame().GetPlayer();
			// fall back to the object the player is inside
			car = CarScript.Cast(GetHierarchyRoot());
		}
		if (car)
		{
			MR_VehicleHost vHost = car.MR_GetRadioHost();
			if (vHost.MR_CanPlay())
				return vHost;
			blockReason = vHost.MR_BlockReason();
			return null;
		}

		blockReason = "get in a vehicle or hold a radio";
		return null;
	}
}
