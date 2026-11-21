// ============================================================================
//  MusicRadio - Core
//  MR_MediaItems.c : the album index - item class name -> album id.
//  Layer: 4_World
//
//  The item classes themselves (cassettes, mini discs, cards, players) live in
//  MR_Carriers.c and MR_Decks.c: one class, one file. This file only answers
//  "which album does class X play" and holds the engine-side hooks
//  (CarScript / PlayerBase / PersonalRadio).
//
//  The physical side of the library. Two rules keep this cheap:
//
//   1. a carrier is IDENTITY ONLY - it says which album to play, never where
//      the audio is. Audio stays in the media pack (CfgSoundSets), so a tape
//      costs 40 g of inventory space and zero bytes of .ogg;
//   2. one carrier = one album. Two tapes never share an album, and a player
//      holds exactly one carrier, so "what is playing" always has one answer.
//
//  Link between item and album:
//      class MR_Cassette_Rock01  ->  "rock01"   (see MR_MediaItems.AlbumOf)
//  That id is what the DB mod uses as its album key in index.json.
// ============================================================================
// NOTE on "modded": it injects only into classes the ENGINE declared (ItemBase,
// CarScript, PersonalRadio...). MR_* items come from config.cpp (CfgVehicles),
// so they take a plain "class" - "modded class MR_CassetteCase_Base" fails with
//   Unknown type 'MR_CassetteCase_Base'   (log 18:12)
// That is why the carriers and decks now live in their own files without it.

// ---------------------------------------------------------------------------
//  Static helpers: class name -> album id.
// ---------------------------------------------------------------------------
class MR_MediaItems
{
	// Prefixes are part of the naming contract, keep them in one place.
	static const string PREFIX_CASSETTE = "MR_Cassette_";
	static const string PREFIX_MINIDISC = "MR_MiniDisc_";
	static const string PREFIX_CARD     = "MR_MemoryCard";

	// "MR_Cassette_Rock01" -> "rock01";  "" when the class is not a carrier.
	static string AlbumOf(string className)
	{
		if (className.IndexOf(PREFIX_CASSETTE) == 0)
			return Lower(className.Substring(PREFIX_CASSETTE.Length(), className.Length() - PREFIX_CASSETTE.Length()));

		if (className.IndexOf(PREFIX_MINIDISC) == 0)
			return Lower(className.Substring(PREFIX_MINIDISC.Length(), className.Length() - PREFIX_MINIDISC.Length()));

		return string.Empty;
	}

	static string Lower(string s)
	{
		// ToLower() меняет строку НА МЕСТЕ и возвращает int (длину), поэтому
		// "return s.ToLower()" - это возврат int из функции со string:
		//   Incompatible parameter '#return'   (лог 20:47, строка 53)
		s.ToLower();
		return s;
	}

	// True for the base classes that must never spawn or be inserted.
	static bool IsAbstractCarrier(string className)
	{
		return (className.IndexOf("_Base") > 0);
	}

	static bool IsCassette(EntityAI item)
	{
		if (!item)
			return false;
		string t = item.GetType();
		return (t.IndexOf(PREFIX_CASSETTE) == 0 && !IsAbstractCarrier(t));
	}

	static bool IsMiniDisc(EntityAI item)
	{
		if (!item)
			return false;
		string t = item.GetType();
		return (t.IndexOf(PREFIX_MINIDISC) == 0 && !IsAbstractCarrier(t));
	}

	static bool IsMemoryCard(EntityAI item)
	{
		if (!item)
			return false;
		return (item.GetType().IndexOf(PREFIX_CARD) == 0 && !IsAbstractCarrier(item.GetType()));
	}

	// Что вообще может лежать в слоте машины: плеер (дека/MD) ИЛИ сам носитель
	// (кассета/мини-диск). Кассету в бардачок кладут чаще, чем целый плеер,
	// поэтому поддерживаются оба варианта - как с рацией в багажнике.
	static bool IsCarDockItem(EntityAI item)
	{
		if (!item)
			return false;
		return (IsCarrierPlayer(item) || IsCarrier(item));
	}

