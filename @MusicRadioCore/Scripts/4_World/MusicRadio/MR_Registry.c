// ============================================================================
//  MusicRadio - Core
//  MR_Registry.c : which objects in the world are MusicRadio hosts.
//  Layer: 3_Game
//
//  The client cannot ask "is this EntityAI a radio?" without a cast, and casts
//  across modules are expensive and brittle. Instead every host registers
//  itself by network id when it starts and unregisters in its destructor.
// ============================================================================

class MR_Registry
{
	protected static ref map<string, ref MR_HostEntry> m_Entries;

	// A host's bookkeeping. The actual device state lives on the object itself
	// (MR_DeviceBase mixin); this is only an index + cached audio state used by
	// the client side RPC handler.
	static void Ensure()
	{
		if (!m_Entries)
			m_Entries = new map<string, ref MR_HostEntry>();
	}

	static string Key(MR_HostType type, int idLow, int idHigh)
	{
		return ((int)type).ToString() + ":" + idLow.ToString() + ":" + idHigh.ToString();
	}

	static void Register(MR_HostType type, Object obj)
	{
		Ensure();
		if (!obj)
			return;

		int idLow; int idHigh;
		MR.NetId(obj, idLow, idHigh);
		string key = Key(type, idLow, idHigh);
		MR_HostEntry entry = m_Entries.Get(key);
		if (!entry)
			entry = new MR_HostEntry();

		entry.m_Type = type;
		entry.m_Object = obj;
		entry.m_IdLow = idLow;
		entry.m_IdHigh = idHigh;
		m_Entries.Set(key, entry);
	}

	static void Unregister(MR_HostType type, Object obj)
	{
		Ensure();
		if (!obj)
			return;
		int idLow; int idHigh;
		MR.NetId(obj, idLow, idHigh);
		m_Entries.Remove(Key(type, idLow, idHigh));
	}

	static Object Find(MR_HostType type, int idLow, int idHigh)
	{
		Ensure();
		MR_HostEntry entry = m_Entries.Get(Key(type, idLow, idHigh));
		if (!entry)
			return null;
		return entry.m_Object;
	}

	// Network ids can differ in the high part between server and client on some
	// builds (persistent id vs bubble id), so the exact key is tried first and
	// then every entry with the same low id and type.
	static Object FindLoose(MR_HostType type, int idLow)
	{
		Ensure();
		foreach (string key, MR_HostEntry entry : m_Entries)
		{
			if (!entry || !entry.m_Object)
				continue;
			if (entry.m_Type == type && entry.m_IdLow == idLow)
				return entry.m_Object;
		}
		return null;
	}

	static MR_HostEntry FindEntry(MR_HostType type, int idLow, int idHigh)
	{
		Ensure();
		return m_Entries.Get(Key(type, idLow, idHigh));
	}

	// Drop entries whose object is gone (e.g. vehicle deleted by cleanup).
	static void Prune()
	{
		Ensure();
		TStringArray dead = new TStringArray();
		foreach (string key, MR_HostEntry entry : m_Entries)
		{
			if (!entry || !entry.m_Object)
				dead.Insert(key);
		}
		foreach (string k : dead)
			m_Entries.Remove(k);
	}
}

class MR_HostEntry
{
	MR_HostType m_Type;
	Object      m_Object;
	int         m_IdLow;
	int         m_IdHigh;
}

// ============================================================================
//  MR_VehicleRadio : helpers to find/consume the radio item inside a vehicle.
//  Layer: 4_World (needs CargoBase / GameInventory API)
// ============================================================================

class MR_VehicleRadio
{
	// Default accepted radio class names. A server can extend this list in
	// $mission:MusicRadio/AudioConfig.json -> "vehicles": { "radioItems": [...] }
	static const string RADIO_ITEM = "PersonalRadio";

	static bool HasRadioInCargo(CarScript car, out EntityAI found)
	{
		found = null;
		if (!car)
			return false;

		GameInventory inv = car.GetInventory();
		if (!inv)
			return false;

		CargoBase cargo = inv.GetCargo();
		if (!cargo)
			return false;

		int count = cargo.GetItemCount();
		for (int i = 0; i < count; i++)
		{
			EntityAI item = cargo.GetItem(i);
			if (!item)
				continue;

			if (IsRadioItem(item))
			{
				found = item;
				return true;
			}
		}
		return false;
	}

	static bool IsRadioItem(EntityAI item)
	{
		if (!item)
			return false;

		string type = item.GetType();
		if (type == RADIO_ITEM)
			return true;

		// Cassette deck / MD player: a "radio" in the sense that it is the object
		// which makes the sound. They are registered as HOST_PLAYER, but the
		// cargo check in a vehicle should accept them as a music source too.
		if (MR_MediaItems.IsCarrierPlayer(item))
			return true;

		// Any transceiver / walkie talkie the server declared, plus the vanilla
		// handheld radio family by base class.
		if (item.IsInherited(PersonalRadio))
			return true;

		return false;
	}

	// Battery / power check. Radios in DayZ are CE powered; a dead radio cannot
	// drive a speaker, which is both realistic and a nice progression hook.
	static bool HasPower(EntityAI item)
	{
		if (!item)
			return false;
		if (!item.HasEnergyManager())
			return true;   // no power requirement at all

		ComponentEnergyManager em = item.GetCompEM();
		if (!em)
			return true;

		return em.IsWorking();
	}
}
