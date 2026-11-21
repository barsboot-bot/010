// ============================================================================
//  MusicRadio - Core
//  MR_Decks.c : what PLAYS the carrier. One file, one place per class.
//  Layer: 4_World
//
//  Same rule as MR_Carriers.c: the classes come from config.cpp, so the script
//  side is a plain "class" with the same name - never "modded class".
//
//  Contents:
//    MR_CassettePlayer_Base      - readiness, slot lock, inserted carrier
//    MR_MiniDiscPlayer_Base      - same, MR_MiniDisc slot
//    MR_CassettePlayer           - concrete device + MR_DeckHost registration
//    MR_MiniDiscPlayer           - same for the MD player
//
//  Slot lock comes from VOS_Radio (SetSlotLock while the motor spins); the host
//  object and resume come from OMD_CarRadio. Both live here only.
// ============================================================================


// ============================================================================
//  MR_MediaBay : "что вставлено" - одна логика на ВСЕ устройства.
// ----------------------------------------------------------------------------
//  20261174: переносная рация стала таким же плеером, как дека (слоты
//  MR_Cassette / MR_MiniDisc / MR_MemoryCard добавлены конфигом), поэтому
//  проверки вынесены сюда и вызываются из ДВУХ мест:
//      MR_DeckBase        (кассетный плеер / MD-плеер)
//      modded PersonalRadio (ванильная рация)
//
//  Почему не наследование: PersonalRadio приходит из ванильных скриптов, и
//  переписать её родителя мы не можем - только дописать методы. Поэтому
//  статические функции, которым объект передают параметром.
//
//  Правила (единый контракт всего мода):
//      носитель -> карта памяти -> батарейка -> можно играть
//  Порядок проверки намеренно именно такой: сначала "что играем", потом
//  "чем помним", потом "чем питаемся" - так причина в чате всегда конкретная.
// ============================================================================
class MR_MediaBay
{
	// Кассета/диск, вставленные в устройство. "" -> null.
	static EntityAI Carrier(EntityAI device, string slotName)
	{
		if (!device)
			return null;
		return device.FindAttachmentBySlotName(slotName);
	}

	// Первый непустой носитель из списка слотов: рация принимает и кассету,
	// и мини-диск. Так порядок слотов задаёт приоритет, если положили оба.
	static EntityAI AnyCarrier(EntityAI device, TStringArray slots)
	{
		if (!device || !slots)
			return null;
		foreach (string slot : slots)
		{
			EntityAI found = Carrier(device, slot);
			if (found)
				return found;
		}
		return null;
	}

	// Карта памяти: сначала В НОСИТЕЛЕ (так её видно там, куда вставили),
	// потом в самом устройстве (совместимость со старыми сборками).
	static bool HasCard(EntityAI device, EntityAI carrier)
	{
		int slotId = InventorySlots.GetSlotIdFromString("MR_MemoryCard");
		if (slotId >= 0)
		{
			if (carrier && carrier.GetInventory())
			{
				if (carrier.GetInventory().FindAttachment(slotId))
					return true;
			}
			if (device && device.GetInventory())
			{
				if (device.GetInventory().FindAttachment(slotId))
					return true;
			}
			return false;
		}

		// Слот неизвестен этой сборке: старое поведение через имя.
		if (carrier && carrier.FindAttachmentBySlotName("MR_MemoryCard"))
			return true;
		if (device && device.FindAttachmentBySlotName("MR_MemoryCard"))
			return true;
		return false;
	}

	static bool HasBattery(EntityAI device)
	{
		if (!device)
			return false;
		if (device.FindAttachmentBySlotName("BatteryD"))
			return true;
		// Рация ванильно питается от Battery9V: у неё слот BatteryD есть, но
		// заспавненные старые предметы могли прийти с 9V - не отказываем.
		if (device.FindAttachmentBySlotName("Battery9V"))
			return true;
		return false;
	}