	// Album that the docked player OR the cassette lying in the car provides.
	// "" when nothing is docked or it brings no audio of its own.
	static string AlbumInVehicle(EntityAI vehicle)
	{
		EntityAI docked = DockedInVehicle(vehicle);
		if (!docked)
			return string.Empty;

		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(docked);
		if (deck)
			return deck.MR_CurrentAlbum();

		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(docked);
		if (md)
			return md.MR_CurrentAlbum();

		// A bare cassette/MD: the deck lives in the car stereo - its album is
		// the cassette's own album key (MR_Cassette_Rock01 -> "rock01").
		if (IsCarrier(docked))
			return AlbumOf(docked.GetType());

		return string.Empty;
	}

	// Human readable state of a car stereo, for the UI and the server log.
	static string VehicleStatusText(EntityAI vehicle)
	{
		EntityAI docked = DockedInVehicle(vehicle);
		if (!docked)
			return "no deck docked (use a cassette or a player in the dash slot)";

		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(docked);
		if (deck)
		{
			string why;
			if (!deck.MR_IsReady(why))
				return "docked deck: " + why;
			return "docked deck: album '" + deck.MR_CurrentAlbum() + "'";
		}

		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(docked);
		if (md)
		{
			string why2;
			if (!md.MR_IsReady(why2))
				return "docked MD: " + why2;
			return "docked MD: album '" + md.MR_CurrentAlbum() + "'";
		}

		if (IsCarrier(docked))
		{
			string album = AlbumOf(docked.GetType());
			if (album == string.Empty)
				return "docked cassette has no album";
			return "docked cassette: album '" + album + "'";
		}

		return "docked item is not a music carrier";
	}

	// Носитель (кассета/диск) или плеер, вставленный в слот машины, или null.
	// Слот один (MR_DashSlot): что положили, то и играет.
	static EntityAI DockedInVehicle(EntityAI vehicle)
	{
		if (!vehicle)
			return null;
		return vehicle.FindAttachmentBySlotName("MR_DashSlot");
	}

	static bool IsCarrierPlayer(EntityAI item)
	{
		if (!item)
			return false;
		return (item.IsInherited(MR_CassettePlayer_Base) || item.IsInherited(MR_MiniDiscPlayer_Base));
	}

	static bool IsCarrier(EntityAI item)
	{
		return (IsCassette(item) || IsMiniDisc(item));
	}

	// Human readable name of the album, used by the UI and the chat notice.
	static string CarrierLabel(EntityAI item)
	{
		if (!item)
			return "(no media)";
		string album = AlbumOf(item.GetType());
		if (album == string.Empty)
			return item.GetType();
		return album;
	}
}

// ---------------------------------------------------------------------------
//  Carriers: refuse to be "used" as a generic item, and log their identity so
//  a server admin can trace a dupe back to a class name.
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  Players: what is in the deck right now.
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  Decks are hosts: they register like radios so the client can find the object
//  a PLAY RPC points at, and so a car can use one as its music source.
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  Cars: what is docked on the dash, and can it play.
// ---------------------------------------------------------------------------
modded class CarScript
{
	EntityAI MR_GetDockedPlayer()
	{
		return MR_MediaItems.DockedInVehicle(this);
	}

	bool MR_HasDockedPlayer()
	{
		return (MR_GetDockedPlayer() != null);
	}

	// Album the docked player would start with ("" if none).
	string MR_DockedAlbum()
	{
		return MR_MediaItems.AlbumInVehicle(this);
	}

	string MR_DashStatus()
	{
		return MR_MediaItems.VehicleStatusText(this);
	}
}

