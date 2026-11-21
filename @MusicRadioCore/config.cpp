// =============================================================================
//  MusicRadio - Core (mod #2 of 3: shared logic, server + client)
//  config.cpp   -   version 1.1.0
//
//  PBO layout (source folder for AddonBuilder):
//      config.cpp
//      Scripts/3_Game/MusicRadio/*.c
//      Scripts/4_World/MusicRadio/*.c
//      Scripts/5_Mission/MusicRadio/*.c
//      gui/layouts/*.layout
//
//  Build: python3 tools/stage_pbo.py  ->  tools\build_pbo.bat
//  PBO prefix must be "MusicRadioCore" - the same name as the CfgMods key.
//
//  MEDIA CARRIERS (added in 1.1.0)
//  -----------------------------------------------------------------
//    MR_MemoryCard_Base      SD card for the radio   - EMPTY, holds nothing
//    MR_CassetteCase_Base    album identity only (which SoundSet to play)
//    MR_CassettePlayer       head unit: battery + memory card + one tape
//    MR_MiniDiscPlayer       same, for MDs (one album per disc)
//
//  Rule of the project: ONE CARRIER = ONE ALBUM, and NO audio inside a carrier
//  item. Audio stays in @MusicRadioDB (CfgSoundSets). A tape is just a class
//  name like MR_Cassette_Rock01; the code maps it to the album id and resolves
//  the SoundSets by the usual provider|trackId contract.
//
//  Models are borrowed from vanilla on purpose - the mod keeps working without
//  custom art, and nothing here has to be re-binarized when the game updates.
//  Replace the paths when your own .p3d files exist; class names stay the same.
// =============================================================================

class CfgPatches
{
	class MusicRadioCore
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = { "DZ_Data", "DZ_Sounds_Effects", "DZ_Gear_Consumables" };
	};
};

class CfgMods
{
	class MusicRadioCore
	{
		dir = "MusicRadioCore";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "MusicRadio - Core";
		credits = "MusicRadio project";
		author = "MusicRadio project";
		authorID = "0";
		version = "1.1.0";
		extra = 0;
		type = "mod";
		dependencies[] = { "Game", "World", "Mission" };

		class defs
		{
			// Own keys, own tab in the settings menu. Nothing in the vanilla
			// input files is patched, so this never conflicts with another mod
			// that also adds actions (that is how FoXyRadio, OMD and VOS_Radio
			// all ended up fighting over the same "kJ" key).
			inputs = "MusicRadioCore/inputs.xml";

			class gameScriptModule
			{
				value = "";
				files[] = { "MusicRadioCore/Scripts/3_Game" };
			};
			class worldScriptModule
			{
				value = "";
				files[] = { "MusicRadioCore/Scripts/4_World" };
			};
			class missionScriptModule
			{
				value = "";
				files[] = { "MusicRadioCore/Scripts/5_Mission" };
			};
		};
	};
};

class CfgSoundSets
{
	// Fallback used when a media pack does not declare its own SoundSet.
	// No base class on purpose: a vanilla SoundSet name ("personalradio_...")
	// is not guaranteed to exist in every DayZ build, and inheriting from a
	// missing class fails the whole config (CfgConvert: "Undefined base
	// class"). An empty standalone SoundSet is valid and simply plays nothing.
	class MR_RadioFallback_SoundSet
	{
		soundShaders[] = {};
		volumeFactor = 0.35;
		spatial = 1;
		loop = 0;
	};
};

// =============================================================================
//  CARRIERS
// =============================================================================
class CfgVehicles
{
	class Inventory_Base;