	// Полная проверка готовности. reason = "" когда всё на месте.
	static bool IsReady(EntityAI device, EntityAI carrier, out string reason)
	{
		reason = string.Empty;
		if (!device)
		{
			reason = "device missing";
			return false;
		}
		if (device.IsRuined())
		{
			reason = "the device is ruined";
			return false;
		}
		if (!carrier)
		{
			reason = "no cassette or disc inserted";
			return false;
		}
		if (!HasCard(device, carrier))
		{
			reason = "no memory card";
			return false;
		}
		if (!HasBattery(device))
		{
			reason = "no battery";
			return false;
		}
		return true;
	}

	// Альбом вставленного носителя, "" если играть нечего.
	static string Album(EntityAI device, EntityAI carrier, out string reason)
	{
		if (!IsReady(device, carrier, reason))
			return string.Empty;
		return MR_MediaItems.AlbumOf(carrier.GetType());
	}
}

// ============================================================================
//  MR_DeckBase : common parent for the cassette deck and the MD player.
// ----------------------------------------------------------------------------
//  Same story as MR_CarrierBase (log 19:18, stage World):
//      mr_decks.c,76:  Function 'EEInit'       is marked as override, but ...
//      mr_decks.c,130: Function 'OnSwitchOn'   is marked as override, but ...
//      mr_decks.c,136: Function 'OnSwitchOff'  is marked as override, but ...
//      mr_decks.c,143: Function 'EEDelete'     is marked as override, but ...
//
//  The class came from config.cpp only, so in the SCRIPT tree it had no base:
//  "override" had nothing to override and "super." nothing to call. Extending
//  ItemBase (which vanilla defines as `typedef ItemBase Inventory_Base`) gives
//  the script class the same parent the config has, and those four callbacks
//  exist for real. OnSwitchOn/OnSwitchOff are ItemBase/EnergyManager callbacks -
//  the error said "no function in base class" because there was no base at all,
//  not because the names are wrong.
//
//  Everything both devices share lives here once: readiness, the slot lock
//  (idea from VOS_Radio), inserted carrier. Subclasses only name their slot.
// ============================================================================
class MR_DeckBase extends ItemBase
{
	// Which inventory slot holds the carrier. Overridden per device.
	string MR_CarrierSlotName()
	{
		return "MR_Cassette";
	}

	// The inserted cassette / mini disc, or null.
	EntityAI MR_InsertedCarrier()
	{
		return FindAttachmentBySlotName(MR_CarrierSlotName());
	}

	EntityAI MR_GetInsertedCard()
	{
		return FindAttachmentBySlotName("MR_MemoryCard");
	}

	// Карта памяти: сначала в носителе, потом (для совместимости со старыми
	// сборками и уже заспавненными предметами) в самом плеере.
	bool MR_HasMemoryCard()
	{
		// Одна логика на деку и рацию - MR_MediaBay (карта в носителе или в
		// самом устройстве). Раньше проверка была скопирована здесь.
		return MR_MediaBay.HasCard(this, MR_InsertedCarrier());
	}

	bool MR_HasBattery()
	{
		// BatteryD - ванильная D-батарейка; девятка принимается для старых
		// заспавненных предметов (см. MR_MediaBay.HasBattery).
		return MR_MediaBay.HasBattery(this);
	}

	// Everything the deck needs before it can make sound.
	// Порядок (единый для всех устройств, см. MR_MediaBay): носитель -> карта
	// -> батарейка. Карта живёт В НОСИТЕЛЕ: так её видно там, куда вставили,
	// и одна карта переставляется между кассетами.
	bool MR_IsReady(out string reason)
	{
		return MR_MediaBay.IsReady(this, MR_InsertedCarrier(), reason);
	}

	// Album that would play right now ("" when not ready).
	string MR_CurrentAlbum()
	{
		string reason;
		return MR_MediaBay.Album(this, MR_InsertedCarrier(), reason);
	}

	// -1 when the slot name is unknown on this build (then locking is a no-op).
	int MR_CarrierSlotId()
	{
		return InventorySlots.GetSlotIdFromString(MR_CarrierSlotName());
	}

