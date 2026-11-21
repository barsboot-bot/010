// ============================================================================
//  MusicRadio - Core
//  MR_Hosts.c : concrete hosts (item / vehicle) built on MR_DeviceBase.
//  Layer: 4_World
// ============================================================================

// ---------------------------------------------------------------- item host
class MR_ItemHost extends MR_DeviceBase
{
	protected EntityAI m_Item;

	void MR_ItemHost(EntityAI item)
	{
		MR_Init();
		m_Item = item;
	}

	EntityAI GetItem()
	{
		return m_Item;
	}

	override Object MR_GetAudioObject()
	{
		return m_Item;
	}

	// Рация - такое же устройство, как дека: играет только то, что в неё
	// вставили (кассета/диск), с картой памяти и батарейкой. Проверка одна на
	// весь мод - MR_MediaBay.
	override bool MR_CanPlay()
	{
		if (!m_Item)
			return false;
		if (m_Item.IsRuined())
			return false;
		if (!MR_VehicleRadio.HasPower(m_Item))
			return false;

		PersonalRadio radio = PersonalRadio.Cast(m_Item);
		if (radio)
		{
			// Сервер может вернуть старое поведение ("рация играет из
			// воздуха") флагом radioNeedsCarrier: false - тогда достаточно
			// батарейки, как было до 20261174.
			if (!MR_ConfigRuntime.Settings().m_RadioNeedsCarrier)
				return true;
			string why;
			return radio.MR_IsReady(why);
		}
		return true;   // не рация (будущие устройства) - старое поведение
	}

	override string MR_BlockReason()
	{
		if (!m_Item)
			return "radio missing";
		if (m_Item.IsRuined())
			return "radio is ruined";
		if (!MR_VehicleRadio.HasPower(m_Item))
			return "radio battery is empty";

		PersonalRadio radio = PersonalRadio.Cast(m_Item);
		if (radio)
		{
			if (!MR_ConfigRuntime.Settings().m_RadioNeedsCarrier)
				return string.Empty;
			string why;
			if (radio.MR_IsReady(why))
				return string.Empty;
			return why;   // "no cassette or disc inserted" / "no memory card"
		}
		return string.Empty;
	}

	// Handheld radios are quieter than a car stereo, and their reach shrinks.
	override float MR_GetEffectiveRadius()
	{
		return MR_ClampRadius(super.MR_GetEffectiveRadius() * 0.5);
	}

	static float MR_ClampRadius(float r)
	{
		return MR.ClampFloat(r, 5, MR.RADIUS_MAX);
	}
}

// ------------------------------------------------------------- vehicle host
class MR_VehicleHost extends MR_DeviceBase
{
	protected CarScript m_Car;

	void MR_VehicleHost(CarScript car)
	{
		MR_Init();
		m_Car = car;
	}

	CarScript GetCar()
	{
		return m_Car;
	}

	override Object MR_GetAudioObject()
	{
		return m_Car;
	}

	override bool MR_CanPlay()
	{
		if (!m_Car)
			return false;
		if (m_Car.IsRuined())
			return false;

		// 1) плеер ИЛИ кассета/диск в слоте машины - полноценный источник музыки.
		//    (Раньше в слот можно было положить только целый плеер.)
		if (MR_MediaItems.AlbumInVehicle(m_Car) != string.Empty)
			return true;
		if (MR_MediaItems.DockedInVehicle(m_Car))
			return true;   // положили носитель - причина "почему тихо" ниже

		// 2) otherwise the classic rule: a radio must physically be inside the
		//    vehicle (or the server can be configured to skip that requirement).
		if (!MR_ConfigRuntime.Settings().m_RequireRadioItemInCargo)
			return true;

		EntityAI radio;
		if (!MR_VehicleRadio.HasRadioInCargo(m_Car, radio))
			return false;

		if (!MR_VehicleRadio.HasPower(radio))
			return false;

		return true;
	}

	override string MR_BlockReason()
	{
		if (!m_Car)
			return "vehicle missing";
		if (m_Car.IsRuined())
			return "vehicle is ruined";

		// A docked player exists but is not playable: say exactly what is wrong
		// ("docked deck: no memory card") instead of a generic message.
		if (MR_MediaItems.DockedInVehicle(m_Car))
		{
			string status = MR_MediaItems.VehicleStatusText(m_Car);
			if (MR_MediaItems.AlbumInVehicle(m_Car) == string.Empty)
				return status;
			return string.Empty;
		}

		if (!MR_ConfigRuntime.Settings().m_RequireRadioItemInCargo)
			return string.Empty;

		EntityAI radio;
		if (!MR_VehicleRadio.HasRadioInCargo(m_Car, radio))
			return "no radio in the vehicle (put a radio in the cargo)";

		if (!MR_VehicleRadio.HasPower(radio))
			return "the radio's battery is empty";

		return string.Empty;
	}

	// A car stereo is louder than a handheld and dies with the engine off only
	// if the battery is flat - the latter is handled by the radio item.
	override float MR_GetEffectiveRadius()
	{
		return MR.ClampFloat(super.MR_GetEffectiveRadius(), 5, MR.RADIUS_MAX);
	}
}

// ============================================================================
//  MR_ConfigRuntime : process wide access to the active settings.
//  Layer: 4_World  (filled by the mission classes on both sides)
// ============================================================================
class MR_ConfigRuntime
{
	protected static ref MR_Settings m_Settings;

	static MR_Settings Settings()
	{
		if (!m_Settings)
			m_Settings = new MR_Settings();
		return m_Settings;
	}

	static void Set(MR_Settings s)
	{
		if (s)
			m_Settings = s;
	}

	static bool IsServer()
	{
		return GetGame().IsServer();
	}

	static bool IsClient()
	{
		return GetGame().IsClient();
	}

	// True on a listen server / single player where one process is both.
	static bool IsLocalHost()
	{
		return (!GetGame().IsMultiplayer() || (GetGame().IsServer() && GetGame().IsClient()));
	}
}