	// --------------------------------------------------------------- memory card
	// The radio's storage. Deliberately empty: music is NOT stored on it (that
	// would mean shipping audio twice - the project keeps audio only in the
	// media pack). What matters now is that the card exists, fits a slot and can
	// be inserted/removed, which is the inventory gameplay players expect.
	class MR_MemoryCard_Base : Inventory_Base
	{
		scope = 0;
		displayName = "$STR_MR_MemoryCard";
		descriptionShort = "$STR_MR_MemoryCard_desc";
		model = "\DZ\items\Misc\BoxOfMatches.p3d";
		rotationFlags = 17;
		weight = 10;
		itemSize[] = { 1, 1 };
		inventorySlot[] = { "MR_MemoryCard" };
		absorbency = 0.1;

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 60;
					healthLevels[] =
					{
						{ 1.0, {} },
						{ 0.7, {} },
						{ 0.5, {} },
						{ 0.3, {} },
						{ 0.0, {} }
					};
				};
			};
		};
	};

	class MR_MemoryCard_01 : MR_MemoryCard_Base
	{
		scope = 2;
		displayName = "$STR_MR_MemoryCard";
		descriptionShort = "$STR_MR_MemoryCard_desc";
	};

	// ------------------------------------------------------------------- cassette
	// Album identity ONLY. No audio, no url, no file. The class name is the key:
	// MR_Cassette_Rock01 -> album "rock01" in the media pack index.


	// -------------------------------------------------------------- cassette deck
	// A media player. Two attachment slots: memory card + one tape. It is NOT a
	// transceiver (the vanilla radio keeps that job), it only turns a tape into
	// sound. The code side lives in MR_MediaItems.c (4_World).
	class MR_CassettePlayer_Base : Inventory_Base
	{
		scope = 0;
		displayName = "$STR_MR_CassettePlayer";
		descriptionShort = "$STR_MR_CassettePlayer_desc";
		model = "\DZ\gear\consumables\WalkieTalkie.p3d";
		animClass = "Knife";
		rotationFlags = 17;
		weight = 550;
		itemSize[] = { 2, 2 };
		// BatteryD - ванильный слот для большой батарейки (BatteryD).
		// У самого предмета нет EnergyManager, поэтому питание проверяется
		// только наличием батарейки в слоте (см. MR_HasBattery).
		attachments[] = { "MR_MemoryCard", "MR_Cassette", "BatteryD" };
		class EnergyManager
		{
			// Without this block OnSwitchOn/OnSwitchOff in MR_Decks.c never fire
			// (they are EnergyManager callbacks), so the deck would never report
			// "spinning" and the slot lock would never engage.
			hasIcon = 1;
			energyUsagePerSecond = 0.002;
			plugType = 1;
			attachmentAction = 1;
			updateInterval = 3;
			wetnessExposure = 0.5;
			autoSwitchOffWhenInCargo = 1;
		};

		absorbency = 0.1;

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] =
					{
						{ 1.0, {} },
						{ 0.7, {} },
						{ 0.5, {} },
						{ 0.3, {} },
						{ 0.0, {} }
					};
				};
			};
		};
	};

	class MR_CassettePlayer : MR_CassettePlayer_Base
	{
		scope = 2;
		displayName = "$STR_MR_CassettePlayer";
		descriptionShort = "$STR_MR_CassettePlayer_desc";
	};

	class MR_CassettePlayer_Black : MR_CassettePlayer
	{
		scope = 2;
		displayName = "$STR_MR_CassettePlayerBlack";
	};

	// ------------------------------------------------------------------ MD player
	// Same idea as the cassette deck: memory card + one disc. Kept a separate
	// class so tapes and discs can never be mixed up in one device.
	class MR_MiniDiscPlayer_Base : Inventory_Base
	{
		scope = 0;
		displayName = "$STR_MR_MiniDiscPlayer";
		descriptionShort = "$STR_MR_MiniDiscPlayer_desc";
		model = "\DZ\gear\consumables\WalkieTalkie.p3d";
		animClass = "Knife";
		rotationFlags = 17;
		weight = 400;
		itemSize[] = { 2, 2 };
		attachments[] = { "MR_MemoryCard", "MR_MiniDisc", "BatteryD" };
		class EnergyManager
		{
			// Without this block OnSwitchOn/OnSwitchOff in MR_Decks.c never fire
			// (they are EnergyManager callbacks), so the deck would never report
			// "spinning" and the slot lock would never engage.
			hasIcon = 1;
			energyUsagePerSecond = 0.002;
			plugType = 1;
			attachmentAction = 1;
			updateInterval = 3;
			wetnessExposure = 0.5;
			autoSwitchOffWhenInCargo = 1;
		};

		absorbency = 0.1;

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] =
					{
						{ 1.0, {} },
						{ 0.7, {} },
						{ 0.5, {} },
						{ 0.3, {} },
						{ 0.0, {} }
					};
				};
			};
		};
	};

	class MR_MiniDiscPlayer : MR_MiniDiscPlayer_Base
	{
		scope = 2;
		displayName = "$STR_MR_MiniDiscPlayer";
		descriptionShort = "$STR_MR_MiniDiscPlayer_desc";
	};

	// One class per album is GENERATED (tools/generate_db_config.py) between the
	// markers below: the class name is the album key, so it must never be typed
	// by hand. Add an album to the media pack and run tools/import_music.bat.
	class MR_CassetteCase_Base : Inventory_Base
	{
		scope = 0;
		displayName = "$STR_MR_CassetteCase";
		descriptionShort = "$STR_MR_CassetteCase_desc";
		// MusicRadioCore\data\mr_cassette.p3d once the mesh is exported.
		// hiddenSelections below is the texture hook: the selection for the
		// shell + label is called "mr_cassette" inside that model.
		model = "\DZ\gear\consumables\sewing_kit.p3d";
		rotationFlags = 17;
		weight = 40;
		itemSize[] = { 2, 1 };
		// Кассета - это НОСИТЕЛЬ: в неё вставляется карта памяти, поэтому у неё
		// есть слот MR_MemoryCard (как у кассетного плеера). Без этого карту было
		// физически некуда вставить, и плеер навсегда оставался с "no memory card".
		inventorySlot[] = { "MR_Cassette" };
		attachments[] = { "MR_MemoryCard" };
		hiddenSelections[] = { "mr_cassette" };
		absorbency = 0.2;

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] =
					{
						{ 1.0, {} },
						{ 0.7, {} },
						{ 0.5, {} },
						{ 0.3, {} },
						{ 0.0, {} }
					};
				};
			};
		};
	};

	// ---------------------------------------------------------------- mini disc
	class MR_MiniDisc_Base : Inventory_Base
	{
		scope = 0;
		displayName = "$STR_MR_MiniDisc";
		descriptionShort = "$STR_MR_MiniDisc_desc";
		// MusicRadioCore\data\mr_minidisc.p3d once the mesh is exported.
		model = "\DZ\items\map\compass\Compass.p3d";
		rotationFlags = 17;
		weight = 30;
		itemSize[] = { 1, 1 };
		inventorySlot[] = { "MR_MiniDisc" };
		attachments[] = { "MR_MemoryCard" };
		hiddenSelections[] = { "mr_minidisc" };
		absorbency = 0.1;

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[] =
					{
						{ 1.0, {} },
						{ 0.7, {} },
						{ 0.5, {} },
						{ 0.3, {} },
						{ 0.0, {} }
					};
				};
			};
		};
	};

	// ------------------------------------------------- handheld radio slots
	// Рация = такой же плеер, как дека (20261174): в неё вставляют
	// кассету/диск и карту памяти, звук идёт со вставленного носителя.
	//
	// ВАЖНО ПРО ПАРЕНТА. Ванильная рация объявлена как
	//     PersonalRadio : Transmitter_Base      (DZ\gear\radio\config.cpp)
	// и attachments[] живёт именно на этом уровне. Array += дописывается к
	// массиву, объявленному НА УРОВНЕ ПРЯМОГО ПАРЕНТА, поэтому здесь повторён
	// прямой парент Transmitter_Base, а не PersonalRadio: иначе получили бы
	// '/CfgVehicles.PersonalRadio: Member already defined' - ровно та ошибка,
	// из-за которой был вырезан CAR DASH.
	//
	// Слот BatteryD не перечисляем: он приходит от Transmitter_Base, а +=
	// только дописывает наши три.
	class Transmitter_Base;
	class PersonalRadio : Transmitter_Base
	{
		attachments[] += { "MR_MemoryCard", "MR_Cassette", "MR_MiniDisc" };

		// Инвентарь покажет отдельную группу вместо плоского списка.
		class GUIInventoryAttachmentsProps
		{
			class MR_MusicMedia
			{
				// в группе инвентаря движок берёт displayName
				displayName = "$STR_MR_Attach_Music";
				name = "$STR_MR_Attach_Music";
				description = "";
				attachmentSlots[] = { "MR_MemoryCard", "MR_Cassette", "MR_MiniDisc" };
				icon = "set:dayz_inventory image:radio";
			};
		};
	};

	// @@CARRIER_CLASSES_BEGIN@@
		// Классы носителей переехали в @MusicRadioDB/config.cpp: альбомы -
		// это контент базы, и обновлять их можно отдельно от Core.
	// @@CARRIER_CLASSES_END@@
};