	void MR_LockCarrierSlot()
	{
		int slotId = MR_CarrierSlotId();
		if (slotId < 0)
			return;

		GameInventory inv = GetInventory();
		if (!inv)
			return;

		inv.SetSlotLock(slotId, true);
	}

	void MR_UnlockCarrierSlot()
	{
		int slotId = MR_CarrierSlotId();
		if (slotId < 0)
			return;

		GameInventory inv = GetInventory();
		if (!inv)
			return;

		inv.SetSlotLock(slotId, false);
	}

	bool MR_CarrierSlotLocked()
	{
		int slotId = MR_CarrierSlotId();
		if (slotId < 0)
			return false;

		GameInventory inv = GetInventory();
		if (!inv)
			return false;

		return inv.GetSlotLock(slotId);
	}

	override void EEInit()
	{
		super.EEInit();

		// Unlock later, not now: the inventory does not exist yet (VOS_Radio
		// hit exactly this - GetSlotLock returned garbage inside EEInit).
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MR_UnlockCarrierSlot, MR.SLOT_UNLOCK_DELAY_MS, false);
	}

	// Energy manager callbacks: on = the deck is spinning.
	override void OnSwitchOn()
	{
		super.OnSwitchOn();
		MR_LockCarrierSlot();
	}

	override void OnSwitchOff()
	{
		super.OnSwitchOff();
		// Delayed for the same reason as in EEInit.
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MR_UnlockCarrierSlot, MR.SLOT_UNLOCK_DELAY_MS, false);
	}

	override void EEDelete(EntityAI parent)
	{
		MR_UnlockCarrierSlot();
		super.EEDelete(parent);
	}
};


class MR_CassettePlayer_Base extends MR_DeckBase
{
	override string MR_CarrierSlotName()
	{
		return "MR_Cassette";
	}

	// Name kept from the first version: the server engine calls it.
	EntityAI MR_GetInsertedCassette()
	{
		return MR_InsertedCarrier();
	}
}


class MR_MiniDiscPlayer_Base extends MR_DeckBase
{
	override string MR_CarrierSlotName()
	{
		return "MR_MiniDisc";
	}

	EntityAI MR_GetInsertedDisc()
	{
		return MR_InsertedCarrier();
	}
}


class MR_CassettePlayer extends MR_CassettePlayer_Base
{
	override void EEInit()
	{
		super.EEInit();
		MR_Registry.Register(MR_HostType.HOST_PLAYER, this);
	}

	// Slot the device reports to the UI (one carrier, one album).
	string MR_CarrierLabel()
	{
		EntityAI tape = MR_GetInsertedCassette();
		// без "?:": Enforce Script спотыкается на тернарнике внутри return
		if (tape)
			return MR_MediaItems.CarrierLabel(tape);
		return "(empty)";
	}

	protected ref MR_DeckHost m_MRDeckHost;

	MR_DeckHost MR_GetDeckHost()
	{
		if (!m_MRDeckHost)
			m_MRDeckHost = new MR_DeckHost(this);
		return m_MRDeckHost;
	}

	bool MR_DeckIsPlaying()
	{
		return (m_MRDeckHost && m_MRDeckHost.MR_IsPlaying());
	}
}


class MR_MiniDiscPlayer extends MR_MiniDiscPlayer_Base
{
	override void EEInit()
	{
		super.EEInit();
		MR_Registry.Register(MR_HostType.HOST_PLAYER, this);
	}

	string MR_CarrierLabel()
	{
		EntityAI disc = MR_GetInsertedDisc();
		if (disc)
			return MR_MediaItems.CarrierLabel(disc);
		return "(empty)";
	}

	protected ref MR_DeckHost m_MRDeckHost;

	MR_DeckHost MR_GetDeckHost()
	{
		if (!m_MRDeckHost)
			m_MRDeckHost = new MR_DeckHost(this);
		return m_MRDeckHost;
	}

	bool MR_DeckIsPlaying()
	{
		return (m_MRDeckHost && m_MRDeckHost.MR_IsPlaying());
	}
}