modded class PlayerBase
{
	// First cassette deck / MD player the player carries (hands first, then
	// anywhere in the clothing / backpack). Null when there is none.
	EntityAI MR_FindMediaPlayer()
	{
		EntityAI inHands = GetHumanInventory().GetEntityInHands();
		if (MR_MediaItems.IsCarrierPlayer(inHands))
			return inHands;

		GameInventory inv = GetInventory();
		if (!inv)
			return null;

		array<EntityAI> items = new array<EntityAI>();
		inv.EnumerateInventory(InventoryTraversalType.PREORDER, items);
		foreach (EntityAI item : items)
		{
			if (MR_MediaItems.IsCarrierPlayer(item))
				return item;
		}
		return null;
	}

	// Album that would play from the carried deck ("" when nothing is loaded).
	// Что играет у игрока В РУКАХ: рация (носитель в ней) или плеер.
	// "" - ничего; тогда меню покажет "вставь кассету".
	string MR_HeldAlbum()
	{
		EntityAI inHands = GetHumanInventory().GetEntityInHands();
		PersonalRadio radio = PersonalRadio.Cast(inHands);
		if (radio)
			return radio.MR_CurrentAlbum();
		return string.Empty;
	}

	string MR_CarriedAlbum()
	{
		// Рация в руках имеет приоритет: она ближе, чем плеер в рюкзаке.
		string held = MR_HeldAlbum();
		if (held != string.Empty)
			return held;

		EntityAI deckObj = MR_FindMediaPlayer();
		if (!deckObj)
			return string.Empty;

		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(deckObj);
		if (deck)
			return deck.MR_CurrentAlbum();

		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(deckObj);
		if (md)
			return md.MR_CurrentAlbum();

		return string.Empty;
	}
}

// ---------------------------------------------------------------------------
//  Handheld radio = a player too (20261174).
// ---------------------------------------------------------------------------
//  Слоты MR_MemoryCard / MR_Cassette / MR_MiniDisc добавлены конфигом
//  (CfgVehicles -> PersonalRadio : Transmitter_Base, attachments[] += ...),
//  поэтому рация перестала быть "устройством, играющим из воздуха": звук
//  идёт с вставленной кассеты/диска, как у деки. Проверки - через MR_MediaBay,
//  то есть РОВНО те же, что у плеера (носитель -> карта -> батарейка).
//
//  Класс ванильный, поэтому только дописываем методы; родителя переписать
//  нельзя (это дало бы 'Member already defined' на CfgVehicles).
modded class PersonalRadio
{
	// Слоты в порядке приоритета: если вставили и кассету, и диск.
	static void MR_CarrierSlots(out TStringArray slots)
	{
		slots = new TStringArray();
		slots.Insert("MR_Cassette");
		slots.Insert("MR_MiniDisc");
	}

	// Кассета / мини-диск в рации (null, если пусто).
	EntityAI MR_InsertedCarrier()
	{
		TStringArray slots;
		MR_CarrierSlots(slots);
		return MR_MediaBay.AnyCarrier(this, slots);
	}

	EntityAI MR_GetInsertedCassette()
	{
		return MR_MediaBay.Carrier(this, "MR_Cassette");
	}

	EntityAI MR_GetInsertedDisc()
	{
		return MR_MediaBay.Carrier(this, "MR_MiniDisc");
	}

	// A memory card is storage the player inserts first; without it the radio
	// is silent, exactly like the deck.
	EntityAI MR_GetMemoryCard()
	{
		EntityAI carrier = MR_InsertedCarrier();
		if (carrier)
		{
			int slotId = InventorySlots.GetSlotIdFromString("MR_MemoryCard");
			if (slotId >= 0 && carrier.GetInventory())
			{
				EntityAI inCarrier = carrier.GetInventory().FindAttachment(slotId);
				if (inCarrier)
					return inCarrier;
			}
		}
		return MR_MediaBay.Carrier(this, "MR_MemoryCard");
	}

	bool MR_HasMemoryCard()
	{
		return (MR_GetMemoryCard() != null);
	}

	bool MR_HasBattery()
	{
		return MR_MediaBay.HasBattery(this);
	}

	bool MR_IsReady(out string reason)
	{
		return MR_MediaBay.IsReady(this, MR_InsertedCarrier(), reason);
	}

	// Альбом вставленного носителя; "" если играть нечего.
	string MR_CurrentAlbum()
	{
		string reason;
		return MR_MediaBay.Album(this, MR_InsertedCarrier(), reason);
	}

	// Почему тихо - одной строкой для чата и лога.
	string MR_MediaStatus()
	{
		string reason;
		if (MR_IsReady(reason))
			return "radio: album '" + MR_CurrentAlbum() + "'";
		return "radio: " + reason;
	}

	// Shown by the UI when the radio is opened without a card.
	string MR_CardHint()
	{
		if (MR_HasMemoryCard())
			return string.Empty;
		return "no memory card - insert one to use the music library";
	}
}