class CfgSlots
{
	// Слот в машине: в него кладут кассету/диск ИЛИ плеер (один слот - что
	// положили, то и играет). selection указывает на одноимённую селекцию в
	// модели машины; если её нет, предмет всё равно вставляется, просто не
	// рисуется на торпедо.
	class Slot_MR_DashSlot
	{
		name = "MR_DashSlot";
		displayName = "$STR_MR_Slot_Dash";
		selection = "MR_DashSlot";
		ghostIcon = "missing";
	};
	class Slot_MR_MemoryCard
	{
		name = "MR_MemoryCard";
		displayName = "$STR_MR_Slot_MemoryCard";
		ghostIcon = "missing";
	};
	class Slot_MR_Cassette
	{
		name = "MR_Cassette";
		displayName = "$STR_MR_Slot_Cassette";
		ghostIcon = "missing";
	};
	class Slot_MR_MiniDisc
	{
		name = "MR_MiniDisc";
		displayName = "$STR_MR_Slot_MiniDisc";
		ghostIcon = "missing";
	};
};

// =============================================================================
//  MEDIA CARRIERS - one item class per album
// -----------------------------------------------------------------------------
//  GENERATED between the markers below by tools/generate_db_config.py: the
//  class name IS the album key (MR_Cassette_Rock01 -> album "rock01"), so it
//  must never be typed by hand. Add an album to the media pack and run
//  REBUILD.bat - the classes appear here by themselves.
//
//  The base classes (MR_CassetteCase_Base / MR_MiniDisc_Base) are declared
//  ABOVE this point on purpose: DayZ reads config.cpp top to bottom, and a
//  base declared after its child fails with 'Undefined base class'.
// =============================================================================

// =============================================================================
//  CAR DASH - вставить кассету (или плеер) прямо в машину
// -----------------------------------------------------------------------------
//  Один слот MR_DashSlot на машину. Что положили - то и играет: кассета/диск
//  играет сама (дека - это магнитола машины), плеер - через свою деку.
//
//  ПОЧЕМУ ИМЕННО ТАК (и почему так падало раньше):
//    * внутри ОДНОГО блока CfgVehicles ванильную машину второй раз объявить
//      нельзя - парсер видит два члена с одним именем
//        /CfgVehicles.CivilianSedan: Member already defined   (лог игры);
//    * в Enforce-конфиге НЕТ конструкции "дополнить чужой класс": "class X :
//      Родитель { }" создаёт новое определение, а не дополнение;
//    * поэтому машины НЕ объявляются заново. Слот живёт в CfgSlots, а
//      вкладывать в него можно любую кассету/диск/плеер (см.
//      MR_MediaItems.IsCarDockItem). Проверяется это в check_project.py.
// =============================================================================
