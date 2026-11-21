# MusicRadio - ПОЛНЫЙ ИСХОДНЫЙ КОД В ОДНОМ ФАЙЛЕ

Ревизия **20261175**. Файл собран автоматически: `python tools/make_single_md.py`.
Не редактируйте его - правки делаются в настоящих файлах, потом пересобирается
этот документ.

## Что внутри

| # | Раздел | Файлов |
|---|--------|--------|
| 1 | Core - config.cpp | 1 |
| 2 | Core - 3_Game | 13 |
| 3 | Core - 4_World | 9 |
| 4 | Core - 5_Mission | 5 |
| 5 | Core - интерфейс и локализация | 4 |
| 6 | Server - config.cpp и скрипты | 4 |
| 7 | DB - config.cpp (генерируется) | 1 |
| 8 | DB - данные медиапака | 6 |
| 9 | Файлы для сервера | 3 |
| 10 | Справочник: прокси для торпеды (не подключается) | 1 |

**Не включено осознанно:** папка `tools/` (python-инструменты, 35 файлов),
ванильный `server_files/mpmissions/types.xml` (896 КБ - в него только
добавляются 4 записи из `types_musicradio.xml`), медиа-файлы `.ogg`, текстуры
`.png/.paa` и модели `.p3d` (бинарники).

## Как этим пользоваться

* Скрипты лежат в слоях `3_Game` → `4_World` → `5_Mission`; зависимости идут
  только вниз, поэтому порядок в этом файле = порядок компиляции.
* Свои классы объявляются в `config.cpp` (CfgVehicles) и в скриптах пишутся как
  `class ... extends ...`; ванильные (`PersonalRadio`, `CarScript`,
  `PlayerBase`) - только `modded class`.
* `@MusicRadioDB/config.cpp` и блок `@@CARRIER_CLASSES@@` **генерируются**
  скриптом - руками там ничего не правится.

---

## Оглавление

- [**1. Core - config.cpp**](#1-core-configcpp)
  - [`@MusicRadioCore/config.cpp`](#musicradiocoreconfigcpp)
- [**2. Core - 3_Game**](#2-core-3_game)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Constants.c`](#musicradiocorescripts3_gamemusicradiomr_constantsc)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Net.c`](#musicradiocorescripts3_gamemusicradiomr_netc)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Json.c`](#musicradiocorescripts3_gamemusicradiomr_jsonc)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Track.c`](#musicradiocorescripts3_gamemusicradiomr_trackc)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Locator.c`](#musicradiocorescripts3_gamemusicradiomr_locatorc)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_MediaResolver.c`](#musicradiocorescripts3_gamemusicradiomr_mediaresolverc)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Library.c`](#musicradiocorescripts3_gamemusicradiomr_libraryc)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Config.c`](#musicradiocorescripts3_gamemusicradiomr_configc)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_AudioTypes.c`](#musicradiocorescripts3_gamemusicradiomr_audiotypesc)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_DeviceBase.c`](#musicradiocorescripts3_gamemusicradiomr_devicebasec)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_ClientAudio.c`](#musicradiocorescripts3_gamemusicradiomr_clientaudioc)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_CarrierState.c`](#musicradiocorescripts3_gamemusicradiomr_carrierstatec)
  - [`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_SoundMix.c`](#musicradiocorescripts3_gamemusicradiomr_soundmixc)
- [**3. Core - 4_World**](#3-core-4_world)
  - [`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Carriers.c`](#musicradiocorescripts4_worldmusicradiomr_carriersc)
  - [`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Decks.c`](#musicradiocorescripts4_worldmusicradiomr_decksc)
  - [`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_DeckResume.c`](#musicradiocorescripts4_worldmusicradiomr_deckresumec)
  - [`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_MediaItems.c`](#musicradiocorescripts4_worldmusicradiomr_mediaitemsc)
  - [`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Hosts.c`](#musicradiocorescripts4_worldmusicradiomr_hostsc)
  - [`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Registry.c`](#musicradiocorescripts4_worldmusicradiomr_registryc)
  - [`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_WorldHooks.c`](#musicradiocorescripts4_worldmusicradiomr_worldhooksc)
  - [`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_PlayerRPC.c`](#musicradiocorescripts4_worldmusicradiomr_playerrpcc)
  - [`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Animations.c`](#musicradiocorescripts4_worldmusicradiomr_animationsc)
- [**4. Core - 5_Mission**](#4-core-5_mission)
  - [`@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_ClientRPC.c`](#musicradiocorescripts5_missionmusicradiomr_clientrpcc)
  - [`@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_ClientState.c`](#musicradiocorescripts5_missionmusicradiomr_clientstatec)
  - [`@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_LibraryIndex.c`](#musicradiocorescripts5_missionmusicradiomr_libraryindexc)
  - [`@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_RadioMenu.c`](#musicradiocorescripts5_missionmusicradiomr_radiomenuc)
  - [`@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_GameHooks.c`](#musicradiocorescripts5_missionmusicradiomr_gamehooksc)
- [**5. Core - интерфейс и локализация**](#5-core-интерфейс-и-локализация)
  - [`@MusicRadioCore/gui/layouts/radio_ui.layout`](#musicradiocoreguilayoutsradio_uilayout)
  - [`@MusicRadioCore/gui/layouts/radio_row.layout`](#musicradiocoreguilayoutsradio_rowlayout)
  - [`@MusicRadioCore/inputs.xml`](#musicradiocoreinputsxml)
  - [`@MusicRadioCore/stringtable.csv`](#musicradiocorestringtablecsv)
- [**6. Server - config.cpp и скрипты**](#6-server-configcpp-и-скрипты)
  - [`@MusicRadioServer/config.cpp`](#musicradioserverconfigcpp)
  - [`@MusicRadioServer/Scripts/4_World/MusicRadio/MR_ServerEngine.c`](#musicradioserverscripts4_worldmusicradiomr_serverenginec)
  - [`@MusicRadioServer/Scripts/4_World/MusicRadio/MR_ServerHooks.c`](#musicradioserverscripts4_worldmusicradiomr_serverhooksc)
  - [`@MusicRadioServer/Scripts/5_Mission/MusicRadio/MR_ServerMission.c`](#musicradioserverscripts5_missionmusicradiomr_servermissionc)
- [**7. DB - config.cpp (генерируется)**](#7-db-configcpp-генерируется)
  - [`@MusicRadioDB/config.cpp`](#musicradiodbconfigcpp)
- [**8. DB - данные медиапака**](#8-db-данные-медиапака)
  - [`@MusicRadioDB/Music/index.json`](#musicradiodbmusicindexjson)
  - [`@MusicRadioDB/Music/carriers.json`](#musicradiodbmusiccarriersjson)
  - [`@MusicRadioDB/Music/DatabaseConfig.json`](#musicradiodbmusicdatabaseconfigjson)
  - [`@MusicRadioDB/Music/Rock/demo_playlist.json`](#musicradiodbmusicrockdemo_playlistjson)
  - [`@MusicRadioDB/CREDITS.txt`](#musicradiodbcreditstxt)
  - [`@MusicRadioDB/Scripts/4_World/MR_DB_Placeholder.c`](#musicradiodbscripts4_worldmr_db_placeholderc)
- [**9. Файлы для сервера**](#9-файлы-для-сервера)
  - [`server_files/profiles/MusicRadio/AudioConfig.json`](#server_filesprofilesmusicradioaudioconfigjson)
  - [`server_files/mpmissions/types_musicradio.xml`](#server_filesmpmissionstypes_musicradioxml)
  - [`server_files/mpmissions/cfgspawnabletypes.xml`](#server_filesmpmissionscfgspawnabletypesxml)
- [**10. Справочник: прокси для торпеды (не подключается)**](#10-справочник-прокси-для-торпеды-не-подключается)
  - [`Models/config_proxy_snippet.cpp`](#modelsconfig_proxy_snippetcpp)

---

## 1. Core - config.cpp

### @MusicRadioCore/config.cpp

`@MusicRadioCore/config.cpp` - 465 строк, 15712 байт.

```cpp
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
```

## 2. Core - 3_Game

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Constants.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Constants.c` - 259 строк, 8896 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_Constants.c : all shared constants, enums and small helpers.
//  Layer: 3_Game  (must not reference 4_World / 5_Mission classes)
//  Target: DayZ 1.30+
// ============================================================================

class MR
{
	// --- identity -----------------------------------------------------------
	static const string MOD_NAME        = "MusicRadio";
	static const string VERSION         = "1.0.0";
	static const int    PROTOCOL        = 1;

	// PBO prefixes (must match the folder name inside the @mod folder)
	static const string PREFIX_CORE     = "MusicRadioCore";
	static const string PREFIX_DB       = "MusicRadioDB";

	// --- config / data files ------------------------------------------------
	// Server side  ($mission: -> mpmissions\<map>\)
	static const string DIR_MISSION     = "$mission:MusicRadio/";
	static const string CFG_AUDIO       = "AudioConfig.json";
	static const string CFG_DATABASE    = "DatabaseConfig.json";
	// Client side  ($profile: -> DayZ\profiles\ or client profile dir)
	static const string DIR_PROFILE     = "$profile:MusicRadio/";
	static const string CFG_AUDIO_SAVED = "AudioConfigClient.json";
	static const string LOG_FILE        = "MusicRadio.log";

	// --- playback ------------------------------------------------------------
	static const int    VOLUME_STEPS     = 10;      // 0..10 (10 = 100%)
	static const float  RADIUS_MIN       = 10.0;
	static const float  RADIUS_MAX       = 80.0;
	static const float  RADIUS_DEFAULT   = 40.0;
	static const int    MAX_TRACK_LENGTH = 1800;    // sec, safety net
	static const float  FADE_IN          = 0.8;
	static const float  FADE_OUT         = 0.5;
	static const int    TICK_MS          = 250;     // audio manager tick
	static const int    SERVER_TICK_MS   = 1000;    // server watchdog tick

	// --- tape deck / carrier state (persisted on the cassette itself) --------
	// Pattern taken from OMD_CarRadio: the position lives on the CARRIER, so a
	// tape can be pulled out, carried to another player and resumed there.
	// "Side A 2:17" survives a server restart.
	static const int    TAPE_MARKER      = 78410;   // OnStoreSave version marker
	static const int    TAPE_TICK_MS     = 500;     // position update cadence
	static const int    TAPE_BATTERY_IDLE_MS = 5000; // I/O buffer flush interval

	// Physical tape: 46 minutes per side, so a track can never be longer.
	static const int    TAPE_SIDE_MAX    = 46 * 60;

	// Slot locks: the carrier cannot be pulled out while the deck is spinning.
	// Release is delayed, otherwise the inventory is not ready yet and
	// GetSlotLock returns garbage (same fix as VOS_Radio CarScript.c).
	static const int    SLOT_UNLOCK_DELAY_MS = 1000;

	// Sound set naming contract, implemented by the media pack (DB mod):
	//     MR_<PROVIDER>_<TRACKID>_SoundSet

	// Default (fallback) radio sound set shipped with Core.
	static const string RADIO_FALLBACK_SS = "MR_RadioFallback_SoundSet";

	// --- RPC ids (single base, avoid vanilla ERPC range) ---------------------
	static const int RPC_BASE           = 784100;
	static const int RPC_CONFIG         = 784101;   // server -> client (settings)
	static const int RPC_LIBRARY        = 784102;   // server -> client (tracks)
	static const int RPC_PLAY           = 784103;   // host -> client(s)
	static const int RPC_STOP           = 784104;
	static const int RPC_VOLUME         = 784105;
	static const int RPC_TRACK_ENDED    = 784106;   // host -> client(s)
	static const int RPC_REQUEST        = 784107;   // client -> server (control)
	static const int RPC_REPLY          = 784108;   // server -> single client
	static const int RPC_NOWPLAYING     = 784109;
	static const int RPC_STATE          = 784110;   // late-join sync

	// --- logging -------------------------------------------------------------
	// ------------------------------------------------------------ string kit
	// У string НЕТ StartsWith/EndsWith:
	//   mr_mediaresolver.c,130: Undefined function 'string.StartsWith'
	//   mr_library.c,336:       Undefined function 'string.EndsWith'
	static bool StartsWith(string s, string prefix)
	{
		if (s == string.Empty || prefix == string.Empty)
			return false;
		if (s.Length() < prefix.Length())
			return false;
		return s.Substring(0, prefix.Length()) == prefix;
	}

	static bool EndsWith(string s, string suffix)
	{
		if (s == string.Empty || suffix == string.Empty)
			return false;
		int len = s.Length();
		int slen = suffix.Length();
		if (len < slen)
			return false;
		return s.Substring(len - slen, slen) == suffix;
	}

	// ------------------------------------------------------------ enum kit
	// Приведение "(MR_HostType)x" в присваивании движок считает "не NULL
	// присваиванием" (mr_net.c,84 "Only NULL assignment is allowed").
	// Поэтому конвертация спрятана в функцию: результат уже нужного типа.
	static MR_MediaKind MediaKindFromInt(int value)
	{
		if (value == 1)
			return MR_MediaKind.MEDIA_TRACK;
		return MR_MediaKind.MEDIA_NONE;
	}

	static MR_HostType HostTypeFromInt(int value)
	{
		if (value == 1)
			return MR_HostType.HOST_ITEM;
		if (value == 2)
			return MR_HostType.HOST_VEHICLE;
		if (value == 3)
			return MR_HostType.HOST_SPEAKER;
		if (value == 4)
			return MR_HostType.HOST_PLAYER;
		return MR_HostType.HOST_NONE;
	}

	static void Log(string msg)
	{
		Print("[MusicRadio] " + msg);
	}

	static void LogWarn(string msg)
	{
		Print("[MusicRadio][WARN] " + msg);
	}

	static void LogError(string msg)
	{
		Print("[MusicRadio][ERROR] " + msg);
	}

	// --- math helpers --------------------------------------------------------
	static float Clamp01(float v)
	{
		if (v < 0) return 0;
		if (v > 1) return 1;
		return v;
	}

	// ------------------------------------------------------------------- id
	//  Network id of an object, both halves in one call.
	//
	//  WHY NOT GetNetworkIDLow()/GetNetworkIDHigh(): there is no such method in
	//  Enforce Script. The engine gives exactly one entry point -
	//      proto void Object.GetNetworkID(out int lowBits, out int highBits)
	//  and calling the split names fails at compile time with
	//      Undefined function 'CarScript#3545#5.GetNetworkIDLow'   (log 20:47)
	//  Names are composed at compile time, so ONE wrong call in the base class
	//  breaks every subclass of it: the same error appeared for CarScript,
	//  PersonalRadio and the deck - from a single line. Hence this wrapper:
	//  the API is touched in one place and fixed in one place.
	static void NetId(Object obj, out int idLow, out int idHigh)
	{
		idLow = 0;
		idHigh = 0;
		if (!obj)
			return;
		obj.GetNetworkID(idLow, idHigh);
	}

	static int ClampInt(int v, int lo, int hi)
	{
		if (v < lo) return lo;
		if (v > hi) return hi;
		return v;
	}

	static float ClampFloat(float v, float lo, float hi)
	{
		if (v < lo) return lo;
		if (v > hi) return hi;
		return v;
	}

	// 0..10 user step -> 0..1 gain (perceptual curve, x^1.6)
	static float VolumeStepToGain(int step)
	{
		float t = ClampFloat((float)step / (float)VOLUME_STEPS, 0, 1);
		return Math.Pow(t, 1.6);
	}

	// Distance attenuation: full volume inside 35% of radius, then falls to 0.
	static float DistanceGain(float distance, float radius)
	{
		if (radius <= 0 || distance <= 0)
			return 1.0;

		float inner = radius * 0.35;
		if (distance <= inner)
			return 1.0;
		if (distance >= radius)
			return 0.0;

		float t = (distance - inner) / (radius - inner);
		return 1.0 - t * t;   // soft shoulder
	}

	static string Pad2(int value)
	{
		if (value < 10)
			return "0" + value.ToString();
		return value.ToString();
	}

	static string Safe(string s)
	{
		if (s == string.Empty)
			return "";
		return s;
	}

	static bool ToBool(string s)
	{
		string t = s;
		t.ToLower();
		return (t == "1" || t == "true" || t == "yes");
	}
}

// What kind of host is playing (see MR_DeviceBase / modded CarScript).
enum MR_HostType
{
	HOST_NONE    = 0,
	HOST_ITEM    = 1,   // handheld / portable radio item
	HOST_VEHICLE = 2,   // car, truck, heli ... (needs a radio in cargo)
	HOST_SPEAKER = 3,   // static loudspeaker (optional, future)
	HOST_PLAYER  = 4    // cassette / MD player item (tape = one album)
}

// What kind of media a library entry is.
// Only local .ogg files are supported by design: streaming was evaluated and
// dropped (see _archive/internet_radio/README.md).
enum MR_MediaKind
{
	MEDIA_NONE  = 0,
	MEDIA_TRACK = 1    // .ogg file shipped by a media pack
}

// Why playback stopped - used for UI strings and logging.
enum MR_StopReason
{
	STOP_USER       = 0,
	STOP_ENDED      = 1,
	STOP_TOO_FAR    = 2,
	STOP_HOST_GONE  = 3,
	STOP_REMOVED    = 4,
	STOP_RUINED     = 5,
	STOP_RATE_LIMIT = 6,
	STOP_DENIED     = 7,
	STOP_ERROR      = 8
}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Net.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Net.c` - 314 строк, 9868 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_Net.c : wire format. One place so the two sides can never disagree.
//  Layer: 3_Game
//
//  Transport: ScriptRPC with a numeric RPC id (see MR.RPC_*), payload is a
//  single string. Strings are used deliberately:
//    * readable in logs while developing
//    * forward compatible (unknown trailing fields are ignored)
//    * no per-field Param<?> allocations
//  Fields are separated by '\n', records by '\t'. Titles never contain either
//  because MR_Net.Sanitize() strips them.
// ============================================================================

// What the player asked for (client -> server).
enum MR_Action
{
	ACT_UNKNOWN     = 0,
	ACT_PLAY_TRACK  = 1,
	// 2 was ACT_PLAY_STREAM: internet radio is not part of the mod any more
	// (_archive/internet_radio/). The number stays reserved.
	ACT_STOP        = 3,
	ACT_VOLUME      = 4,
	ACT_RADIUS      = 5,
	ACT_NEXT        = 6,
	ACT_PREV        = 7,
	ACT_REQUEST_LIB = 8,
	ACT_NOWPLAYING  = 9,
	ACT_TOGGLE_BROADCAST = 10,
	// 11: play the album that sits in a cassette / MiniDisc player
	//     m_TrackId carries the album id, the server resolves it to tracks
	ACT_PLAY_ALBUM  = 11,
	// 12: the client moved the head inside the current track (seek). The tape
	//     deck reports its own position so the server keeps the authoritative
	//     copy that is written to the carrier's OnStoreSave.
	//     m_Volume carries the position in SECONDS (reuses an existing field,
	//     no wire format change, old servers ignore the action).
	ACT_SEEK        = 12
}

class MR_Request
{
	int    m_Action;
	MR_HostType m_HostType;
	int    m_HostId;
	int    m_HostIdHigh;
	string m_TrackId;
	int    m_Volume;
	float  m_Radius;
	bool   m_Broadcast;
	// Offset (секунды), с которого продолжать плёнку. Поле было в
	// MR_PlayRequest, но сервер работает с MR_Request - и падал с
	//   Can't find variable 'm_StartOffset'   (лог 00:52, 319 и 455)
	// Держим его ровно в одном месте: MR_PlayRequest больше не нужен.
	int    m_StartOffset;

	void MR_Request()
	{
		m_Action = MR_Action.ACT_UNKNOWN;
		m_HostType = MR_HostType.HOST_NONE;
		m_HostId = 0;
		m_HostIdHigh = 0;
		m_TrackId = string.Empty;
		m_Volume = -1;
		m_Radius = -1;
		m_Broadcast = true;
		m_StartOffset = 0;
	}

	string Serialize()
	{
		TStringArray parts = new TStringArray();
		parts.Insert(((int)m_Action).ToString());
		parts.Insert(((int)m_HostType).ToString());
		parts.Insert(m_HostId.ToString());
		parts.Insert(m_HostIdHigh.ToString());
		parts.Insert(MR_Net.Sanitize(m_TrackId));
		parts.Insert(m_Volume.ToString());
		parts.Insert(m_Radius.ToString());
		parts.Insert(MR_Track.BoolJson(m_Broadcast));
		parts.Insert(m_StartOffset.ToString());
		return MR_Net.JoinParts(parts);
	}

	static MR_Request Parse(string payload)
	{
		MR_Request r = new MR_Request();
		if (payload == string.Empty)
			return r;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 4)
			return r;

		r.m_Action     = f.Get(0).ToInt();
		r.m_HostType   = MR.HostTypeFromInt(f.Get(1).ToInt());
		r.m_HostId     = f.Get(2).ToInt();
		r.m_HostIdHigh = f.Get(3).ToInt();

		if (f.Count() > 4) r.m_TrackId   = f.Get(4);
		if (f.Count() > 5) r.m_Volume    = f.Get(5).ToInt();
		if (f.Count() > 6) r.m_Radius    = f.Get(6).ToFloat();
		if (f.Count() > 7) r.m_Broadcast = MR.ToBool(f.Get(7));
		if (f.Count() > 8) r.m_StartOffset = f.Get(8).ToInt();

		return r;
	}
}

class MR_Net
{
	// ---------------------------------------------------------------- helpers
	// Собрать строку из частей. Каждая строка исходника - отдельный
	// оператор: длинные выражения с переносами этот компилятор не принимает
	// (mr_audiotypes.c(118): "Syntax error", даже когда "+" стоит в конце
	// строки). Части добавляются вызовом AddPart.
	static string JoinParts(TStringArray parts)
	{
		string outS = "";
		foreach (string part : parts)
			outS = outS + part;
		return outS;
	}

	static string Sanitize(string input)
	{
		string outS = input;
		outS.Replace("\n", " ");
		outS.Replace("\t", " ");
		outS.Replace("\r", " ");
		return outS;
	}

	// ------------------------------------------------------------- play request
	static string EncodePlay(MR_PlayRequest req)
	{
		if (!req)
			return string.Empty;

		TStringArray parts = new TStringArray();
		parts.Insert("PLAY");
		parts.Insert(((int)req.m_HostType).ToString());
		parts.Insert(req.m_HostId.ToString());
		parts.Insert(req.m_HostIdHigh.ToString());
		// Локатор может быть null (запросы без трека): тогда пустая строка.
		string locText = "";
		if (req.m_Locator)
			locText = req.m_Locator.Serialize();
		parts.Insert(locText);
		parts.Insert(req.m_StartOffset.ToString());
		parts.Insert(req.m_Radius.ToString());
		parts.Insert(req.m_Volume.ToString());
		parts.Insert(MR_Track.BoolJson(req.m_Loop));
		parts.Insert(MR_Track.BoolJson(req.m_Broadcast));
		return MR_Net.JoinParts(parts);
	}

	static MR_PlayRequest DecodePlay(string payload)
	{
		if (payload == string.Empty)
			return null;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 9 || f.Get(0) != "PLAY")
			return null;

		MR_PlayRequest req = new MR_PlayRequest();
		req.m_HostType   = MR.HostTypeFromInt(f.Get(1).ToInt());
		req.m_HostId     = f.Get(2).ToInt();
		req.m_HostIdHigh = f.Get(3).ToInt();
		MR_Locator parsedLoc = MR_Locator.FromString(f.Get(4));
		if (parsedLoc)
			req.m_Locator = parsedLoc;
		req.m_StartOffset = f.Get(5).ToInt();
		req.m_Radius     = f.Get(6).ToFloat();
		req.m_Volume     = f.Get(7).ToInt();
		req.m_Loop       = MR.ToBool(f.Get(8));
		// тернарный оператор отдельной строкой: внутри аргумента/списка
		// Enforce Script отвечает "Broken expression (missing ';'?)"
		if (f.Count() > 9)
			req.m_Broadcast = MR.ToBool(f.Get(9));
		else
			req.m_Broadcast = true;
		return req;
	}

	// ------------------------------------------------------------- stop / misc
	// "STOP\n<type>\n<idLow>\n<idHigh>\n<reason>"
	static string EncodeStop(MR_HostType type, int idLow, int idHigh, MR_StopReason reason)
	{
		TStringArray parts = new TStringArray();
		parts.Insert("STOP");
		parts.Insert(((int)type).ToString());
		parts.Insert(idLow.ToString());
		parts.Insert(idHigh.ToString());
		parts.Insert(((int)reason).ToString());
		return MR_Net.JoinParts(parts);
	}

	static bool DecodeStop(string payload, out MR_HostType type, out int idLow, out int idHigh, out int reason)
	{
		type = MR_HostType.HOST_NONE;
		idLow = 0;
		idHigh = 0;
		reason = 0;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 5 || f.Get(0) != "STOP")
			return false;

		type   = MR.HostTypeFromInt(f.Get(1).ToInt());
		idLow  = f.Get(2).ToInt();
		idHigh = f.Get(3).ToInt();
		reason = f.Get(4).ToInt();
		return true;
	}

	// "VOL\n<type>\n<idLow>\n<idHigh>\n<volume0..10>"
	static string EncodeVolume(MR_HostType type, int idLow, int idHigh, int volume)
	{
		return "VOL\n" + ((int)type).ToString() + "\n" + idLow.ToString() + "\n" + idHigh.ToString() + "\n" + MR.ClampInt(volume, 0, MR.VOLUME_STEPS).ToString();
	}

	static bool DecodeVolume(string payload, out MR_HostType type, out int idLow, out int idHigh, out int volume)
	{
		type = MR_HostType.HOST_NONE;
		idLow = 0;
		idHigh = 0;
		volume = 0;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 5 || f.Get(0) != "VOL")
			return false;

		type   = MR.HostTypeFromInt(f.Get(1).ToInt());
		idLow  = f.Get(2).ToInt();
		idHigh = f.Get(3).ToInt();
		volume = f.Get(4).ToInt();
		return true;
	}

	// "ENDED\n<type>\n<idLow>\n<idHigh>" - the client tells the server the media
	// finished (server is authority for "next track" logic).
	static string EncodeEnded(MR_HostType type, int idLow, int idHigh)
	{
		return "ENDED\n" + ((int)type).ToString() + "\n" + idLow.ToString() + "\n" + idHigh.ToString();
	}

	static bool DecodeEnded(string payload, out MR_HostType type, out int idLow, out int idHigh)
	{
		type = MR_HostType.HOST_NONE;
		idLow = 0;
		idHigh = 0;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 4 || f.Get(0) != "ENDED")
			return false;

		type   = MR.HostTypeFromInt(f.Get(1).ToInt());
		idLow  = f.Get(2).ToInt();
		idHigh = f.Get(3).ToInt();
		return true;
	}

	// "NP\n<title>\n<artist>\n<provider>" - now playing feed for chat / UI
	static string EncodeNowPlaying(string title, string artist, string provider)
	{
		return "NP\n" + Sanitize(title) + "\n" + Sanitize(artist) + "\n" + Sanitize(provider);
	}

	static bool DecodeNowPlaying(string payload, out string title, out string artist, out string provider)
	{
		title = string.Empty;
		artist = string.Empty;
		provider = string.Empty;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 4 || f.Get(0) != "NP")
			return false;

		title    = f.Get(1);
		artist   = f.Get(2);
		provider = f.Get(3);
		return true;
	}

	// "REPLY\n<ok>\n<message>" - server answer to a rejected/accepted request
	static string EncodeReply(bool ok, string message)
	{
		return "REPLY\n" + MR_Track.BoolJson(ok) + "\n" + Sanitize(message);
	}

	static bool DecodeReply(string payload, out bool ok, out string message)
	{
		ok = false;
		message = string.Empty;

		TStringArray f = new TStringArray();
		payload.Split("\n", f);
		if (f.Count() < 3 || f.Get(0) != "REPLY")
			return false;

		ok = MR.ToBool(f.Get(1));
		message = f.Get(2);
		return true;
	}
}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Json.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Json.c` - 506 строк, 11081 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_Json.c : tiny tolerant JSON reader.
//
//  DayZ ships a JSON serializer but it refuses comments, trailing commas and
//  single quotes - all of which humans put into hand edited media databases.
//  This reader accepts a slightly relaxed JSON so pack authors can keep notes:
//      // comment        # comment        trailing commas        'single quotes'
//  It only produces a generic tree (MR_JsonValue). Parsing into typed
//  MR_Track / MR_Station objects happens in MR_Library.c so that a single
//  broken entry can be skipped without throwing away the whole database.
// ============================================================================

class MR_JsonValue
{
	static const int T_NULL   = 0;
	static const int T_STRING = 1;
	static const int T_NUMBER = 2;
	static const int T_BOOL   = 3;
	static const int T_OBJECT = 4;
	static const int T_ARRAY  = 5;

	int    m_Type;
	string m_Str;
	string m_Num;                       // kept as text - avoids locale issues
	bool   m_Bool;
	ref map<string, ref MR_JsonValue> m_Obj;
	ref array<ref MR_JsonValue>       m_Arr;

	void MR_JsonValue()
	{
		m_Type = T_NULL;
		m_Str  = string.Empty;
		m_Num  = "0";
		m_Bool = false;
	}

	// --- typed accessors (never fail, return defaults) ----------------------
	string AsString(string def = "")
	{
		if (m_Type == T_STRING) return m_Str;
		if (m_Type == T_NUMBER) return m_Num;
		if (m_Type == T_BOOL)   return m_Bool.ToString();
		return def;
	}

	float AsFloat(float def = 0)
	{
		if (m_Type == T_NUMBER) return m_Num.ToFloat();
		if (m_Type == T_STRING) return m_Str.ToFloat();
		return def;
	}

	int AsInt(int def = 0)
	{
		if (m_Type == T_NUMBER) return m_Num.ToInt();
		if (m_Type == T_STRING) return m_Str.ToInt();
		return def;
	}

	bool AsBool(bool def = false)
	{
		if (m_Type == T_BOOL) return m_Bool;
		if (m_Type == T_NUMBER) return (m_Num.ToInt() != 0);
		if (m_Type == T_STRING)
		{
			string t = m_Str;
			t.ToLower();
			if (t == "true" || t == "1" || t == "yes") return true;
			if (t == "false" || t == "0" || t == "no") return false;
		}
		return def;
	}

	// Object member lookup; returns null when missing.
	MR_JsonValue Get(string key)
	{
		if (m_Type != T_OBJECT || !m_Obj)
			return null;
		return m_Obj.Get(key);
	}

	string GetString(string key, string def = "")
	{
		MR_JsonValue v = Get(key);
		if (!v) return def;
		return v.AsString(def);
	}

	int GetInt(string key, int def = 0)
	{
		MR_JsonValue v = Get(key);
		if (!v) return def;
		return v.AsInt(def);
	}

	float GetFloat(string key, float def = 0)
	{
		MR_JsonValue v = Get(key);
		if (!v) return def;
		return v.AsFloat(def);
	}

	bool GetBool(string key, bool def = false)
	{
		MR_JsonValue v = Get(key);
		if (!v) return def;
		return v.AsBool(def);
	}

	array<ref MR_JsonValue> GetArray(string key)
	{
		MR_JsonValue v = Get(key);
		if (!v || v.m_Type != T_ARRAY)
			return null;
		return v.m_Arr;
	}

	// Integer-indexed access used by files that describe a list as
	//   "tracks": { "1": {...}, "2": {...} }   (handier than arrays for humans)
	array<ref MR_JsonValue> CollectValues()
	{
		array<ref MR_JsonValue> outArr = new array<ref MR_JsonValue>();
		if (m_Type == T_ARRAY && m_Arr)
		{
			foreach (MR_JsonValue av : m_Arr)
				outArr.Insert(av);
			return outArr;
		}
		if (m_Type == T_OBJECT && m_Obj)
		{
			foreach (string k, MR_JsonValue ov : m_Obj)
				outArr.Insert(ov);
		}
		return outArr;
	}
}

class MR_Json
{
	protected string m_Src;
	protected int    m_Pos;
	protected int    m_Len;

	// If set, the file was NOT valid JSON and the parser did a best effort pass.
	bool m_HadErrors;

	static MR_JsonValue ParseFile(string path)
	{
		if (!FileExist(path))
		{
			MR.LogWarn("JSON file not found: " + path);
			return null;
		}

		string raw;
		if (!MR_Json.LoadText(path, raw))
			return null;

		return MR_Json.Parse(raw);
	}

	static bool LoadText(string path, out string raw)
	{
		// FileHandle has NO methods: it is "typedef int[] FileHandle", so
		// neither "fh == 0" (Incompatible parameter) nor "fh.IsOpen()"
		// (Undefined function, log 18:00) compiles. The reliable check is to
		// ask the filesystem first - the idiom already used in this project.
		if (!FileExist(path))
		{
			MR.LogWarn("Cannot open file: " + path);
			return false;
		}

		FileHandle fh = OpenFile(path, FileMode.READ);
		if (!fh)
		{
			MR.LogWarn("Cannot open file: " + path);
			return false;
		}

		string line = string.Empty;
		raw = string.Empty;
		while (FGets(fh, line) >= 0)
		{
			raw = raw + line + "\n";
		}
		CloseFile(fh);
		return true;
	}

	static bool SaveText(string path, string data)
	{
		// A write may create the file, so FileExist() is not a precondition
		// here: "if (!fh)" on the returned handle is what the working
		// call sites in this project use.
		FileHandle fh = OpenFile(path, FileMode.WRITE);
		if (!fh)
		{
			MR.LogWarn("Cannot write file: " + path);
			return false;
		}
		FPrint(fh, data);
		CloseFile(fh);
		return true;
	}

	static MR_JsonValue Parse(string src)
	{
		MR_Json p = new MR_Json();
		p.m_Src = src;
		p.m_Len = src.Length();
		p.m_Pos = 0;
		p.m_HadErrors = false;

		// Skip UTF-8 BOM if present. In a byte string 0xEF 0xBB 0xBF shows up
		// as three characters; "\u00EF" below is the first of them.
		if (p.m_Len > 2 && src.Substring(0, 1) == "\u00EF" && src.Substring(1, 1) == "\u00BB")
			p.m_Pos = 3;

		MR_JsonValue root = p.ParseValue();
		if (!root)
		{
			MR.LogError("JSON: root value is empty");
			return null;
		}

		if (p.m_HadErrors)
			MR.LogWarn("JSON: parsed with recoverable errors");

		return root;
	}

	// ------------------------------------------------------------------ lexer
	protected void SkipWhitespace()
	{
		while (m_Pos < m_Len)
		{
			string ch = m_Src.Substring(m_Pos, 1);

			if (ch == " " || ch == "\t" || ch == "\n" || ch == "\r")
			{
				m_Pos = m_Pos + 1;
				continue;
			}

			// line comments
			if (ch == "/" && m_Pos + 1 < m_Len && m_Src.Substring(m_Pos + 1, 1) == "/")
			{
				m_Pos = m_Pos + 2;
				while (m_Pos < m_Len && m_Src.Substring(m_Pos, 1) != "\n")
					m_Pos = m_Pos + 1;
				continue;
			}

			// hash comments (helpful in files edited by non programmers)
			if (ch == "#")
			{
				while (m_Pos < m_Len && m_Src.Substring(m_Pos, 1) != "\n")
					m_Pos = m_Pos + 1;
				continue;
			}

			break;
		}
	}

	protected string Peek()
	{
		if (m_Pos >= m_Len)
			return string.Empty;
		return m_Src.Substring(m_Pos, 1);
	}

	protected MR_JsonValue ParseValue()
	{
		SkipWhitespace();
		string ch = Peek();
		if (ch == string.Empty)
			return null;

		if (ch == "{") return ParseObject();
		if (ch == "[") return ParseArray();
		if (ch == "\"" || ch == "'") return ParseString(ch);

		// literal / number
		int start = m_Pos;
		while (m_Pos < m_Len)
		{
			string c = m_Src.Substring(m_Pos, 1);
			if (c == "," || c == "}" || c == "]" || c == " " || c == "\n" || c == "\r" || c == "\t" || c == ":")
				break;
			m_Pos = m_Pos + 1;
		}

		string token = m_Src.Substring(start, m_Pos - start);
		MR_JsonValue v = new MR_JsonValue();

		if (token == string.Empty)
		{
			m_HadErrors = true;
			return v;
		}

		string lower = token;
		lower.ToLower();

		if (lower == "null")
		{
			v.m_Type = MR_JsonValue.T_NULL;
		}
		else if (lower == "true" || lower == "false")
		{
			v.m_Type = MR_JsonValue.T_BOOL;
			v.m_Bool = (lower == "true");
		}
		else
		{
			v.m_Type = MR_JsonValue.T_NUMBER;
			v.m_Num = token;
		}

		return v;
	}

	protected MR_JsonValue ParseObject()
	{
		MR_JsonValue obj = new MR_JsonValue();
		obj.m_Type = MR_JsonValue.T_OBJECT;
		obj.m_Obj = new map<string, ref MR_JsonValue>();

		m_Pos = m_Pos + 1;   // {

		while (m_Pos < m_Len)
		{
			SkipWhitespace();
			string ch = Peek();

			if (ch == string.Empty)
				break;

			if (ch == "}")
			{
				m_Pos = m_Pos + 1;
				return obj;
			}

			if (ch == ",")
			{
				m_Pos = m_Pos + 1;
				continue;
			}

			// key
			MR_JsonValue keyVal = null;
			if (ch == "\"" || ch == "'")
				keyVal = ParseString(ch);
			else
			{
				// unquoted key - tolerate it
				int start = m_Pos;
				while (m_Pos < m_Len)
				{
					string c = m_Src.Substring(m_Pos, 1);
					if (c == ":" || c == " " || c == "\n" || c == "\r" || c == "\t")
						break;
					m_Pos = m_Pos + 1;
				}
				keyVal = new MR_JsonValue();
				keyVal.m_Type = MR_JsonValue.T_STRING;
				keyVal.m_Str = m_Src.Substring(start, m_Pos - start);
			}

			if (!keyVal)
				break;

			string key = keyVal.AsString();
			if (key == string.Empty)
			{
				m_HadErrors = true;
			}

			SkipWhitespace();
			if (Peek() == ":")
				m_Pos = m_Pos + 1;

			MR_JsonValue val = ParseValue();
			if (!val)
			{
				m_HadErrors = true;
				val = new MR_JsonValue();
			}

			if (key != string.Empty && !obj.m_Obj.Contains(key))
				obj.m_Obj.Set(key, val);
			else if (key != string.Empty)
				MR.LogWarn("JSON: duplicate key ignored: " + key);
		}

		m_HadErrors = true;
		return obj;
	}

	protected MR_JsonValue ParseArray()
	{
		MR_JsonValue arr = new MR_JsonValue();
		arr.m_Type = MR_JsonValue.T_ARRAY;
		arr.m_Arr = new array<ref MR_JsonValue>();

		m_Pos = m_Pos + 1;   // [

		while (m_Pos < m_Len)
		{
			SkipWhitespace();
			string ch = Peek();

			if (ch == string.Empty)
				break;

			if (ch == "]")
			{
				m_Pos = m_Pos + 1;
				return arr;
			}

			if (ch == ",")
			{
				m_Pos = m_Pos + 1;
				continue;
			}

			MR_JsonValue val = ParseValue();
			if (!val)
			{
				m_HadErrors = true;
				break;
			}
			arr.m_Arr.Insert(val);
		}

		m_HadErrors = true;
		return arr;
	}

	protected MR_JsonValue ParseString(string quote)
	{
		MR_JsonValue v = new MR_JsonValue();
		v.m_Type = MR_JsonValue.T_STRING;

		m_Pos = m_Pos + 1;   // opening quote
		string outStr = string.Empty;

		while (m_Pos < m_Len)
		{
			string ch = m_Src.Substring(m_Pos, 1);

			if (ch == "\\")
			{
				// escape sequence
				if (m_Pos + 1 >= m_Len)
					break;

				string esc = m_Src.Substring(m_Pos + 1, 1);
				if (esc == "n")       outStr = outStr + "\n";
				else if (esc == "t")  outStr = outStr + "\t";
				else if (esc == "r")  outStr = outStr + "\r";
				else if (esc == "\"") outStr = outStr + "\"";
				else if (esc == "'")  outStr = outStr + "'";
				else if (esc == "\\") outStr = outStr + "\\";
				else if (esc == "/")  outStr = outStr + "/";
				else                  outStr = outStr + esc;

				// Enforce strings are byte strings; we cannot synthesise an
				// arbitrary UTF-8 byte from an int. \uXXXX is therefore
				// skipped and replaced - save databases as UTF-8 (no escapes).
				if (esc == "u")
				{
					MR.LogWarn("JSON: \\u escape found, replaced by '?'. Save the file as plain UTF-8.");
					outStr = outStr + "?";
					m_Pos = m_Pos + 6;
					continue;
				}

				m_Pos = m_Pos + 2;
				continue;
			}

			if (ch == quote)
			{
				m_Pos = m_Pos + 1;
				v.m_Str = outStr;
				return v;
			}

			outStr = outStr + ch;
			m_Pos = m_Pos + 1;
		}

		// unterminated string
		m_HadErrors = true;
		v.m_Str = outStr;
		return v;
	}

}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Track.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Track.c` - 258 строк, 7971 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_Track.c : one entry of the media library (a local .ogg track).
//  Layer: 3_Game
// ============================================================================

class MR_Track
{
	// --- identity -----------------------------------------------------------
	string          m_Id;            // stable id, e.g. "wasteland_01"
	string          m_Provider;      // pack that owns it, e.g. "MusicRadioDB"
	string          m_Kind;          // "music" | "ambient" | "sfx"
	string          m_Title;
	string          m_Artist;
	string          m_Album;        // display name ("Demo Album")
	string          m_AlbumId;      // album key = cassette/MD class suffix ("rock01")
	int             m_Year;
	int             m_Duration;      // seconds, 0 = unknown
	autoptr array<string> m_Tags;    // "rock","chill","night" ...

	// --- audio source -------------------------------------------------------
	// m_File: relative path inside the pack ("Music\\rock\\track01.ogg"),
	//         converted to a SoundSet name by MR_MediaResolver.
	MR_MediaKind    m_MediaKind;
	string          m_File;
	string          m_SoundSet;      // optional explicit override

	// --- playback properties ------------------------------------------------
	float           m_DefaultVolume; // 0..1
	bool            m_Loop;          // loop single track
	bool            m_Enabled;       // admins can disable one entry
	bool            m_Explicit;      // for a "no swearing" server filter
	int             m_Bitrate;       // kbps, informational (media metadata only)
	string          m_Cover;         // optional image name inside the pack

	// --- validation state (filled by the resolver) --------------------------
	bool            m_Resolved;
	string          m_ResolvedSoundSet;
	string          m_ResolveError;

	void MR_Track()
	{
		m_Id             = string.Empty;
		m_Provider       = string.Empty;
		m_Kind           = "music";
		m_Title          = string.Empty;
		m_Artist         = string.Empty;
		m_Album          = string.Empty;
		m_AlbumId        = string.Empty;
		m_Year           = 0;
		m_Duration       = 0;
		m_Tags           = new array<string>();
		m_MediaKind      = MR_MediaKind.MEDIA_NONE;
		m_File           = string.Empty;
		m_SoundSet       = string.Empty;
		m_DefaultVolume  = 0.8;
		m_Loop           = false;
		m_Enabled        = true;
		m_Explicit       = false;
		m_Bitrate        = 0;
		m_Cover          = string.Empty;
		m_Resolved       = false;
		m_ResolvedSoundSet = string.Empty;
		m_ResolveError   = string.Empty;
	}

	bool IsPlayable()
	{
		if (!m_Enabled)
			return false;
		if (m_MediaKind == MR_MediaKind.MEDIA_TRACK)
			return (m_File != string.Empty || m_SoundSet != string.Empty);
		return false;
	}

	// "Artist - Title" for the UI / chat / now playing feed.
	string DisplayName()
	{
		if (m_Artist != string.Empty && m_Title != string.Empty)
			return m_Artist + " - " + m_Title;
		if (m_Title != string.Empty)
			return m_Title;
		if (m_Id != string.Empty)
			return m_Id;
		return "unknown";
	}

	string DurationText()
	{
		if (m_Duration <= 0)
			return "--:--";
		int m = m_Duration / 60;
		int s = m_Duration % 60;
		return m.ToString() + ":" + MR.Pad2(s);
	}

	// ------------------------------------------------------------------ parse
	// Accepts both the compact form  { "id":"x", "artist":"y" ... }
	// and the "array of strings" shorthand for tags.
	static MR_Track FromJson(MR_JsonValue obj, string provider)
	{
		if (!obj || obj.m_Type != MR_JsonValue.T_OBJECT)
			return null;

		MR_Track t = new MR_Track();
		t.m_Provider = provider;

		t.m_Id       = obj.GetString("id");
		t.m_Kind     = obj.GetString("kind", "music");
		t.m_Title    = obj.GetString("title");
		t.m_Artist   = obj.GetString("artist");
		t.m_Album    = obj.GetString("album");
		t.m_AlbumId  = obj.GetString("albumId");
		t.m_Year     = obj.GetInt("year", 0);
		t.m_Duration = obj.GetInt("duration", 0);
		t.m_File     = obj.GetString("file");
		t.m_SoundSet = obj.GetString("soundset");
		t.m_Cover    = obj.GetString("cover");
		t.m_Bitrate  = obj.GetInt("bitrate", 0);
		t.m_Enabled  = obj.GetBool("enabled", true);
		t.m_Explicit = obj.GetBool("explicit", false);
		t.m_Loop     = obj.GetBool("loop", false);
		t.m_DefaultVolume = obj.GetFloat("volume", 0.8);

		// Durations are commonly written "3:42" by humans.
		MR_JsonValue durJson = obj.Get("duration");
		if (durJson && durJson.m_Type == MR_JsonValue.T_STRING)
			t.m_Duration = MR_Track.ParseTime(durJson.m_Str);

		// Tags: array of strings or a single comma separated string.
		array<ref MR_JsonValue> tagsArr = obj.GetArray("tags");
		if (tagsArr)
		{
			foreach (MR_JsonValue tv : tagsArr)
			{
				string tag = tv.AsString();
				if (tag != string.Empty)
					t.m_Tags.Insert(tag);
			}
		}
		else
		{
			MR_JsonValue tagsOne = obj.Get("tags");
			if (tagsOne && tagsOne.m_Type == MR_JsonValue.T_STRING)
			{
				TStringArray parts = new TStringArray();
				tagsOne.m_Str.Split(",", parts);
				foreach (string p : parts)
				{
					string trim = MR_Track.Trim(p);
					if (trim != string.Empty)
						t.m_Tags.Insert(trim);
				}
			}
		}

		// Every entry is a local .ogg from a media pack.
		if (t.m_File != string.Empty || t.m_SoundSet != string.Empty)
			t.m_MediaKind = MR_MediaKind.MEDIA_TRACK;

		// An id is mandatory; synthesise one from the title when missing so a
		// sloppy entry still works, but warn the pack author.
		if (t.m_Id == string.Empty)
		{
			t.m_Id = MR_Track.Slugify(t.DisplayName());
			MR.LogWarn("Track without id in provider '" + provider + "', generated: " + t.m_Id);
		}

		if (t.m_Title == string.Empty)
			t.m_Title = t.m_Id;

		return t;
	}

	string ToJson()
	{
		string tags = "\"tags\":[";
		for (int i = 0; i < m_Tags.Count(); i++)
		{
			if (i > 0) tags = tags + ",";
			tags = tags + "\"" + MR_Track.Escape(m_Tags.Get(i)) + "\"";
		}
		tags = tags + "]";

		string s = "{";
		s = s + "\"id\":\"" + MR_Track.Escape(m_Id) + "\",";
		s = s + "\"provider\":\"" + MR_Track.Escape(m_Provider) + "\",";
		s = s + "\"kind\":\"" + MR_Track.Escape(m_Kind) + "\",";
		s = s + "\"title\":\"" + MR_Track.Escape(m_Title) + "\",";
		s = s + "\"artist\":\"" + MR_Track.Escape(m_Artist) + "\",";
		s = s + "\"album\":\"" + MR_Track.Escape(m_Album) + "\",";
		s = s + "\"albumId\":\"" + MR_Track.Escape(m_AlbumId) + "\",";
		s = s + "\"year\":" + m_Year.ToString() + ",";
		s = s + "\"duration\":" + m_Duration.ToString() + ",";
		s = s + tags + ",";
		s = s + "\"file\":\"" + MR_Track.Escape(m_File) + "\",";
		s = s + "\"soundset\":\"" + MR_Track.Escape(m_SoundSet) + "\",";
		s = s + "\"volume\":" + m_DefaultVolume.ToString() + ",";
		s = s + "\"bitrate\":" + m_Bitrate.ToString() + ",";
		s = s + "\"enabled\":" + MR_Track.BoolJson(m_Enabled) + ",";
		s = s + "\"explicit\":" + MR_Track.BoolJson(m_Explicit);
		s = s + "}";
		return s;
	}

	// ------------------------------------------------------------------ utils
	static int ParseTime(string text)
	{
		if (text == string.Empty)
			return 0;

		TStringArray parts = new TStringArray();
		text.Split(":", parts);

		if (parts.Count() == 1)
			return parts.Get(0).ToInt();
		if (parts.Count() == 2)
			return parts.Get(0).ToInt() * 60 + parts.Get(1).ToInt();
		if (parts.Count() >= 3)
			return parts.Get(0).ToInt() * 3600 + parts.Get(1).ToInt() * 60 + parts.Get(2).ToInt();

		return 0;
	}

	static string Trim(string s)
	{
		string outS = s;
		outS.Trim();
		return outS;
	}

	static string Slugify(string s)
	{
		string outS = s;
		outS.ToLower();
		outS.Replace(" ", "_");
		outS.Replace(".", "");
		outS.Replace(",", "");
		outS.Replace("-", "_");
		outS.Replace("'", "");
		return outS;
	}

	static string Escape(string s)
	{
		string outS = s;
		outS.Replace("\\", "\\\\");
		outS.Replace("\"", "\\\"");
		return outS;
	}

	static string BoolJson(bool b)
	{
		if (b) return "true";
		return "false";
	}
}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Locator.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Locator.c` - 73 строк, 2393 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_Locator.c : the "LCT" track locator.
//
//  WHY THIS EXISTS
//  ---------------
//  A media pack (the 3rd mod) is distributed as a PBO whose prefix may change
//  between releases (@MyMusic v1 -> @MyMusicPack v2 ...). If the server hard
//  coded the PBO prefix, every rename would break every saved playlist.
//
//  So the server never sends a raw file path. It sends a *locator*:
//      provider | providerVersion | trackId | volume(out) | seed
//  and the CLIENT resolves  provider -> concrete SoundSet name  using its own
//  copy of the media pack. Nothing about the pack layout is hard coded on the
//  server, and the client validates everything before playing.
//
//  Wire format (the one string that travels over RPC):
//      LCT|1|<provider>|<trackId>|<volume>|<seed>
//  Audio (file / url / duration) is sent separately in the PLAY RPC.
// ============================================================================

class MR_Locator
{
	int    m_Protocol;
	string m_Provider;      // e.g. "MusicRadioDB"
	string m_TrackId;       // stable id inside that provider
	int    m_Volume;        // 0..10
	int    m_Seed;          // random start offset seed (client derives offset)

	void MR_Locator()
	{
		m_Protocol = MR.PROTOCOL;
		m_Provider = string.Empty;
		m_TrackId  = string.Empty;
		m_Volume   = MR.VOLUME_STEPS;
		m_Seed     = 0;
	}

	static MR_Locator FromString(string raw)
	{
		MR_Locator loc = new MR_Locator();
		if (raw == string.Empty)
			return loc;

		TStringArray parts = new TStringArray();
		raw.Split("|", parts);
		if (parts.Count() < 6)
		{
			MR.LogWarn("Bad locator: " + raw);
			return loc;
		}

		loc.m_Protocol = parts.Get(1).ToInt();
		loc.m_Provider = parts.Get(2);
		loc.m_TrackId  = parts.Get(3);
		loc.m_Volume   = parts.Get(4).ToInt();
		loc.m_Seed     = parts.Get(5).ToInt();
		return loc;
	}

	// НЕ ToString(): такое имя уже есть у базового Class, движок отвечает
	// "Multiple declaration of function 'ToString'". Своё имя - Serialize().
	string Serialize()
	{
		return "LCT|1|" + m_Provider + "|" + m_TrackId + "|" + m_Volume.ToString() + "|" + m_Seed.ToString();
	}

	bool IsValid()
	{
		return (m_Protocol == MR.PROTOCOL && m_Provider != string.Empty && m_TrackId != string.Empty);
	}
}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_MediaResolver.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_MediaResolver.c` - 138 строк, 4223 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_MediaResolver.c : locator -> SoundSet resolution, per platform.
//  Layer: 3_Game
//
//  THIS IS THE CONTRACT BETWEEN THE THREE MODS
//  -------------------------------------------
//  Mod 1 (server) and Mod 2 (logic) contain NO audio asset paths.
//  Only the media pack (mod 3) owns files, and it must expose each playable
//  item as a CfgSoundSets entry named:
//
//        MR_<PROVIDER>_<TRACKID>_SoundSet
//
//  Provider and track id are upper cased and stripped of anything that is not
//  [A-Z0-9_]. Example:
//        provider "MusicRadioDB", id "wasteland_01"
//        ->  MR_MUSICRADIODB_WASTELAND_01_SoundSet
//
//  A pack may instead declare an explicit "soundset" field per track, which
//  wins over the convention. Either way the CLIENT validates the name with
//  ConfigIsExisting() before anything is played: a missing or misnamed entry
//  degrades to a chat warning instead of a silent failure.
//
//  Only local .ogg media is supported. Streaming (relay / recorded chunks /
//  external Icecast) was evaluated and dropped from the mod: it always needed
//  either extra software on the player PC or a constantly republished pack.
//  The full analysis and the code are kept in _archive/internet_radio/.
// ============================================================================

class MR_MediaResolver
{
	// Build the canonical SoundSet name for a track inside a pack.
	static string BuildSoundSetName(string provider, string trackId)
	{
		return "MR_" + Normalize(provider) + "_" + Normalize(trackId) + "_SoundSet";
	}

	static string Normalize(string input)
	{
		string outS = input;
		outS.ToUpper();
		outS.Replace(" ", "_");
		outS.Replace("-", "_");
		outS.Replace(".", "_");
		outS.Replace("/", "_");
		outS.Replace("\\", "_");

		// Drop anything that is not A-Z 0-9 _ (keeps config class names legal)
		string clean = string.Empty;
		for (int i = 0; i < outS.Length(); i++)
		{
			string ch = outS.Substring(i, 1);
			if (MR_MediaResolver.IsNameChar(ch))
				clean = clean + ch;
		}
		return clean;
	}

	static bool IsNameChar(string ch)
	{
		if (ch == "_")
			return true;
		// ASCII ranges: 0-9 / A-Z
		if (ch >= "0" && ch <= "9")
			return true;
		if (ch >= "A" && ch <= "Z")
			return true;
		return false;
	}

	// ------------------------------------------------------------------ tracks
	// Resolve local media on the CLIENT. Returns "" when not playable here.
	static string ResolveTrack(MR_Track track, out string error)
	{
		error = string.Empty;

		if (!track)
		{
			error = "no track";
			return string.Empty;
		}

		string candidate = track.m_SoundSet;
		if (candidate == string.Empty)
			candidate = BuildSoundSetName(track.m_Provider, track.m_Id);

		if (!SoundSetExists(candidate))
		{
			// A pack might use its own naming for a whole folder; the track
			// may still declare "file" - in that case try the file based name
			// as a second chance before giving up.
			string alt = string.Empty;
			if (track.m_File != string.Empty)
				alt = BuildSoundSetName(track.m_Provider, track.m_File);

			if (alt != string.Empty && SoundSetExists(alt))
				candidate = alt;
			else
			{
				error = "soundset missing: " + candidate + " (is the media pack installed on the client?)";
				return string.Empty;
			}
		}

		track.m_Resolved = true;
		track.m_ResolvedSoundSet = candidate;
		return candidate;
	}

	static bool SoundSetExists(string name)
	{
		if (name == string.Empty)
			return false;
		return GetGame().ConfigIsExisting("CfgSoundSets " + name);
	}

	// Is this provider (media pack) present on the client at all?
	static bool ProviderInstalled(string provider)
	{
		string probe = "MR_" + Normalize(provider) + "_";
		// Cheap heuristic: any class in CfgSoundSets starting with the prefix.
		// ConfigGetChildrenCount avoids loading the full class list.
		string path = "CfgSoundSets";
		int count = GetGame().ConfigGetChildrenCount(path);
		for (int i = 0; i < count; i++)
		{
			string child = string.Empty;
			if (GetGame().ConfigGetChildName(path, i, child))
			{
				if (MR.StartsWith(child, probe))
					return true;
			}
		}
		return false;
	}
}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Library.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Library.c` - 414 строк, 11170 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_Library.c : media library = tracks from every installed media pack.
//  Layer: 3_Game
//
//  The library is built on BOTH sides, from the same data files:
//    * server: authoritative copy, used for validation and for sending the
//              library to late joiners ("what exists at all")
//    * client: resolved copy, used to translate a locator into a SoundSet
//
//  Because the media pack ships the .ogg files AND the json index, and both are
//  part of the same workshop item, the two copies stay in sync by construction.
// ============================================================================

class MR_Library
{
	protected ref array<ref MR_Track> m_Tracks;
	protected ref map<string, ref MR_Track> m_ById;

	// Diagnostics
	string m_LastError;
	int    m_LoadedFiles;
	int    m_SkippedEntries;

	void MR_Library()
	{
		m_Tracks = new array<ref MR_Track>();
		m_ById   = new map<string, ref MR_Track>();
		m_LastError = string.Empty;
		m_LoadedFiles = 0;
		m_SkippedEntries = 0;
	}

	// ------------------------------------------------------------------ load
	// Loads every music/**.json found in the media pack. On a dedicated server
	// the pack folder is resolved through the mission's mod list; on the client
	// the same files are read from the mounted PBO - FileExist() sees both.
	void LoadAll()
	{
		m_Tracks.Clear();
		m_ById.Clear();
		m_LoadedFiles = 0;
		m_SkippedEntries = 0;

		// Standard layout produced by the DB mod:
		//   MusicRadioDB/Music/index.json            <- always present, all packs merged
		//   MusicRadioDB/Music/<Genre>/<pack>.json   <- per genre / per playlist
		// The index alone is enough; the per-genre files are loaded too so that a
		// pack can be dropped in without regenerating the index.
		string baseDir = MR.PREFIX_DB + "/Music";

		TStringArray dirs = new TStringArray();
		MR_FileUtils.CollectDirs(baseDir, dirs);

		int enumerated = 0;
		foreach (string sub : dirs)
		{
			string dir = baseDir + "/" + sub;
			TStringArray files = new TStringArray();
			MR_FileUtils.CollectJsonFiles(dir, files);
			foreach (string file : files)
			{
				LoadFile(file);
				enumerated = enumerated + 1;
			}
		}

		// Root level files (index.json lives here)
		TStringArray rootFiles = new TStringArray();
		MR_FileUtils.CollectJsonFiles(baseDir, rootFiles);
		foreach (string rootFile : rootFiles)
		{
			LoadFile(rootFile);
			enumerated = enumerated + 1;
		}

		if (enumerated == 0)
		{
			// Directory enumeration inside a PBO can be restricted on some
			// builds; the merged index is the documented fallback.
			MR.LogWarn("No media files enumerated, trying the merged index only.");
			LoadFile(baseDir + "/index.json");
		}

		MR.Log("Library loaded: " + m_Tracks.Count().ToString() + " entries from " + m_LoadedFiles.ToString() + " files (skipped " + m_SkippedEntries.ToString() + ")");
	}

	void LoadFile(string path)
	{
		MR_JsonValue root = MR_Json.ParseFile(path);
		if (!root)
		{
			m_LastError = "cannot parse " + path;
			return;
		}

		string provider = root.GetString("provider");
		if (provider == string.Empty)
			provider = MR.PREFIX_DB;

		array<ref MR_JsonValue> entries = null;

		// Two accepted shapes:
		//   { "provider":"X", "tracks":[ {...}, {...} ] }
		//   { "provider":"X", "tracks":{ "1":{...}, "2":{...} } }
		MR_JsonValue tracksNode = root.Get("tracks");
		if (tracksNode)
			entries = tracksNode.CollectValues();
		else
			entries = root.CollectValues();   // file is just a list

		if (!entries || entries.Count() == 0)
		{
			MR.LogWarn("No entries in " + path);
			return;
		}

		m_LoadedFiles = m_LoadedFiles + 1;

		foreach (MR_JsonValue entry : entries)
		{
			MR_Track t = MR_Track.FromJson(entry, provider);
			if (!t)
			{
				m_SkippedEntries = m_SkippedEntries + 1;
				continue;
			}

			if (m_ById.Contains(t.m_Id))
			{
				// Same id in two packs: first one wins, report it once.
				MR.LogWarn("Duplicate track id '" + t.m_Id + "' in " + path + " (ignored)");
				m_SkippedEntries = m_SkippedEntries + 1;
				continue;
			}

			m_ById.Set(t.m_Id, t);
			m_Tracks.Insert(t);
		}
	}

	// ---------------------------------------------------------------- queries
	int Count()
	{
		return m_Tracks.Count();
	}

	// Rebuild the id index after the track array was modified from outside
	// (the client does that when the server manifest arrives).
	void Reindex()
	{
		if (!m_ById)
			m_ById = new map<string, ref MR_Track>();
		m_ById.Clear();
		foreach (MR_Track t : m_Tracks)
		{
			if (t && !m_ById.Contains(t.m_Id))
				m_ById.Set(t.m_Id, t);
		}
	}

	void Add(MR_Track track)
	{
		if (!track)
			return;
		m_Tracks.Insert(track);
	}

	void ClearIndex()
	{
		if (!m_ById)
			m_ById = new map<string, ref MR_Track>();
		m_ById.Clear();
	}

	array<ref MR_Track> All()
	{
		return m_Tracks;
	}

	MR_Track GetById(string id)
	{
		return m_ById.Get(id);
	}

	// Case-insensitive substring search over title/artist/album/tags.
	array<ref MR_Track> Search(string query, string kindFilter = "")
	{
		array<ref MR_Track> outArr = new array<ref MR_Track>();
		if (query == string.Empty && kindFilter == string.Empty)
			return m_Tracks;

		string q = query;
		q.ToLower();
		string kind = kindFilter;
		kind.ToLower();

		foreach (MR_Track t : m_Tracks)
		{
			if (!t.m_Enabled)
				continue;

			if (kind != string.Empty)
			{
				string tk = t.m_Kind;
				tk.ToLower();
				if (tk != kind)
					continue;
			}

			if (q == string.Empty)
			{
				outArr.Insert(t);
				continue;
			}

			string haystack = (t.m_Title + " " + t.m_Artist + " " + t.m_Album + " " + t.m_Id);
			foreach (string tag : t.m_Tags)
				haystack = haystack + " " + tag;
			haystack.ToLower();

			if (haystack.Contains(q))
				outArr.Insert(t);
		}

		return outArr;
	}

	// Every track of one album, in index order (= side A order).
	// Used by the cassette / MiniDisc players: one carrier = one album.
	array<ref MR_Track> ByAlbum(string albumId)
	{
		array<ref MR_Track> outArr = new array<ref MR_Track>();
		if (albumId == string.Empty)
			return outArr;
		foreach (MR_Track t : m_Tracks)
		{
			if (!t || !t.m_Enabled)
				continue;
			if (t.m_AlbumId == albumId)
				outArr.Insert(t);
		}
		return outArr;
	}

	int CountAlbum(string albumId)
	{
		int n = 0;
		foreach (MR_Track t : m_Tracks)
		{
			if (t && t.m_Enabled && t.m_AlbumId == albumId)
				n = n + 1;
		}
		return n;
	}

	array<ref MR_Track> ByKind(string kind)
	{
		string k = kind;
		k.ToLower();
		return Search(string.Empty, k);
	}

	// Random pick that is not the track currently playing.
	MR_Track RandomTrack(string kind = "music", string excludeId = "")
	{
		array<ref MR_Track> pool = ByKind(kind);
		if (excludeId != string.Empty && pool.Count() > 1)
		{
			array<ref MR_Track> filtered = new array<ref MR_Track>();
			foreach (MR_Track t : pool)
			{
				if (t.m_Id != excludeId)
					filtered.Insert(t);
			}
			pool = filtered;
		}

		if (pool.Count() == 0)
			return null;

		int idx = Math.RandomInt(0, pool.Count());
		return pool.Get(idx);
	}

	// ----------------------------------------------------------------- export
	// Serialised for the LIBRARY RPC (client only needs what is playable there)
	string SerializeManifest()
	{
		string s = "{\"v\":" + MR.PROTOCOL.ToString() + ",\"tracks\":[";
		bool first = true;
		foreach (MR_Track t : m_Tracks)
		{
			if (!t.m_Enabled)
				continue;
			if (!first)
				s = s + ",";
			first = false;
			// Local tracks are addressed by id and resolved against the
			// client's own copy of the media pack - no paths on the wire.
			// Поля трека собираются в массив: одна строка - один оператор.
			TStringArray parts = new TStringArray();
			parts.Insert("{\"id\":\"");
			parts.Insert(MR_Track.Escape(t.m_Id));
			parts.Insert("\",\"p\":\"");
			parts.Insert(MR_Track.Escape(t.m_Provider));
			parts.Insert("\",\"k\":\"");
			parts.Insert(MR_Track.Escape(t.m_Kind));
			parts.Insert("\",\"t\":\"");
			parts.Insert(MR_Track.Escape(t.m_Title));
			parts.Insert("\",\"a\":\"");
			parts.Insert(MR_Track.Escape(t.m_Artist));
			parts.Insert("\",\"al\":\"");
			parts.Insert(MR_Track.Escape(t.m_AlbumId));
			parts.Insert("\",\"d\":");
			parts.Insert(t.m_Duration.ToString());
			parts.Insert(",\"m\":");
			parts.Insert(t.m_MediaKind.ToString());
			parts.Insert("}");
			s = s + MR_Net.JoinParts(parts);
		}
		s = s + "}";
		return s;
	}
}

// ============================================================================
//  MR_FileUtils : small file system helpers (3_Game layer, no world objects)
// ============================================================================
class MR_FileUtils
{
	static bool IsJson(string name)
	{
		string n = name;
		n.ToLower();
		return MR.EndsWith(n, ".json");
	}

	// Collect *.json directly inside dir (non recursive).
	//
	// FILE ENUMERATION - the API is easy to get wrong, and three attempts at
	// this function failed for three different reasons. The canonical shape
	// (DayZ wiki, DZEXP-134 workaround, vanilla init.c):
	//
	//     FindFileHandle handle = FindFile(pattern, fileName, fileAttr, flags);
	//     if (handle)
	//     {
	//         // IMPORTANT: the FIRST match is already in fileName here
	//         bool found = true;
	//         while (found)
	//         {
	//             ... use fileName / fileAttr ...
	//             found = FindNextFile(handle, fileName, fileAttr);   // bool!
	//         }
	//         CloseFindFile(handle);
	//     }
	//
	// The mistakes that cost us compile errors (logs 17:46 and 18:00):
	//   * FindNextFile returns bool, not a handle. "handle = FindNextFile(...)"
	//     gives "Incompatible parameter 'handle'" whatever the argument count.
	//   * it takes THREE arguments: (handle, out fileName, out fileAttr).
	//   * the handle must be released with CloseFindFile().
	static int CollectJsonFiles(string dir, inout TStringArray files)
	{
		string pattern = dir + "/*.json";
		string fileName = string.Empty;
		FileAttr fileAttr;

		FindFileHandle handle = FindFile(pattern, fileName, fileAttr, FindFileFlags.ALL);
		if (!handle)
			return files.Count();

		// The first result is already delivered by FindFile() itself.
		bool found = true;
		while (found)
		{
			if (fileName != string.Empty && IsJson(fileName))
				files.Insert(dir + "/" + fileName);

			found = FindNextFile(handle, fileName, fileAttr);
		}

		CloseFindFile(handle);
		return files.Count();
	}

	// Collect sub directory NAMES (relative, single level) inside dir.
	static int CollectDirs(string dir, inout TStringArray dirs)
	{
		string pattern = dir + "/*";
		string fileName = string.Empty;
		FileAttr fileAttr;

		FindFileHandle handle = FindFile(pattern, fileName, fileAttr, FindFileFlags.DIRECTORIES);
		if (!handle)
			return dirs.Count();

		bool found = true;
		while (found)
		{
			if (fileName != string.Empty && fileName != "." && fileName != "..")
			{
				if (dirs.Find(fileName) == -1)
					dirs.Insert(fileName);
			}

			found = FindNextFile(handle, fileName, fileAttr);
		}

		CloseFindFile(handle);
		return dirs.Count();
	}
}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Config.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Config.c` - 175 строк, 5179 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_Config.c : reading/writing JSON settings on both sides, safe defaults.
//  Layer: 3_Game
// ============================================================================

class MR_Config
{
	// ---- server side --------------------------------------------------------
	// Reads $mission:MusicRadio/AudioConfig.json. If the file is missing a
	// documented default is written so admins always have something to edit.
	static MR_Settings LoadServer()
	{
		string dirTwo = MR.DIR_MISSION;
		MR_Config.EnsureDir(dirTwo);

		string path = dirTwo + MR.CFG_AUDIO;
		MR_Settings s = null;

		if (!FileExist(path))
		{
			s = new MR_Settings();
			string text;
			s.WriteJson(text);
			MR_Json.SaveText(path, text);
			MR.Log("Default server config written to " + path);
			return s;
		}

		MR_JsonValue root = MR_Json.ParseFile(path);
		if (!root)
		{
			MR.LogError("Server config is invalid, using defaults: " + path);
			return new MR_Settings();
		}

		s = MR_Settings.FromJson(root);

		MR.Log("Server config loaded from " + path);
		return s;
	}

	// ---- client side --------------------------------------------------------
	// The client only needs layout/limits for its own UI. It is sent by the
	// server at connect time; the local file caches the last known values so a
	// player who joins a modded server with a plain client is not lost.
	static MR_Settings LoadClient()
	{
		string path = MR.DIR_PROFILE + MR.CFG_AUDIO_SAVED;
		if (!FileExist(path))
			return new MR_Settings();

		MR_JsonValue root = MR_Json.ParseFile(path);
		if (!root)
			return new MR_Settings();

		return MR_Settings.FromJson(root);
	}

	static void SaveClient(MR_Settings s)
	{
		if (!s)
			return;
		MR_Config.EnsureDir(MR.DIR_PROFILE);
		string text;
		s.WriteJson(text);
		MR_Json.SaveText(MR.DIR_PROFILE + MR.CFG_AUDIO_SAVED, text);
	}

	// ---- helpers ------------------------------------------------------------
	static void EnsureDir(string dir)
	{
		if (dir == string.Empty)
			return;
		if (!FileExist(dir))
		{
			MakeDirectory(dir);
			MR.Log("Created directory " + dir);
		}
	}

	// Human readable summary for console / logs.
	static void DumpSummary(MR_Settings s)
	{
		if (!s)
			return;
		MR.Log("---- settings ----");
		MR.Log("vehicle radio      : " + s.m_EnableVehicleRadio.ToString());
		MR.Log("handheld radio     : " + s.m_EnableHandheldRadio.ToString());
		MR.Log("require radio item : " + s.m_RequireRadioItemInCargo.ToString());
		MR.Log("local tracks       : " + s.m_AllowLocalTracks.ToString());
		MR.Log("broadcast          : " + s.m_AllowBroadcast.ToString());
		MR.Log("default volume     : " + s.m_DefaultVolume.ToString());
		MR.Log("default radius     : " + s.m_DefaultRadius.ToString());
		MR.Log("database index     : " + s.m_DbPath);
	}
}

// ============================================================================
//  MR_AuditLog : append-only log for moderation (who played what, when).
//  Layer: 3_Game, server only in practice.
// ============================================================================
class MR_AuditLog
{
	// Статические поля инициализируются ТОЛЬКО нулём: "= string.Empty" в
	// script.log даёт "FIX-ME: Only null assignment is allowed on plain data".
	// У FileHandle те же правила, поэтому признак открытого файла - отдельный
	// bool, а не сравнение хендла с 0.
	protected static FileHandle m_Handle;
	protected static bool       m_Open;
	protected static string     m_Path;

	static void Open(string path)
	{
		if (m_Open)
			Close();

		m_Path = path;
		MR_Config.EnsureDir(MR.DIR_PROFILE);
		MR_Config.EnsureDir(MR.DIR_MISSION);
		// FileHandle is "typedef int[] FileHandle": it has no IsOpen(), and
		// "m_Handle != 0" gives "Incompatible parameter 'm_Handle'"
		// (logs 17:46 and 18:00). What DOES work is the engine's own truth
		// test on the array itself - the same "if (!handle)" the DayZ wiki
		// uses for FindFileHandle. A failed open returns an empty array.
		m_Handle = OpenFile(path, FileMode.WRITE);
		if (!m_Handle)
		{
			m_Open = false;
			MR.LogWarn("Audit log disabled (cannot open " + path + ")");
			return;
		}

		m_Open = true;
		Line("log opened, ver " + MR.VERSION);
	}

	static void Close()
	{
		if (!m_Open)
			return;

		Line("log closed");
		CloseFile(m_Handle);
		// CloseFile() closes the stream; the handle itself is an int[] and
		// cannot be nulled or set to 0 (that was mr_config.c:136). Only the
		// flag is reset - every write path checks m_Open first.
		m_Open = false;
	}

	static void Line(string text)
	{
		if (!m_Open)
			return;

		string stamp = MR_AuditLog.Stamp();
		FPrint(m_Handle, stamp + " | " + text + "\n");
	}

	static string Stamp()
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;

		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);

		return year.ToString() + "-" + MR.Pad2(month) + "-" + MR.Pad2(day) + " " + MR.Pad2(hour) + ":" + MR.Pad2(minute) + ":" + MR.Pad2(second);
	}
}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_AudioTypes.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_AudioTypes.c` - 266 строк, 10019 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_AudioTypes.c : settings objects and the "play request" that travels
//                    from the server to the clients.
//  Layer: 3_Game
// ============================================================================

// ---------------------------------------------------------------- settings
class MR_Settings
{
	// General
	int    m_Protocol;
	string m_Version;

	// What players may use
	bool   m_EnableVehicleRadio;
	bool   m_EnableHandheldRadio;
	bool   m_RequireRadioItemInCargo;   // vehicles: radio must be in cargo
	bool   m_AllowLocalTracks;          // may players play library tracks
	bool   m_AllowBroadcast;            // sound reaches other players in radius
	bool   m_AllowRadioTransmission;    // handheld radio -> same frequency
	bool   m_AllowRepeat;               // repeat / autoplay next track in a car
	bool   m_RadioNeedsCarrier;         // handheld radio plays only an inserted tape

	// Defaults
	int    m_DefaultVolume;             // 0..10
	float  m_DefaultRadius;             // meters
	int    m_MaxVolume;                 // 0..10
	float  m_MaxRadius;

	// Data
	string m_DbPath;                    // where the DB mod writes its json
	bool   m_EnableExplicit;            // allow "explicit" flagged tracks

	// Anti abuse
	int    m_CooldownSeconds;           // between two play requests per player
	bool   m_LogRequests;

	void MR_Settings()
	{
		Defaults();
	}

	void Defaults()
	{
		m_Protocol               = MR.PROTOCOL;
		m_Version                = MR.VERSION;
		m_EnableVehicleRadio     = true;
		m_EnableHandheldRadio    = true;
		m_RequireRadioItemInCargo = true;
		m_AllowLocalTracks       = true;
		m_AllowBroadcast         = true;
		m_AllowRadioTransmission = true;
		m_AllowRepeat            = true;
		m_RadioNeedsCarrier      = true;
		m_DefaultVolume          = 6;
		m_DefaultRadius          = MR.RADIUS_DEFAULT;
		m_MaxVolume              = MR.VOLUME_STEPS;
		m_MaxRadius              = MR.RADIUS_MAX;
		m_DbPath                 = MR.PREFIX_DB + "/Music/index.json";
		m_EnableExplicit         = false;
		m_CooldownSeconds        = 3;
		m_LogRequests            = true;
	}

	static MR_Settings FromJson(MR_JsonValue root)
	{
		MR_Settings s = new MR_Settings();
		if (!root || root.m_Type != MR_JsonValue.T_OBJECT)
			return s;

		MR_JsonValue g = root.Get("general");
		if (g)
		{
			s.m_DefaultVolume = g.GetInt("defaultVolume", s.m_DefaultVolume);
			s.m_DefaultRadius = g.GetFloat("defaultRadius", s.m_DefaultRadius);
			s.m_MaxVolume     = g.GetInt("maxVolume", s.m_MaxVolume);
			s.m_MaxRadius     = g.GetFloat("maxRadius", s.m_MaxRadius);
			s.m_EnableExplicit = g.GetBool("allowExplicit", s.m_EnableExplicit);
		}

		MR_JsonValue f = root.Get("features");
		if (f)
		{
			s.m_EnableVehicleRadio   = f.GetBool("vehicleRadio", s.m_EnableVehicleRadio);
			s.m_EnableHandheldRadio  = f.GetBool("handheldRadio", s.m_EnableHandheldRadio);
			s.m_RequireRadioItemInCargo = f.GetBool("requireRadioItemInCargo", s.m_RequireRadioItemInCargo);
			s.m_AllowLocalTracks     = f.GetBool("localTracks", s.m_AllowLocalTracks);
			s.m_AllowRepeat          = f.GetBool("autoplayNext", s.m_AllowRepeat);
			s.m_AllowBroadcast       = f.GetBool("broadcastToOthers", s.m_AllowBroadcast);
			s.m_AllowRadioTransmission = f.GetBool("radioTransmission", s.m_AllowRadioTransmission);
			s.m_RadioNeedsCarrier    = f.GetBool("radioNeedsCarrier", s.m_RadioNeedsCarrier);
		}

		MR_JsonValue l = root.Get("limits");
		if (l)
		{
			s.m_CooldownSeconds    = l.GetInt("cooldownSeconds", s.m_CooldownSeconds);
			s.m_LogRequests        = l.GetBool("logRequests", s.m_LogRequests);
		}

		MR_JsonValue d = root.Get("database");
		if (d)
			s.m_DbPath = d.GetString("index", s.m_DbPath);

		// Sanity
		s.m_DefaultVolume = MR.ClampInt(s.m_DefaultVolume, 0, MR.VOLUME_STEPS);
		s.m_MaxVolume     = MR.ClampInt(s.m_MaxVolume, 1, MR.VOLUME_STEPS);
		s.m_DefaultRadius = MR.ClampFloat(s.m_DefaultRadius, MR.RADIUS_MIN, MR.RADIUS_MAX);
		s.m_MaxRadius     = MR.ClampFloat(s.m_MaxRadius, MR.RADIUS_MIN, MR.RADIUS_MAX);

		return s;
	}

	// Ни одна строка не переносится: каждая - отдельный оператор.
	// Перенос выражения (даже с "+" в КОНЦЕ строки) Enforce Script не
	// принимает: mr_audiotypes.c(118) "Syntax error" + "Missing ';'".
	// Поэтому длинный текст собирается по частям обычными присваиваниями.
	void WriteJson(out string text)
	{
		text = "{\n";
		text = text + "  \"general\": {\n";
		text = text + "    \"defaultVolume\": " + m_DefaultVolume.ToString() + ",\n";
		text = text + "    \"defaultRadius\": " + m_DefaultRadius.ToString() + ",\n";
		text = text + "    \"maxVolume\": " + m_MaxVolume.ToString() + ",\n";
		text = text + "    \"maxRadius\": " + m_MaxRadius.ToString() + ",\n";
		text = text + "    \"allowExplicit\": " + MR_Track.BoolJson(m_EnableExplicit) + "\n";
		text = text + "  },\n";
		text = text + "  \"features\": {\n";
		text = text + "    \"vehicleRadio\": " + MR_Track.BoolJson(m_EnableVehicleRadio) + ",\n";
		text = text + "    \"handheldRadio\": " + MR_Track.BoolJson(m_EnableHandheldRadio) + ",\n";
		text = text + "    \"requireRadioItemInCargo\": " + MR_Track.BoolJson(m_RequireRadioItemInCargo) + ",\n";
		text = text + "    \"localTracks\": " + MR_Track.BoolJson(m_AllowLocalTracks) + ",\n";
		text = text + "    \"autoplayNext\": " + MR_Track.BoolJson(m_AllowRepeat) + ",\n";
		text = text + "    \"broadcastToOthers\": " + MR_Track.BoolJson(m_AllowBroadcast) + ",\n";
		text = text + "    \"radioTransmission\": " + MR_Track.BoolJson(m_AllowRadioTransmission) + ",\n";
		text = text + "    \"radioNeedsCarrier\": " + MR_Track.BoolJson(m_RadioNeedsCarrier) + "\n";
		text = text + "  },\n";
		text = text + "  \"limits\": {\n";
		text = text + "    \"cooldownSeconds\": " + m_CooldownSeconds.ToString() + ",\n";
		text = text + "    \"logRequests\": " + MR_Track.BoolJson(m_LogRequests) + "\n";
		text = text + "  },\n";
		text = text + "  \"database\": {\n";
		text = text + "    \"index\": \"" + m_DbPath + "\"\n";
		text = text + "  }\n";
		text = text + "}\n";
	}
}

// ------------------------------------------------------- play request (wire)
// This is what the server decides and what every client executes. It carries
// no file path in a client resolvable form: files are addressed through the
// locator + the client's own copy of the media pack.
class MR_PlayRequest
{
	MR_HostType   m_HostType;      // item / vehicle
	int           m_HostId;        // network low/high id of the host object
	int           m_HostIdHigh;
	// ВАЖНО: ref. Без него движок пишет
	//   mr_audiotypes.c,167: Variable 'm_Locator' is not strong ref
	ref MR_Locator m_Locator;    // provider + track id + seed
	int           m_StartOffset;   // seconds to skip (sync / late join)
	float         m_Radius;        // meters (broadcast)
	int           m_Volume;        // 0..10
	bool          m_Loop;
	bool          m_Broadcast;     // false = only the requesting client hears it

	void MR_PlayRequest()
	{
		m_HostType = MR_HostType.HOST_NONE;
		m_HostId = 0;
		m_HostIdHigh = 0;
		// Локатор создаёт вызывающий код (DecodePlay / сервер), здесь только
		// проверка на null в тех местах, где он используется.
		m_StartOffset = 0;
		m_Radius = MR.RADIUS_DEFAULT;
		m_Volume = 6;
		m_Loop = false;
		m_Broadcast = true;
	}
}

// ------------------------------------------------------------- session state
// One "radio" that is currently playing somewhere in the world. Lives on the
// server (source of truth) and as a lightweight mirror on each client.
class MR_Session
{
	string      m_Id;              // unique session id (host id + time)
	MR_HostType m_HostType;
	int         m_HostId;
	int         m_HostIdHigh;
	string      m_OwnerId;         // boilerplate of the player who started it
	string      m_OwnerName;
	string      m_TrackId;
	string      m_Provider;
	string      m_Title;
	// Album bound to the session when it was started from a cassette / disc
	// ("" for the normal library). Autoplay never leaves this album.
	string      m_AlbumId;
	int         m_StartTimeMs;     // server time when playback started
	int         m_StartOffset;     // media offset at start
	int         m_Duration;        // seconds
	int         m_Volume;
	float       m_Radius;
	bool        m_Loop;
	bool        m_Broadcast;

	void MR_Session()
	{
		m_Id = string.Empty;
		m_HostType = MR_HostType.HOST_NONE;
		m_HostId = 0;
		m_HostIdHigh = 0;
		m_OwnerId = string.Empty;
		m_OwnerName = string.Empty;
		m_TrackId = string.Empty;
		m_Provider = string.Empty;
		m_Title = string.Empty;
		m_AlbumId = string.Empty;
		m_StartTimeMs = 0;
		m_StartOffset = 0;
		m_Duration = 0;
		m_Volume = 6;
		m_Radius = MR.RADIUS_DEFAULT;
		m_Loop = false;
		m_Broadcast = true;
	}

	// Where the media should be right now (seconds).
	int CurrentOffset()
	{
		int elapsed = (GetGame().GetTime() - m_StartTimeMs) / 1000;
		return m_StartOffset + elapsed;
	}

	bool IsFinished()
	{
		if (m_Loop)
			return false;
		if (m_Duration <= 0)
			return false;
		return (CurrentOffset() >= m_Duration);
	}

	string SerializeConfig()
	{
		string s = m_Id + "\n";
		s = s + ((int)m_HostType).ToString() + "\n";
		s = s + m_HostId.ToString() + "\n";
		s = s + m_HostIdHigh.ToString() + "\n";
		s = s + m_OwnerId + "\n";
		s = s + m_OwnerName + "\n";
		s = s + m_TrackId + "\n";
		s = s + m_Provider + "\n";
		s = s + m_Title + "\n";
		s = s + m_StartTimeMs.ToString() + "\n";
		s = s + m_StartOffset.ToString() + "\n";
		s = s + m_Duration.ToString() + "\n";
		s = s + m_Volume.ToString() + "\n";
		s = s + m_Radius.ToString() + "\n";
		s = s + MR_Track.BoolJson(m_Loop) + "\n";
		s = s + MR_Track.BoolJson(m_Broadcast);
		return s;
	}
}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_DeviceBase.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_DeviceBase.c` - 319 строк, 9196 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_DeviceBase.c : the contract every "host" object must fulfil.
//  Layer: 3_Game
//
//  A host is anything that can emit radio audio in the world:
//    * a handheld radio item   (PersonalRadio, ...)
//    * a vehicle               (CarScript gets this via modded class)
//    * a static speaker        (optional)
//
//  The base keeps the replicated state and the hooks; the thin mixins in
//  4_World implement them for the concrete vanilla classes.
// ============================================================================

class MR_DeviceBase
{
	// --- replicated state (changes are pushed to clients by MR_ClientSync) ---
	protected bool        m_MR_IsPlaying;
	protected string      m_MR_TrackId;         // catalogue id, for the UI
	protected string      m_MR_TrackTitle;      // for the "now playing" widget
	protected string      m_MR_Provider;
	protected int         m_MR_Volume;          // 0..10
	protected float       m_MR_Radius;
	protected int         m_MR_OffsetAtStart;   // seconds
	protected int         m_MR_StartTimeMs;     // client clock when playback began
	protected int         m_MR_Duration;        // seconds, 0 = unknown / live
	protected bool        m_MR_EndReported;     // client already told the server

	// --- client only ---------------------------------------------------------
	// NOT "ref": EffectSound is reference counted by the manager and nulling a
	// ref member while the engine still owns the sound is a crash source.
	protected EffectSound m_MR_Sound;
	protected string      m_MR_SoundSet;
	protected int         m_MR_LastAppliedVolume;
	protected bool        m_MR_AudioBroken;     // soundset did not resolve

	void MR_Init()
	{
		m_MR_IsPlaying     = false;
		m_MR_TrackId       = string.Empty;
		m_MR_TrackTitle    = string.Empty;
		m_MR_Provider      = string.Empty;
		m_MR_Volume        = 6;
		m_MR_Radius        = MR.RADIUS_DEFAULT;
		m_MR_OffsetAtStart = 0;
		m_MR_StartTimeMs   = 0;
		m_MR_Sound         = null;
		m_MR_SoundSet      = string.Empty;
		m_MR_LastAppliedVolume = -1;
		m_MR_AudioBroken    = false;
		m_MR_Duration      = 0;
		m_MR_EndReported   = false;
	}

	// ---------------------------------------------------------------- getters
	bool MR_IsPlaying()
	{
		return m_MR_IsPlaying;
	}

	string MR_GetTrackTitle()
	{
		return m_MR_TrackTitle;
	}

	string MR_GetTrackId()
	{
		return m_MR_TrackId;
	}

	int MR_GetVolume()
	{
		return m_MR_Volume;
	}

	float MR_GetRadius()
	{
		return m_MR_Radius;
	}

	int MR_GetElapsedSeconds()
	{
		if (!m_MR_IsPlaying)
			return 0;
		int elapsed = (GetGame().GetTime() - m_MR_StartTimeMs) / 1000;
		return m_MR_OffsetAtStart + elapsed;
	}

	// ---------------------------------------------------------------- setters
	void MR_SetState(string trackId, string title, string provider, int volume, float radius, int offsetSeconds)
	{
		m_MR_TrackId       = trackId;
		m_MR_TrackTitle    = title;
		m_MR_Provider      = provider;
		m_MR_Volume        = MR.ClampInt(volume, 0, MR.VOLUME_STEPS);
		m_MR_Radius        = MR.ClampFloat(radius, MR.RADIUS_MIN, MR.RADIUS_MAX);
		m_MR_OffsetAtStart = offsetSeconds;
		m_MR_StartTimeMs   = GetGame().GetTime();
		m_MR_IsPlaying     = true;
	}

	void MR_SetDurationClient(int duration)
	{
		m_MR_Duration = duration;
		m_MR_EndReported = false;
	}

	int MR_GetDuration()
	{
		return m_MR_Duration;
	}

	bool MR_WantsEndReport()
	{
		return m_MR_EndReported;
	}

	void MR_MarkEndReported()
	{
		m_MR_EndReported = true;
	}

	// Volume changed by the server on behalf of another player.
	void MR_SetVolumeClient(int volume)
	{
		m_MR_Volume = MR.ClampInt(volume, 0, MR.VOLUME_STEPS);
		MR_AudioApplyVolume();
	}

	void MR_ClearState()
	{
		m_MR_IsPlaying  = false;
		m_MR_TrackId    = string.Empty;
		m_MR_TrackTitle = string.Empty;
	}

	// ------------------------------------------------------------ host checks
	// Overridden by concrete hosts. Default: never usable.
	bool MR_CanPlay()
	{
		return false;
	}

	// Human readable reason why the radio cannot be used (shown to the player).
	string MR_BlockReason()
	{
		return "not available";
	}

	// Where the sound is anchored and how far it travels.
	Object MR_GetAudioObject()
	{
		return null;
	}

	float MR_GetEffectiveRadius()
	{
		return m_MR_Radius;
	}

	// Loudness ceiling of this host relative to the configured volume.
	// Overridden by the concrete hosts (a pocket deck is quieter than a car).
	float MR_GetVolumeCeiling()
	{
		return 1.0;
	}

	// ------------------------------------------------------ client playback
	// Starts audio on this host. Called by MR_ClientAudio (client only).
	bool MR_AudioStart(string soundSet, bool loop, int offsetSeconds)
	{
		MR_AudioStop(true);

		m_MR_SoundSet = soundSet;
		Object anchor = MR_GetAudioObject();
		if (!anchor)
		{
			MR.LogWarn("AudioStart: no anchor object");
			return false;
		}

		if (!MR_MediaResolver.SoundSetExists(soundSet))
		{
			m_MR_AudioBroken = true;
			MR.LogWarn("AudioStart: soundset not present on this client: " + soundSet);
			return false;
		}

		m_MR_AudioBroken = false;

		// Looping sets are started with loop=true; for one-shots we let the
		// engine report the end through Event_OnSoundWaveEnded.
		m_MR_Sound = SEffectManager.PlaySoundOnObject(soundSet, anchor, MR.FADE_IN, MR.FADE_OUT, loop);
		if (!m_MR_Sound)
		{
			MR.LogWarn("AudioStart: engine refused to play " + soundSet);
			return false;
		}

		// Resume inside a track: a tape remembers "2:17", not "track 3".
		//
		// EffectSound has NO SetStartOffset of its own - the method belongs to
		// AbstractWave. OMD_CarRadio had to mod EffectSound just to reach it:
		//     void OmdRadioSetStartOffset(float offset)
		//     { if (m_SoundWaveObject) m_SoundWaveObject.SetStartOffset(offset); }
		// We do the same, but in our own wrapper (MR_SoundMix.c) and with a
		// null check, so a sound that failed to build cannot crash the tick.
		//
		// Unit: SetStartOffset takes a 0..1 FRACTION of the wave, not seconds.
		// That is why this branch needs the track length; without one we start
		// from the beginning instead of guessing (a fraction computed from a
		// zero length would rewind the track to the end and play silence).
		if (offsetSeconds > 0 && m_MR_Duration > 0)
		{
			float fraction = (float)offsetSeconds / (float)m_MR_Duration;
			m_MR_Sound.MR_PlayWithOffset(MR.ClampFloat(fraction, 0, 0.999));
		}
		else if (offsetSeconds > 0)
		{
			MR.LogWarn("AudioStart: resume requested but track length is unknown, starting at 0:00");
		}

		m_MR_Sound.SetAutodestroy(true);

		// Per-host ceiling: a walkman must not be as loud as a car stereo.
		// Set ONCE here instead of multiplying the tick value, so the engine's
		// fade in / fade out keeps working (VOS_Radio's SetMaxVolume idea).
		m_MR_Sound.MR_SetVolumeMax(MR_GetVolumeCeiling());

		m_MR_LastAppliedVolume = -1;
		MR_AudioApplyVolume();
		return true;
	}

	void MR_AudioStop(bool immediate = false)
	{
		if (m_MR_Sound)
		{
			m_MR_Sound.SoundStop();
			m_MR_Sound = null;
		}
		m_MR_SoundSet = string.Empty;
		m_MR_LastAppliedVolume = -1;
	}

	bool MR_AudioIsPlaying()
	{
		return (m_MR_Sound != null && m_MR_Sound.IsPlaying());
	}

	// Volume = user volume x distance falloff, recomputed while the player
	// moves (called every tick by MR_ClientAudio).
	void MR_AudioApplyVolume()
	{
		if (!m_MR_Sound)
			return;

		float gain = MR.VolumeStepToGain(m_MR_Volume);
		float radius = MR_GetEffectiveRadius();

		Object anchor = MR_GetAudioObject();
		Object listener = GetGame().GetPlayer();
		if (anchor && listener)
		{
			float dist = vector.Distance(anchor.GetPosition(), listener.GetPosition());
			gain = gain * MR.DistanceGain(dist, radius);
		}

		int stepped = Math.Round(gain * 100);
		if (stepped == m_MR_LastAppliedVolume)
			return;

		m_MR_LastAppliedVolume = stepped;
		m_MR_Sound.SetSoundVolume(gain);
	}

	// ------------------------------------------------------------------- sync
	// Serialise the current state for the "late join" / periodic sync RPC.
	string MR_StateLine()
	{
		TStringArray parts = new TStringArray();
		parts.Insert(MR_Track.BoolJson(m_MR_IsPlaying));
		parts.Insert(m_MR_TrackId);
		parts.Insert(m_MR_TrackTitle);
		parts.Insert(m_MR_Provider);
		parts.Insert(m_MR_Volume.ToString());
		parts.Insert(m_MR_Radius.ToString());
		parts.Insert(MR_GetElapsedSeconds().ToString());
		return MR_Net.JoinParts(parts);
	}

	static void MR_ParseStateLine(string line, out bool playing, out string trackId, out string title, out string provider, out int volume, out float radius, out int offset)
	{
		playing = false;
		trackId = string.Empty;
		title = string.Empty;
		provider = string.Empty;
		volume = 6;
		radius = MR.RADIUS_DEFAULT;
		offset = 0;

		if (line == string.Empty)
			return;

		TStringArray parts = new TStringArray();
		line.Split("|", parts);
		if (parts.Count() < 7)
			return;

		playing  = MR.ToBool(parts.Get(0));
		trackId  = parts.Get(1);
		title    = parts.Get(2);
		provider = parts.Get(3);
		volume   = parts.Get(4).ToInt();
		radius   = parts.Get(5).ToFloat();
		offset   = parts.Get(6).ToInt();
	}
}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_ClientAudio.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_ClientAudio.c` - 187 строк, 5094 байт.

```cpp
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
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_CarrierState.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_CarrierState.c` - 235 строк, 8110 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_CarrierState.c : where the music state lives - ON THE TAPE.
//  Layer: 3_Game
//
//  WHY THIS FILE EXISTS
//  --------------------
//  Before this file the position of a song existed in three places that could
//  disagree: m_MR_OffsetAtStart on the host, the server session struct, and
//  nothing at all on the cassette. Consequences we could see in testing:
//    * pull the tape out and put it back -> the album starts from track one;
//    * restart the server while a tape plays -> the media is at 0:00;
//    * hand the tape to another player -> they hear it from the beginning.
//
//  The reference mod OMD_CarRadio solved it differently and better than we did:
//  the cassette class carries m_OmdRadioCurrentTrack / m_OmdRadioCurrentTrackTime
//  and saves both in OnStoreSave. The tape is the object that travels, so the
//  tape is the object that remembers.
//
//  We keep the same idea, but NOT the same fields:
//    - OMD kept one int track index; we keep the library track id, because our
//      catalogue ids are stable across media pack rebuilds and OMD's indices
//      break the moment a track is inserted in the middle;
//    - OMD wrote a float per cassette; we also store the deck time, so a "side
//      A 2:17" survives a restart of a server whose clock is not monotonic;
//    - OMD read the config path on every call (CfgVehicles <type> CfgCassette);
//      we cache the resolved SoundSet list on first use.
//
//  Everything here is layer 3_Game on purpose: the server writes the state, the
//  client reads it, and both must see the same class without a cast.
// ============================================================================

// One remembered position inside one track of one album.
class MR_CarrierMemory
{
	string m_TrackId;      // library id, e.g. "kostya_02"
	int    m_Position;     // seconds from the start of that track
	int    m_TrackNo;      // 1-based position inside the album, 0 = unknown
	int    m_SavedAtMs;    // server clock when it was written (audit only)

	void MR_CarrierMemory()
	{
		m_TrackId = string.Empty;
		m_Position = 0;
		m_TrackNo = 0;
		m_SavedAtMs = 0;
	}

	bool IsEmpty()
	{
		return (m_TrackId == string.Empty);
	}

	void Clear()
	{
		m_TrackId = string.Empty;
		m_Position = 0;
		m_TrackNo = 0;
		m_SavedAtMs = 0;
	}
}

// ============================================================================
//  Config side: CfgVehicles <cassette> CfgCassette
// ----------------------------------------------------------------------------
//  This block is the open contract borrowed from VOS_Radio: a carrier may
//  declare its own playlist, so a third party mod can ship a tape without
//  knowing our media pack index. Our own carriers keep using the album key
//  derived from the class name; CfgCassette is the fallback.
//
//      class CfgCassette
//      {
//          isPlaylist = 1;
//          soundSets[] = { "MR_MUSICRADIODB_KOSTYA_01_SoundSet", ... };
//      };
// ============================================================================
class MR_CarrierConfig
{
	// "/CfgVehicles MR_Cassette_Rock01 CfgCassette" or "" when absent.
	static string PathFor(string className)
	{
		if (className == string.Empty)
			return string.Empty;

		string path = "CfgVehicles " + className + " CfgCassette";
		if (!GetGame().ConfigIsExisting(path))
			return string.Empty;
		return path;
	}

	static bool HasBlock(string className)
	{
		return (PathFor(className) != string.Empty);
	}

	// SoundSets declared on the item itself. Empty when the tape is silent.
	static void SoundSetsOf(string className, out TStringArray sets)
	{
		sets = new TStringArray();

		string path = PathFor(className);
		if (path == string.Empty)
			return;

		string listPath = path + " soundSets";
		if (GetGame().ConfigIsExisting(listPath))
			GetGame().ConfigGetTextArray(listPath, sets);

		// A single-shot tape (not a playlist) declares one soundSet only.
		if (sets.Count() == 0)
		{
			string one = string.Empty;
			string onePath = path + " soundSet";
			if (GetGame().ConfigIsExisting(onePath))
			{
				one = GetGame().ConfigGetTextOut(onePath);
				if (one != string.Empty)
					sets.Insert(one);
			}
		}
	}

	static bool IsPlaylist(string className)
	{
		string path = PathFor(className);
		if (path == string.Empty)
			return false;

		if (!GetGame().ConfigIsExisting(path + " isPlaylist"))
			return false;

		return (GetGame().ConfigGetInt(path + " isPlaylist") == 1);
	}

	// Human readable label for logs. ConfigCssPath does not exist, so the
	// fallback is the raw class name - enough to find the tape in the log.
	static string Label(string className)
	{
		if (className == string.Empty)
			return "(no carrier)";
		return className;
	}
}

// ============================================================================
//  State side: the memory kept on one physical cassette / mini disc.
// ----------------------------------------------------------------------------
//  These methods are added to the item classes in 4_World (MR_CarrierState_
//  Hooks.c) because 3_Game must not reference EntityAI subclasses. The logic
//  itself lives here so the server engine and the codec below share ONE copy -
//  OMD duplicated its player logic into radio.c and carscript.c and the copies
//  drifted apart; that is the mistake this split avoids.
// ============================================================================
class MR_CarrierState
{
	// --- codec ---------------------------------------------------------------
	// Wire / save format: "MRT1|marker|trackId|position|trackNo"
	//   marker is a version number, not a checksum: a newer server may add
	//   fields to the right and an older one must ignore the line instead of
	//   resuming a tape at a random second.
	static const string PREFIX = "MRT1";

	// OMD used a bare marker int (78410). A prefix is needed because
	// OnStoreSave returns an array that other mods may also write into.
	static string Encode(MR_CarrierMemory mem)
	{
		if (!mem || mem.IsEmpty())
			return string.Empty;

		TStringArray parts = new TStringArray();
		parts.Insert(PREFIX);
		parts.Insert(MR.TAPE_MARKER.ToString());
		parts.Insert(MR_Net.Sanitize(mem.m_TrackId));
		parts.Insert(mem.m_Position.ToString());
		parts.Insert(mem.m_TrackNo.ToString());
		return MR_Net.JoinParts(parts);
	}

	// Returns null when the line is not ours / not readable. Never throws.
	static MR_CarrierMemory Decode(string line)
	{
		if (line == string.Empty)
			return null;

		TStringArray f = new TStringArray();
		line.Split("|", f);
		if (f.Count() < 4)
			return null;
		if (f.Get(0) != PREFIX)
			return null;

		MR_CarrierMemory mem = new MR_CarrierMemory();
		// f.Get(1) is the version marker: parsed but not enforced yet, so a
		// future format with an extra field still loads here.
		mem.m_TrackId = f.Get(2);
		mem.m_Position = f.Get(3).ToInt();
		if (f.Count() > 4)
			mem.m_TrackNo = f.Get(4).ToInt();

		if (mem.m_TrackId == string.Empty)
			return null;
		if (mem.m_Position < 0)
			mem.m_Position = 0;
		if (mem.m_Position > MR.TAPE_SIDE_MAX)
			mem.m_Position = MR.TAPE_SIDE_MAX;
		return mem;
	}

	// --- sanity --------------------------------------------------------------
	// A position past the end of the track means "the tape ran on": the caller
	// decides whether to advance a track or rewind. We only report it.
	static bool IsFinished(MR_CarrierMemory mem, int trackDuration)
	{
		if (!mem)
			return false;
		if (trackDuration <= 0)
			return false;
		return (mem.m_Position >= trackDuration);
	}

	// Clamp a position into a track. Used before it reaches the audio layer:
	// SetStartOffset with a value past the end makes the engine play silence
	// for the whole track length instead of failing.
	static int ClampPosition(int position, int trackDuration)
	{
		if (position < 0)
			return 0;
		if (trackDuration > 0 && position > trackDuration)
			return trackDuration;
		if (position > MR.TAPE_SIDE_MAX)
			return MR.TAPE_SIDE_MAX;
		return position;
	}
}
```

### @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_SoundMix.c

`@MusicRadioCore/Scripts/3_Game/MusicRadio/MR_SoundMix.c` - 81 строк, 2928 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_SoundMix.c : per-sound ceiling on top of the global mix.
//  Layer: 3_Game
//
//  Straight from VOS_Radio (scripts/3_Game/EffectSound.c) - three lines that
//  replace what we were doing wrong. Until now MR_DeviceBase.MR_AudioApplyVolume
//  recomputed gain and called SetSoundVolume() every 250 ms tick, which:
//    * fights with the engine's own fade in / fade out,
//    * overwrites any distance curve a SoundSet declares in config,
//    * writes a new value even when nothing moved (the "stepped" guard helps,
//      but the value still walks around zero).
//
//  Two instruments instead of one:
//    m_SoundWaveVolumeMax  -> the ceiling of ONE sound (walkman quieter than a
//                             car stereo). Set once when playback starts.
//    SetSoundVolume(0..1)  -> the live distance attenuation, still per tick,
//                             but now it scales a ceiling instead of ignoring it.
// ============================================================================

modded class EffectSound
{
	// Ceiling for this sound only. 0..1 of the configured volumeFactor.
	// Kept as a separate method (not an override) so vanilla behaviour and
	// other mods that patch EffectSound are untouched.
	void MR_SetVolumeMax(float volume)
	{
		m_SoundWaveVolumeMax = MR.Clamp01(volume);
	}

	float MR_GetVolumeMax()
	{
		return m_SoundWaveVolumeMax;
	}

	// ---------------------------------------------------------- resume / seek
	// EffectSound is a WRAPPER: it exposes SoundPlay/SoundStop but not the
	// AbstractWave methods. SetStartOffset lives on m_SoundWaveObject and is
	// reachable only from here - effectSound.c keeps it protected and gives no
	// accessor. OMD_CarRadio patched EffectSound for exactly this reason.
	// Cost of not doing it: "Undefined function 'EffectSound.SetStartOffset'"
	// at compile time, which is what the 17:46 log showed.
	bool MR_HasWave()
	{
		return (m_SoundWaveObject != null);
	}

	// fraction: 0..1 of the wave. 0 = start, 0.5 = middle.
	// Returns false when the wave is not built yet (nothing to seek in).
	bool MR_SetStartOffset(float fraction)
	{
		if (!m_SoundWaveObject)
			return false;

		m_SoundWaveObject.SetStartOffset(MR.ClampFloat(fraction, 0, 0.999));
		return true;
	}

	// Offset must be applied after the wave exists and before it is audible,
	// otherwise the listener hears the head of the track and then a jump.
	// Order matters: SoundPlay() -> SetStartOffset() -> volume.
	bool MR_PlayWithOffset(float fraction)
	{
		bool started = SoundPlay();
		if (!started)
			return false;

		MR_SetStartOffset(fraction);
		return true;
	}

	// Kept for parity with the OMD helper: same guard, same meaning.
	void MR_SetSoundVolume(float volume)
	{
		if (!m_SoundWaveObject)
			return;

		m_SoundWaveObject.SetVolume(MR.Clamp01(volume));
	}
}
```

## 3. Core - 4_World

### @MusicRadioCore/Scripts/4_World/MusicRadio/MR_Carriers.c

`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Carriers.c` - 206 строк, 6697 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_Carriers.c : what you put INTO a deck. One file, one place per class.
//  Layer: 4_World
//
//  These classes are declared in config.cpp (CfgVehicles), so the script side
//  uses a plain "class" with the same name. "modded class" only injects into
//  classes the engine declares (ItemBase, CarScript, PersonalRadio); using it
//  on our own class gives "Unknown type 'MR_CassetteCase_Base'" at compile time.
//
//  Contents:
//    MR_CassetteCase_Base  - album identity + the position it stopped at
//    MR_MiniDisc_Base      - same contract, different class
//    MR_MemoryCard_Base    - a key, not a container: never holds audio
//
//  Persistence format lives in MR_CarrierState (3_Game). This file only knows
//  when to read and write it: OnStoreSave always writes the slot (even when
//  empty), because OnStoreLoad matches fields by position.
// ============================================================================

// ============================================================================
//  MR_CarrierBase : common parent for cassettes / mini discs / memory cards.
// ----------------------------------------------------------------------------
//  WHY THIS CLASS EXISTS (log 19:18, stage World):
//      mr_carriers.c,24: Function 'EEInit' is marked as override, but there is
//                        no function with this name in the base class
//
//  Our classes come from config.cpp, and config.cpp has nothing to do with the
//  SCRIPT inheritance tree: a config-level parent (Inventory_Base) does NOT give
//  the script class a base, so "override" had nothing to override and "super."
//  had nothing to call. The class must extend a real script class.
//
//  ItemBase is the right one: in vanilla scripts
//      typedef ItemBase Inventory_Base;        (itembase.c)
//      class ItemBase extends InventoryItem
//  so the script base matches the config base, and EEInit / EEAtive /
//  OnStoreSave / OnStoreLoad come from it for real.
//
//  Everything common lives here once; the three subclasses below only add
//  what makes them different (album label, hint, memory card is not a tape).
// ============================================================================
class MR_CarrierBase extends ItemBase
{
	protected string m_MR_TapeLine;                          // encoded memory
	protected ref MR_CarrierMemory m_MRTape;                 // decoded, cached
	protected ref TStringArray m_MRCfgSoundSets;             // CfgCassette cache

	override void EEInit()
	{
		super.EEInit();
		MR.Log("Carrier in world: " + GetType());
	}

	// The position is written by the SERVER (the deck reports it), read by
	// whoever inserts the tape next. Protected, not replicated: a net sync of a
	// changing int every second is exactly the traffic we are avoiding - the
	// position travels with the object, not with the network.
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);

		// Always write the slot, even when empty: OnStoreLoad matches fields by
		// position, so skipping the line would shift every field that follows
		// (that is how a mod silently corrupts another mod's saved data).
		ctx.Write(m_MR_TapeLine);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		string line = string.Empty;
		if (ctx.Read(line))
		{
			m_MR_TapeLine = line;
			m_MRTape = MR_CarrierState.Decode(line);
			if (m_MRTape)
				MR.Log("Tape resumes at: " + m_MRTape.m_TrackId + " +" + m_MRTape.m_Position.ToString() + "s");
		}
		return true;
	}

	// ------------------------------------------------------------------ memory
	// What this carrier remembers. Never null: callers get an empty object.
	MR_CarrierMemory MR_TapeMemory()
	{
		if (!m_MRTape)
			m_MRTape = MR_CarrierState.Decode(m_MR_TapeLine);
		if (!m_MRTape)
			m_MRTape = new MR_CarrierMemory();
		return m_MRTape;
	}

	bool MR_TapeHasMemory()
	{
		return (!MR_TapeMemory().IsEmpty());
	}

	string MR_TapeTrackId()
	{
		return MR_TapeMemory().m_TrackId;
	}

	int MR_TapePosition()
	{
		return MR_TapeMemory().m_Position;
	}

	int MR_TapeTrackNo()
	{
		return MR_TapeMemory().m_TrackNo;
	}

	// Only the server writes: the deck sends the position, the client never
	// invents one (two writers = the tape and the HUD disagree after a restart).
	void MR_TapeSetPosition(string trackId, int position, int trackNo)
	{
		if (!GetGame().IsServer())
			return;
		if (trackId == string.Empty)
			return;

		MR_CarrierMemory mem = new MR_CarrierMemory();
		mem.m_TrackId = trackId;
		mem.m_Position = MR_CarrierState.ClampPosition(position, -1);
		mem.m_TrackNo = trackNo;
		m_MRTape = mem;
		m_MR_TapeLine = MR_CarrierState.Encode(mem);
	}

	void MR_TapeForget()
	{
		if (!GetGame().IsServer())
			return;
		m_MRTape = new MR_CarrierMemory();
		m_MR_TapeLine = string.Empty;
	}

	// ---------------------------------------------- CfgCassette (VOS_Radio)
	// A carrier may declare its own playlist, so a third-party mod can ship a
	// tape without knowing our media pack index. Cached: ConfigIsExisting per
	// tick is a string comparison per frame, for no reason.
	void MR_CarrierSoundSets(out TStringArray sets)
	{
		if (!m_MRCfgSoundSets)
			MR_CarrierConfig.SoundSetsOf(GetType(), m_MRCfgSoundSets);
		sets = m_MRCfgSoundSets;
	}

	bool MR_CarrierIsPlaylist()
	{
		return MR_CarrierConfig.IsPlaylist(GetType());
	}

	// Album id this tape carries (lower-case, matches index.json).
	string MR_AlbumId()
	{
		return MR_MediaItems.AlbumOf(GetType());
	}

	// Cheap per-frame answer for the HUD: resolved once, reused.
	string MR_AlbumIdCached()
	{
		return MR_MediaItems.AlbumOf(GetType());
	}

	// A carrier is useless without a player; the HUD greys it out with this.
	string MR_CarrierHint()
	{
		return "Carrier '" + MR_AlbumId() + "'";
	}
};

class MR_CassetteCase_Base extends MR_CarrierBase
{
	// A cassette is useless without a player; the HUD greys it out with this.
	override string MR_CarrierHint()
	{
		return "Cassette '" + MR_AlbumId() + "' - put it in a cassette player";
	}
}

class MR_MiniDisc_Base extends MR_CarrierBase
{
	override string MR_CarrierHint()
	{
		return "MiniDisc '" + MR_AlbumId() + "' - put it in an MD player";
	}
}

class MR_MemoryCard_Base extends MR_CarrierBase
{
	// The card is a key, not a container: it never holds audio. Keeping the
	// method makes the intent explicit for anyone reading the code later.
	override string MR_CarrierHint()
	{
		return "Memory card - a key for the deck, it holds no music";
	}

	bool MR_HoldsMusic()
	{
		return false;
	}
}
```

### @MusicRadioCore/Scripts/4_World/MusicRadio/MR_Decks.c

`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Decks.c` - 385 строк, 12112 байт.

```cpp
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
```

### @MusicRadioCore/Scripts/4_World/MusicRadio/MR_DeckResume.c

`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_DeckResume.c` - 367 строк, 10509 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_DeckResume.c : the deck reports its position, the server writes the tape.
//  Layer: 4_World
//
//  THE SPLIT WE KEPT FROM OMD
//  --------------------------
//  OMD let the SERVER advance the tape clock (Timer.Run(1, ..., true) on the
//  server, clients never count time). That is the right call: a client cannot
//  be trusted with a timestamp and two clients never accumulate drift, because
//  neither of them is the source of truth.
//
//  What we did not keep: OMD's timer body also re-implemented playlist advance
//  and noise generation inside the timer. Here the clock does ONE thing - write
//  the current position onto the carrier. Everything else (what plays next) is
//  already decided by MR_ServerEngine and is not duplicated.
// ============================================================================

// ============================================================================
//  Deck host: a cassette deck / MD player as a MusicRadio host.
// ----------------------------------------------------------------------------
//  Until now MR_MediaItems registered decks in MR_Registry as HOST_PLAYER, but
//  no MR_DeviceBase existed for them and MR_ClientRPC.ResolveHost() only knew
//  HOST_ITEM and HOST_VEHICLE. A deck therefore registered, was found by the
//  server and then could not be played on the client. This class closes that.
// ============================================================================
class MR_DeckHost extends MR_DeviceBase
{
	protected EntityAI m_Deck;

	void MR_DeckHost(EntityAI deck)
	{
		MR_Init();
		m_Deck = deck;
	}

	EntityAI GetDeck()
	{
		return m_Deck;
	}

	override Object MR_GetAudioObject()
	{
		return m_Deck;
	}

	override bool MR_CanPlay()
	{
		if (!m_Deck)
			return false;
		if (m_Deck.IsRuined())
			return false;

		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(m_Deck);
		if (deck)
		{
			string why;
			return deck.MR_IsReady(why);
		}

		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(m_Deck);
		if (md)
		{
			string why2;
			return md.MR_IsReady(why2);
		}

		return false;
	}

	override string MR_BlockReason()
	{
		if (!m_Deck)
			return "player missing";

		string why;
		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(m_Deck);
		if (deck)
		{
			if (deck.MR_IsReady(why))
				return string.Empty;
			return why;
		}

		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(m_Deck);
		if (md)
		{
			if (md.MR_IsReady(why))
				return string.Empty;
			return why;
		}

		return "not a media player";
	}

	// A pocket deck is quieter than a car stereo. Half of the configured radius,
	// never less than 5 m - the same rule MR_ItemHost uses, so a tape deck and a
	// handheld radio behave consistently.
	override float MR_GetEffectiveRadius()
	{
		float r = super.MR_GetEffectiveRadius() * 0.5;
		return MR.ClampFloat(r, 5, MR.RADIUS_MAX);
	}

	// A pocket deck is a personal device: 50% of the configured volume, the
	// same value VOS_Radio used for its walkman (0.2 vs 0.4 for the desk unit).
	override float MR_GetVolumeCeiling()
	{
		return 0.5;
	}
}

// ============================================================================
//  MR_DeckState : server side bookkeeping of one playing carrier.
// ----------------------------------------------------------------------------
//  Keyed by host, exactly like MR_ServerEngine sessions, so a deck that is
//  destroyed or emptied simply disappears from the map on the next tick.
//  Nothing here runs on a client: MR_TapeSetPosition refuses to write there.
// ============================================================================
class MR_DeckState
{
	protected static ref map<string, ref MR_DeckRuntime> m_Playing;

	static void Ensure()
	{
		if (!m_Playing)
			m_Playing = new map<string, ref MR_DeckRuntime>();
	}

	static string Key(MR_HostType type, int idLow, int idHigh)
	{
		return ((int)type).ToString() + ":" + idLow.ToString() + ":" + idHigh.ToString();
	}

	// Called by the server engine when a play request was accepted. Resolves
	// which physical carrier is spinning so ticks know where to write.
	static void TrackStart(MR_HostType type, int idLow, int idHigh, string trackId, int trackNo, int offset)
	{
		if (!GetGame().IsServer())
			return;
		Ensure();

		MR_DeckRuntime rt = new MR_DeckRuntime();
		rt.m_HostType = type;
		rt.m_HostId = idLow;
		rt.m_HostIdHigh = idHigh;
		rt.m_TrackId = trackId;
		rt.m_TrackNo = trackNo;
		rt.m_StartOffset = offset;
		rt.m_StartMs = GetGame().GetTime();
		rt.m_Carrier = FindCarrier(type, idLow, idHigh);

		m_Playing.Set(Key(type, idLow, idHigh), rt);

		// First write happens immediately: if the server dies one second later
		// the tape still knows which track was running.
		Write(rt);
	}

	static void TrackStop(MR_HostType type, int idLow, int idHigh)
	{
		if (!GetGame().IsServer())
			return;
		Ensure();

		string key = Key(type, idLow, idHigh);
		MR_DeckRuntime rt = m_Playing.Get(key);
		if (rt)
			Write(rt);           // final position before forgetting
		m_Playing.Remove(key);
	}

	// The carrier physically inside this host ("" carrier -> nothing to write).
	static EntityAI FindCarrier(MR_HostType type, int idLow, int idHigh)
	{
		Object hostObj = MR_Registry.Find(type, idLow, idHigh);
		if (!hostObj)
			return null;

		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(hostObj);
		if (deck)
			return deck.MR_GetInsertedCassette();

		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(hostObj);
		if (md)
			return md.MR_GetInsertedDisc();

		// Рация - тоже устройство с физическим носителем (20261174), поэтому
		// позиция на "кассете в рации" запоминается так же, как в деке.
		PersonalRadio radio = PersonalRadio.Cast(hostObj);
		if (radio)
			return radio.MR_InsertedCarrier();

		// A car plays through the deck docked on its dash.
		CarScript car = CarScript.Cast(hostObj);
		if (car)
		{
			EntityAI docked = MR_MediaItems.DockedInVehicle(car);
			MR_CassettePlayer_Base cdeck = MR_CassettePlayer_Base.Cast(docked);
			if (cdeck)
				return cdeck.MR_GetInsertedCassette();

			MR_MiniDiscPlayer_Base cmd = MR_MiniDiscPlayer_Base.Cast(docked);
			if (cmd)
				return cmd.MR_GetInsertedDisc();

			// В слоте машины может лежать сама кассета (20261173) - у неё
			// тоже есть память позиции.
			if (MR_MediaItems.IsCarrier(docked))
				return docked;
		}
		return null;
	}

	// Seconds into the current track. The server clock is monotonic enough for
	// this: it is the same value the session uses for late joiners.
	static int Position(MR_DeckRuntime rt, int now)
	{
		if (!rt)
			return 0;
		int elapsed = (now - rt.m_StartMs) / 1000;
		return MR_CarrierState.ClampPosition(rt.m_StartOffset + elapsed, 0);
	}

	static void Write(MR_DeckRuntime rt)
	{
		if (!rt || !rt.m_Carrier)
			return;
		if (rt.m_TrackId == string.Empty)
			return;

		// m_Carrier объявлен EntityAI (так его отдают Cast-ы), а MR_TapeSetPosition
		// живёт в MR_CarrierBase: без приведения компилятор пишет
		//   Undefined function 'EntityAI.MR_TapeSetPosition'   (лог 20:47, 221)
		MR_CarrierBase carrier = MR_CarrierBase.Cast(rt.m_Carrier);
		if (!carrier)
			return;
		carrier.MR_TapeSetPosition(rt.m_TrackId, Position(rt, GetGame().GetTime()), rt.m_TrackNo);
	}

	// Server tick (MR_ServerEngine.Tick -> here). One write per playing host per
	// TAPE_TICK_MS; on a 40-slot server with three tapes running this is six
	// string writes per second, and none of them over the network.
	static void Tick()
	{
		if (!GetGame().IsServer())
			return;
		Ensure();

		TStringArray dead = new TStringArray();
		foreach (string key, MR_DeckRuntime rt : m_Playing)
		{
			if (!rt)
			{
				dead.Insert(key);
				continue;
			}

			// The tape may have been swapped while the deck kept playing: write
			// what is still in the slot and drop the entry when it changed.
			EntityAI carrier = FindCarrier(rt.m_HostType, rt.m_HostId, rt.m_HostIdHigh);
			if (!carrier || carrier != rt.m_Carrier)
			{
				Write(rt);
				dead.Insert(key);
				continue;
			}

			Write(rt);
		}

		foreach (string k : dead)
			m_Playing.Remove(k);
	}

	// The head moved inside the current track (rewind / fast-forward / a
	// deliberate seek from the UI). Called from ACT_SEEK.
	static void SeekTo(MR_HostType type, int idLow, int idHigh, int position)
	{
		if (!GetGame().IsServer())
			return;
		Ensure();

		string key = Key(type, idLow, idHigh);
		MR_DeckRuntime rt = m_Playing.Get(key);
		if (!rt)
			return;

		if (position < 0)
			position = 0;
		rt.m_StartOffset = position;
		rt.m_StartMs = GetGame().GetTime();
		Write(rt);
	}

	// True when the cassette inside this host is the one playing.
	static bool IsPlayingCarrier(EntityAI carrier)
	{
		if (!carrier)
			return false;
		Ensure();

		foreach (string key, MR_DeckRuntime rt : m_Playing)
		{
			if (rt && rt.m_Carrier == carrier)
				return true;
		}
		return false;
	}

	// Position this carrier remembers for a given album ("" when it does not
	// belong to it). Used when a tape is inserted into a deck.
	static bool ResumePoint(EntityAI carrier, string albumId, out string trackId, out int position)
	{
		trackId = string.Empty;
		position = 0;

		if (!carrier)
			return false;

		MR_CarrierMemory mem;
		MR_CassetteCase_Base tape = MR_CassetteCase_Base.Cast(carrier);
		if (tape)
			mem = tape.MR_TapeMemory();

		MR_MiniDisc_Base disc = MR_MiniDisc_Base.Cast(carrier);
		if (disc)
			mem = disc.MR_TapeMemory();

		if (!mem || mem.IsEmpty())
			return false;

		trackId = mem.m_TrackId;
		position = mem.m_Position;
		return true;
	}
}

class MR_DeckRuntime
{
	MR_HostType m_HostType;
	int         m_HostId;
	int         m_HostIdHigh;
	string      m_TrackId;
	int         m_TrackNo;
	int         m_StartOffset;
	int         m_StartMs;
	EntityAI    m_Carrier;

	void MR_DeckRuntime()
	{
		m_HostType = MR_HostType.HOST_NONE;
		m_HostId = 0;
		m_HostIdHigh = 0;
		m_TrackId = string.Empty;
		m_TrackNo = 0;
		m_StartOffset = 0;
		m_StartMs = 0;
		m_Carrier = null;
	}
}

// ============================================================================
//  Decks: create the host once and hand it out.
// ============================================================================
```

### @MusicRadioCore/Scripts/4_World/MusicRadio/MR_MediaItems.c

`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_MediaItems.c` - 385 строк, 12822 байт.

```cpp
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
```

### @MusicRadioCore/Scripts/4_World/MusicRadio/MR_Hosts.c

`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Hosts.c` - 213 строк, 5681 байт.

```cpp
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
```

### @MusicRadioCore/Scripts/4_World/MusicRadio/MR_Registry.c

`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Registry.c` - 193 строк, 4950 байт.

```cpp
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
```

### @MusicRadioCore/Scripts/4_World/MusicRadio/MR_WorldHooks.c

`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_WorldHooks.c` - 162 строк, 3987 байт.

```cpp
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
```

### @MusicRadioCore/Scripts/4_World/MusicRadio/MR_PlayerRPC.c

`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_PlayerRPC.c` - 60 строк, 3011 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_PlayerRPC.c : входящие RPC на клиенте.
//  Layer: 4_World
//
//  ПОЧЕМУ ЗДЕСЬ, А НЕ В MISSION
//  -------------------------------------------------------------------------
//  Серверу нужно доставить клиенту конфиг, каталог, PLAY/STOP/VOLUME и
//  состояние. Раньше это делал "modded class DayZGame { override OnRPC }" -
//  и не собралось ни разу:
//
//      Unknown type 'DayZGame'                          (лог 01:12)
//      Function 'OnRPC' is marked as override, but
//      there is no function with this name              (лог 01:23)
//
//  У Mission OnRPC нет вообще, а класс объекта игры в наших слоях недоступен.
//  Зато есть рабочий образец, который уже проверен на этом же сервере: наши
//  ЗАПРОСЫ идут с target = объект игрока и приходят в PlayerBase.OnRPC
//  (MR_ServerHooks.c). Ровно тот же адрес используем и в обратную сторону:
//  сервер шлёт пакет объекту игрока, клиент принимает его здесь.
//
//  Проверка идёт первой строкой: PlayerBase патчат многие моды, и наш
//  обработчик не должен даже читать чужой пакет.
// ============================================================================

// Мост 4_World -> 5_Mission: делегат ставится один раз из MR_ClientRPC.
class MR_MissionBridge
{
	static ref ScriptCaller ClientRpc;

	static void HandleClientRpc(int rpcType, ParamsReadContext ctx)
	{
		if (ClientRpc)
			ClientRpc.Invoke(rpcType, ctx);
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		// Диапазон наших id проверяем ДО IsClient: чужой пакет не трогаем.
		if (rpc_type < MR.RPC_CONFIG || rpc_type > MR.RPC_STATE)
			return;
		if (!GetGame().IsClient())
			return;

		// Client-side only: серверный обработчик живёт в @MusicRadioServer и
		// читает только RPC_REQUEST, поэтому ветки не пересекаются.
		//
		// Сам разбор живёт в 5_Mission (MR_ClientRPC), а этот файл - 4_World:
		// звать верхний слой нельзя (проверка слоёв это запрещает, движок тоже
		// грузит слои по порядку). Поэтому здесь только точка входа, а тело
		// подставляется из 5_Mission (MR_ClientRPC.Install).
		MR_MissionBridge.HandleClientRpc(rpc_type, ctx);
	}
}
```

### @MusicRadioCore/Scripts/4_World/MusicRadio/MR_Animations.c

`@MusicRadioCore/Scripts/4_World/MusicRadio/MR_Animations.c` - 62 строк, 2931 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_Animations.c : how a media player looks in the player's hands.
//  Layer: 4_World
//
//  Without this the deck is held like a pickaxe: the engine has no IK profile
//  for our classes, so it falls back to the default one-handed pose. VOS_Radio
//  shipped exactly this file (scripts/4_World/Animations.c) for its four radios.
//
//  Two strings per item:
//    .asi  - the workspace (which hand, how the arm is posed)
//    .anm  - the IK target for that item class
//
//  Profiles used, all vanilla, so nothing has to be exported with our mod:
//    props/player_main_1h_radio.asi + ik/gear/Radio.anm          big desk unit
//    player_main_1h.asi             + ik/gear/PersonalRadio.anm  pocket sized
//    player_main_1h.asi             + ik/gear/cassette.anm       a bare cassette
//
//  Keep the list in one place: MR_Registry/check_project.py do not see these
//  strings, so a typo here is only visible in game.
// ============================================================================

modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);

		// --- cassette deck -------------------------------------------------
		// A walkman-sized deck uses the PersonalRadio pose: the big radio pose
		// pushes it away from the chest and the animations start to clip.
		pType.AddItemInHandsProfileIK("MR_CassettePlayer",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/PersonalRadio.anm");

		pType.AddItemInHandsProfileIK("MR_CassettePlayer_Black",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/PersonalRadio.anm");

		// --- mini disc -----------------------------------------------------
		pType.AddItemInHandsProfileIK("MR_MiniDiscPlayer",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/PersonalRadio.anm");

		// --- carriers ------------------------------------------------------
		// Only the base classes are registered: every generated album class
		// (MR_Cassette_Rock01, ...) inherits the pose, so a media pack with 400
		// albums does not need 400 lines here.
		pType.AddItemInHandsProfileIK("MR_CassetteCase_Base",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/cassette.anm");

		pType.AddItemInHandsProfileIK("MR_MiniDisc_Base",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/cassette.anm");

		pType.AddItemInHandsProfileIK("MR_MemoryCard_Base",
			"dz/anims/workspaces/player/player_main/player_main_1h.asi",
			pBehavior, "dz/anims/anm/player/ik/gear/cassette.anm");
	}
}
```

## 4. Core - 5_Mission

### @MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_ClientRPC.c

`@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_ClientRPC.c` - 376 строк, 10879 байт.

```cpp
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
```

### @MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_ClientState.c

`@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_ClientState.c` - 128 строк, 2916 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_ClientState.c : what a client knows about the server radio world.
//  Layer: 5_Mission
// ============================================================================

class MR_ClientState
{
	protected static ref MR_Settings m_Settings;
	protected static ref array<string> m_Sessions;     // "type|idLow|idHigh|title|provider"
	protected static ref MR_DeviceBase m_OpenHost;     // host the menu was opened on
	protected static string m_LastMessage;

	static MR_Library Library()
	{
		return MR_LibraryIndex.Get();
	}

	static MR_Settings Settings()
	{
		if (!m_Settings)
			m_Settings = MR_Config.LoadClient();
		return m_Settings;
	}

	static void SetSettings(MR_Settings s)
	{
		if (s)
		{
			m_Settings = s;
			MR_ConfigRuntime.Set(s);
			MR_Config.SaveClient(s);
		}
	}

	static void SetOpenHost(MR_DeviceBase host)
	{
		m_OpenHost = host;
	}

	static MR_DeviceBase GetOpenHost()
	{
		return m_OpenHost;
	}

	static void SetMessage(string msg)
	{
		m_LastMessage = msg;
		MR.Log("UI: " + msg);
	}

	static string GetMessage()
	{
		return m_LastMessage;
	}

	// Rebuild the local library from the manifest the server pushed.
	static void ApplyManifest(string manifest)
	{
		MR_Library lib = Library();
		MR_JsonValue root = MR_Json.Parse(manifest);
		if (!root)
		{
			MR.LogWarn("Cannot parse library manifest");
			return;
		}

		array<ref MR_JsonValue> entries = root.GetArray("tracks");
		if (!entries)
			return;

		array<ref MR_Track> tracks = lib.All();
		tracks.Clear();
		lib.ClearIndex();

		int resolved = 0;
		int broken = 0;

		foreach (MR_JsonValue entry : entries)
		{
			MR_Track t = new MR_Track();
			t.m_Id        = entry.GetString("id");
			t.m_Provider  = entry.GetString("p");
			t.m_Kind      = entry.GetString("k", "music");
			t.m_Title     = entry.GetString("t");
			t.m_Artist    = entry.GetString("a");
			t.m_AlbumId   = entry.GetString("al");
			t.m_Duration  = entry.GetInt("d", 0);
			t.m_MediaKind = MR.MediaKindFromInt(entry.GetInt("m", 0));

			// Validate against the media packs actually installed on this client.
			if (t.m_MediaKind == MR_MediaKind.MEDIA_TRACK)
			{
				string err;
				string ss = MR_MediaResolver.ResolveTrack(t, err);
				if (ss == string.Empty)
				{
					broken = broken + 1;
					t.m_ResolveError = err;
				}
				else
					resolved = resolved + 1;
			}
			else
				resolved = resolved + 1;

			tracks.Insert(t);
		}

		lib.Reindex();

		MR.Log("Library synced: " + tracks.Count().ToString() + " entries, " + resolved.ToString() + " playable, " + broken.ToString() + " unavailable");

		if (broken > 0)
			MR_ClientState.SetMessage(broken.ToString() + " track(s) need a media pack update");
	}

	static void SetSessions(array<string> sessions)
	{
		m_Sessions = sessions;
	}

	static array<string> GetSessions()
	{
		return m_Sessions;
	}
}
```

### @MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_LibraryIndex.c

`@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_LibraryIndex.c` - 23 строк, 550 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_LibraryIndex.c : the one place where a media library instance lives.
//  Layer: 5_Mission   (UI + RPC code reads it from here)
// ============================================================================

class MR_LibraryIndex
{
	protected static ref MR_Library m_Library;

	static MR_Library Get()
	{
		if (!m_Library)
			m_Library = new MR_Library();
		return m_Library;
	}

	static void Reset()
	{
		m_Library = new MR_Library();
	}
}
```

### @MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_RadioMenu.c

`@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_RadioMenu.c` - 629 строк, 17500 байт.

```cpp
// ============================================================================
//  MusicRadio - Core
//  MR_RadioMenu.c : the radio UI (overlay menu).
//  Layer: 5_Mission
//
//  Widget names come from gui/layouts/radio_ui.layout and radio_row.layout.
//  The menu is deliberately plain: vanilla fonts, no images, so it works on
//  every build and does not fight other UI mods.
//
//  Event routing
//  -------------
//  In DayZ a widget only delivers OnClick/OnChange to the handler that was set
//  ON THAT WIDGET - a parent's handler does not receive child events. So every
//  interactive widget is hooked to this menu explicitly in SetWidgetHandler().
// ============================================================================

// ---------------------------------------------------------------------------
//  Кто принимает клики
// ---------------------------------------------------------------------------
//  Widget.SetHandler() принимает ТОЛЬКО ScriptedWidgetEventHandler:
//      proto native void SetHandler(ScriptedWidgetEventHandler eventHandler);
//  А UIScriptedMenu происходит от UIMenuPanel (Managed) - обработчиком он не
//  является. Поэтому "m_Root.SetHandler(m_Events)" из UIScriptedMenu даёт на
//  КАЖДОЙ строке вызова:
//      Types 'MR_RadioMenu' and 'ScriptedWidgetEventHandler' are unrelated
//      Cannot convert 'MR_RadioMenu' to 'ScriptedWidgetEventHandler'
//  (лог 01:41, строки 112, 130, 307-311 - 20 наших ошибок).
//
//  Решение: меню остаётся UIScriptedMenu (иначе UIManager.ShowScriptedMenu его
//  не покажет и Init/Update не будут вызываться), а на виджеты вешается
//  маленький пересыльщик MR_MenuEvents. Так же устроена ваниль:
//  OptionsMenuGame/PlayerListScriptedWidget - это ScriptedWidgetEventHandler,
//  которым владеет меню-окно.
// ---------------------------------------------------------------------------

class MR_MenuEvents extends ScriptedWidgetEventHandler
{
	protected ref MR_RadioMenu m_Menu;

	void MR_MenuEvents(MR_RadioMenu menu)
	{
		m_Menu = menu;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnClick(w, x, y, button);
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnChange(w, x, y, finished);
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (!m_Menu)
			return false;
		return m_Menu.OnClick(w, x, y, button);
	}
}

class MR_RadioMenu extends UIScriptedMenu
{
	protected const string LAYOUT_MAIN = "MusicRadioCore/gui/layouts/radio_ui.layout";
	protected const string LAYOUT_ROW  = "MusicRadioCore/gui/layouts/radio_row.layout";

	// widgets
	protected Widget         m_Root;
	protected TextWidget     m_NowPlaying;
	protected TextWidget     m_TrackInfo;
	protected TextWidget     m_Status;
	protected TextWidget     m_Footer;
	protected TextWidget     m_VolumeLabel;
	protected TextWidget     m_RadiusLabel;
	protected SliderWidget   m_VolumeSlider;
	protected SliderWidget   m_RadiusSlider;
	protected ButtonWidget   m_PlayBtn;
	protected ButtonWidget   m_StopBtn;
	protected ButtonWidget   m_NextBtn;
	protected CheckBoxWidget m_Broadcast;
	protected EditBoxWidget  m_Search;
	protected WrapSpacerWidget m_List;

	// state
	protected int  m_Tab;                       // 0 music, 1 all tracks, 2 tapes/discs
	protected ref array<ref MR_Track> m_View;   // what the list shows right now
	protected ref map<Widget, ref MR_Track> m_RowTrack;
	protected int  m_Selected;
	protected ref MR_DeviceBase m_Host;
	protected MR_HostType m_HostType;
	protected int  m_HostIdLow;
	protected int  m_HostIdHigh;
	protected ref MR_MenuEvents m_Events;   // приёмник кликов, см. выше
	protected bool m_RowsBuilt;
	protected string m_AlbumId;                 // set when a tape/disc tab is shown

	void MR_RadioMenu()
	{
		m_Tab = 0;
		m_Selected = -1;
		m_View = new array<ref MR_Track>();
		m_RowTrack = new map<Widget, ref MR_Track>();
		m_HostType = MR_HostType.HOST_NONE;
		m_Events = new MR_MenuEvents(this);
	}

	// ----------------------------------------------------------------- setup
	override Widget Init()
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets(LAYOUT_MAIN);
		if (!m_Root)
		{
			MR.LogError("Cannot create UI from " + LAYOUT_MAIN);
			return null;
		}

		m_NowPlaying   = TextWidget.Cast(m_Root.FindAnyWidget("MR_NowPlaying"));
		m_TrackInfo    = TextWidget.Cast(m_Root.FindAnyWidget("MR_TrackInfo"));
		m_Status       = TextWidget.Cast(m_Root.FindAnyWidget("MR_StatusLabel"));
		m_Footer       = TextWidget.Cast(m_Root.FindAnyWidget("MR_Footer"));
		m_VolumeLabel  = TextWidget.Cast(m_Root.FindAnyWidget("MR_VolumeLabel"));
		m_RadiusLabel  = TextWidget.Cast(m_Root.FindAnyWidget("MR_RadiusLabel"));
		m_VolumeSlider = SliderWidget.Cast(m_Root.FindAnyWidget("MR_VolumeSlider"));
		m_RadiusSlider = SliderWidget.Cast(m_Root.FindAnyWidget("MR_RadiusSlider"));
		m_PlayBtn      = ButtonWidget.Cast(m_Root.FindAnyWidget("MR_PlayButton"));
		m_StopBtn      = ButtonWidget.Cast(m_Root.FindAnyWidget("MR_StopButton"));
		m_NextBtn      = ButtonWidget.Cast(m_Root.FindAnyWidget("MR_NextButton"));
		m_Broadcast    = CheckBoxWidget.Cast(m_Root.FindAnyWidget("MR_BroadcastCheck"));
		m_Search       = EditBoxWidget.Cast(m_Root.FindAnyWidget("MR_SearchBox"));
		m_List         = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("MR_TrackList"));

		MR_Settings s = MR_ClientState.Settings();

		if (m_VolumeSlider)
		{
			m_VolumeSlider.SetMinMax(0, s.m_MaxVolume);
			m_VolumeSlider.SetCurrent(s.m_DefaultVolume);
		}
		if (m_RadiusSlider)
		{
			m_RadiusSlider.SetMinMax(MR.RADIUS_MIN, s.m_MaxRadius);
			m_RadiusSlider.SetCurrent(s.m_DefaultRadius);
		}
		if (m_Broadcast)
			m_Broadcast.SetChecked(s.m_AllowBroadcast);

		SetWidgetHandler("MR_CloseButton");
		SetWidgetHandler("MR_TabMusic");
		SetWidgetHandler("MR_TabAll");
		SetWidgetHandler("MR_TabTapes");
		SetWidgetHandler("MR_PlayButton");
		SetWidgetHandler("MR_StopButton");
		SetWidgetHandler("MR_NextButton");
		SetWidgetHandler("MR_VolumeSlider");
		SetWidgetHandler("MR_RadiusSlider");
		SetWidgetHandler("MR_SearchBox");
		SetWidgetHandler("MR_BroadcastCheck");

		m_Root.SetHandler(m_Events);

		UpdateLabels();
		MR_ClientRPC.RequestLibrary();
		RefreshList();
		return m_Root;
	}

	// Attach this menu as the event handler of a named child widget.
	// NOTE: имя Hook не годится - в Object это protected-метод движка,
	// и компилятор отвечает 'Method Object is private'. Именно на этом
	// падала компиляция Mission в ревизии 20261165.
	void SetWidgetHandler(string name)
	{
		if (!m_Root)
			return;
		Widget w = m_Root.FindAnyWidget(name);
		if (w)
			w.SetHandler(m_Events);
	}

	override void OnShow()
	{
		super.OnShow();
		GetGame().GetInput().ChangeGameFocus(1);
		if (m_Search)
			SetFocus(m_Search);
	}

	override void OnHide()
	{
		// Tell the mission the menu is gone so F7 re-opens instead of closing.
		// ВАЖНО: этот OnHide - единственный в классе. Второй такой же давал
		//   Multiple declaration of function 'OnHide'   (лог 01:23, строка 560)
		// и Mission-модуль не собирался вообще.
		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (mission)
			mission.MR_NotifyMenuClosed();

		super.OnHide();
		GetGame().GetInput().ChangeGameFocus(-1);
	}

	// Which host is this menu driving? Set by MissionGameplay before opening.
	void SetHost(MR_DeviceBase host, MR_HostType type, Object obj)
	{
		m_Host = host;
		m_HostType = type;
		if (obj)
		{
			MR.NetId(obj, m_HostIdLow, m_HostIdHigh);
		}
		if (host)
			MR_ClientState.SetOpenHost(host);
	}

	// ------------------------------------------------------------------ tabs
	void SelectTab(int tab)
	{
		m_Tab = tab;
		m_Selected = -1;
		m_RowsBuilt = false;
		RefreshList();
		UpdateLabels();
	}

	void ClearRows()
	{
		if (!m_List)
			return;

		m_RowTrack.Clear();

		Widget child = m_List.GetChildren();
		while (child)
		{
			Widget next = child.GetSibling();
			child.Unlink();
			child = next;
		}
	}

	void RefreshList()
	{
		if (!m_List)
			return;

		ClearRows();
		m_View.Clear();

		MR_Library lib = MR_ClientState.Library();
		string query = string.Empty;
		if (m_Search)
			query = m_Search.GetText();

		// Tab 2 is the "what is in my deck" view: one album, the physical one.
		m_AlbumId = string.Empty;
		if (m_Tab == 2)
		{
			m_AlbumId = AlbumInThisDeck();
			if (m_AlbumId == string.Empty)
			{
				CreatePlaceholder("No cassette or disc is loaded. Insert one into the player.");
				m_RowsBuilt = true;
				UpdateFooter(lib);
				return;
			}
			m_View = lib.ByAlbum(m_AlbumId);
		}
		else if (m_Tab == 0)
		{
			m_View = lib.Search(query, "music");
		}
		else
		{
			m_View = lib.Search(query, "");
		}

		int shown = 0;
		for (int i = 0; i < m_View.Count(); i++)
		{
			MR_Track t = m_View.Get(i);
			if (!t)
				continue;

			CreateRow(t, shown);
			shown = shown + 1;
		}

		if (shown == 0)
			CreatePlaceholder("Nothing here. Check the media pack or your search.");

		m_RowsBuilt = true;
		UpdateFooter(lib);
	}

	// Which album is physically loaded in the device this menu is driving?
	// Car: the deck docked on the dash. Handheld: the player-carried deck.
	string AlbumInThisDeck()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return string.Empty;

		if (m_HostType == MR_HostType.HOST_VEHICLE)
		{
			Object hostObj = MR_Registry.Find(m_HostType, m_HostIdLow, m_HostIdHigh);
			CarScript car = CarScript.Cast(hostObj);
			if (car)
				return car.MR_DockedAlbum();
			return string.Empty;
		}

		// HOST_ITEM = рация в руках: у неё теперь тоже есть вставленный
		// носитель, поэтому вкладка TAPES показывает альбом ИЗ РАЦИИ.
		if (m_HostType == MR_HostType.HOST_ITEM)
			return player.MR_HeldAlbum();

		return player.MR_CarriedAlbum();
	}

	void UpdateFooter(MR_Library lib)
	{
		if (!m_Footer)
			return;

		string text = "Library: " + lib.Count().ToString() + " tracks";
		if (m_AlbumId != string.Empty)
			text = "Album '" + m_AlbumId + "': " + m_View.Count().ToString() + " tracks (whole side, loops)";
		else if (m_Host && m_Host.MR_IsPlaying())
			text = text + "   |   next track starts automatically";

		m_Footer.SetText(text + "   |   F7 closes, F8 stops");
	}

	void CreateRow(MR_Track track, int index)
	{
		Widget row = GetGame().GetWorkspace().CreateWidgets(LAYOUT_ROW, m_List);
		if (!row)
			return;

		TextWidget idx    = TextWidget.Cast(row.FindAnyWidget("MR_RowIndex"));
		TextWidget title  = TextWidget.Cast(row.FindAnyWidget("MR_RowTitle"));
		TextWidget artist = TextWidget.Cast(row.FindAnyWidget("MR_RowArtist"));
		TextWidget dur    = TextWidget.Cast(row.FindAnyWidget("MR_RowDuration"));

		if (idx)    idx.SetText((index + 1).ToString());
		if (title)  title.SetText(track.m_Title);
		if (artist) artist.SetText(track.m_Artist);
		if (dur)    dur.SetText(track.DurationText());

		// Grey out entries this client cannot play (media pack missing / older).
		bool playable = (track.m_ResolveError == string.Empty && track.m_MediaKind != MR_MediaKind.MEDIA_NONE);
		if (!playable)
		{
			if (title)  title.SetColor(ARGB(255, 140, 90, 90));
			if (artist) artist.SetText("pack missing");
		}

		// Every clickable part of the row must point at this menu.
		row.SetHandler(m_Events);
		if (idx)    idx.SetHandler(m_Events);
		if (title)  title.SetHandler(m_Events);
		if (artist) artist.SetHandler(m_Events);
		if (dur)    dur.SetHandler(m_Events);

		m_RowTrack.Set(row, track);
	}

	void CreatePlaceholder(string text)
	{
		Widget row = GetGame().GetWorkspace().CreateWidgets(LAYOUT_ROW, m_List);
		if (!row)
			return;
		TextWidget title = TextWidget.Cast(row.FindAnyWidget("MR_RowTitle"));
		if (title)
		{
			title.SetText(text);
			title.SetColor(ARGB(255, 130, 130, 130));
		}
	}

	// ----------------------------------------------------------------- labels
	void UpdateLabels()
	{
		if (m_NowPlaying)
		{
			if (m_Host && m_Host.MR_IsPlaying())
				m_NowPlaying.SetText(m_Host.MR_GetTrackTitle());
			else
				m_NowPlaying.SetText("not playing");
		}

		if (m_Status)
			m_Status.SetText(MR_ClientState.GetMessage());

		if (m_VolumeSlider && m_VolumeLabel)
			m_VolumeLabel.SetText("VOLUME " + Math.Round(m_VolumeSlider.GetCurrent()).ToString() + "%");

		if (m_RadiusSlider && m_RadiusLabel)
			m_RadiusLabel.SetText("RANGE " + Math.Round(m_RadiusSlider.GetCurrent()).ToString() + " m");

		if (m_PlayBtn)
		{
			// тернарник - отдельным оператором, не аргументом (см. MR_Net.c)
			string label = "PLAY";
			if (m_Tab == 2)
				label = "PLAY TAPE";
			m_PlayBtn.SetText(label);
		}
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);

		if (m_Host && m_NowPlaying && m_Host.MR_IsPlaying())
		{
			string t = m_Host.MR_GetTrackTitle();
			if (m_Host.MR_GetDuration() > 0)
				t = t + "   " + MR_RadioMenu.TimeText(m_Host.MR_GetElapsedSeconds());
			m_NowPlaying.SetText(t);
		}
	}

	static string TimeText(int seconds)
	{
		int m = seconds / 60;
		int s = seconds % 60;
		return m.ToString() + ":" + MR.Pad2(s);
	}

	// ----------------------------------------------------------------- events
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!w)
			return false;

		string name = w.GetName();

		if (name == "MR_CloseButton")
		{
			Close();
			return true;
		}
		if (name == "MR_TabMusic")
		{
			SelectTab(0);
			return true;
		}
		if (name == "MR_TabAll")
		{
			SelectTab(1);
			return true;
		}
		if (name == "MR_TabTapes")
		{
			SelectTab(2);
			return true;
		}
		if (name == "MR_PlayButton")
		{
			DoPlay();
			return true;
		}
		if (name == "MR_StopButton")
		{
			DoStop();
			return true;
		}
		if (name == "MR_NextButton")
		{
			DoNext();
			return true;
		}

		// A click lands either on the row frame or on one of its texts: walk up
		// until the row is found.
		Widget row = w;
		int guard = 0;
		while (row && row.GetName() != "MR_Row" && row != m_Root && guard < 8)
		{
			row = row.GetParent();
			guard = guard + 1;
		}

		if (row && row.GetName() == "MR_Row")
		{
			SelectRow(row);
			if (button == MouseState.MIDDLE)
				DoPlay();
			return true;
		}

		return super.OnClick(w, x, y, button);
	}

	void SelectRow(Widget row)
	{
		// m_RowTrack is authoritative; SetUserData is only a debugging aid.
		MR_Track t = m_RowTrack.Get(row);
		if (!t)
			return;

		m_Selected = m_View.Find(t);

		if (m_TrackInfo)
		{
			string year = "";
			if (t.m_Year > 0)
				year = " (" + t.m_Year.ToString() + ")";
			m_TrackInfo.SetText(t.DisplayName() + "\n" + t.m_Album + year + "\n" + t.DurationText() + "   " + t.m_Provider);
		}
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (!w)
			return false;

		string name = w.GetName();

		if (name == "MR_VolumeSlider")
		{
			int vol = Math.Round(m_VolumeSlider.GetCurrent());
			if (m_Host && m_HostType != MR_HostType.HOST_NONE)
				MR_ClientRPC.RequestVolume(m_HostType, m_HostIdLow, m_HostIdHigh, vol);
			UpdateLabels();
			return true;
		}

		if (name == "MR_RadiusSlider")
		{
			UpdateLabels();
			return true;
		}

		if (name == "MR_SearchBox")
		{
			RefreshList();
			return true;
		}

		if (name == "MR_BroadcastCheck")
			return true;

		return super.OnChange(w, x, y, finished);
	}

	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_ESCAPE)
		{
			Close();
			return true;
		}
		return super.OnKeyDown(w, x, y, key);
	}

	// ---------------------------------------------------------------- actions
	protected MR_Track SelectedTrack()
	{
		if (m_Selected < 0 || m_Selected >= m_View.Count())
			return null;
		return m_View.Get(m_Selected);
	}

	void DoPlay()
	{
		// Tapes tab: "play" means "start the album that is in the deck".
		if (m_Tab == 2)
		{
			if (m_AlbumId == string.Empty)
			{
				MR_ClientState.SetMessage("No cassette or disc in the player");
				UpdateLabels();
				return;
			}
			if (!m_Host)
			{
				MR_ClientState.SetMessage("No player available here");
				UpdateLabels();
				return;
			}
			MR_ClientState.SetMessage("Starting album '" + m_AlbumId + "'");
			MR_ClientRPC.RequestPlayCarrier(m_Host, m_HostType, m_HostIdLow, m_HostIdHigh, m_AlbumId);
			UpdateLabels();
			return;
		}

		MR_Track t = SelectedTrack();
		if (!t)
		{
			MR_ClientState.SetMessage("Select a track first");
			UpdateLabels();
			return;
		}

		if (!m_Host)
		{
			MR_ClientState.SetMessage("No radio available here");
			UpdateLabels();
			return;
		}

		MR_ClientState.SetMessage("Requesting: " + t.DisplayName());
		MR_ClientRPC.RequestPlay(m_Host, m_HostType, m_HostIdLow, m_HostIdHigh, t.m_Id, t.m_MediaKind);
		UpdateLabels();
	}

	void DoStop()
	{
		if (!m_Host || m_HostType == MR_HostType.HOST_NONE)
			return;
		MR_ClientRPC.RequestStop(m_HostType, m_HostIdLow, m_HostIdHigh);
	}

	void DoNext()
	{
		if (!m_Host || m_HostType == MR_HostType.HOST_NONE)
			return;
		MR_ClientRPC.RequestNext(m_HostType, m_HostIdLow, m_HostIdHigh);
	}

}
```

### @MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_GameHooks.c

`@MusicRadioCore/Scripts/5_Mission/MusicRadio/MR_GameHooks.c` - 135 строк, 3480 байт.

```cpp
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
```

## 5. Core - интерфейс и локализация

### @MusicRadioCore/gui/layouts/radio_ui.layout

`@MusicRadioCore/gui/layouts/radio_ui.layout` - 273 строк, 5601 байт.

```xml
// =============================================================================
//  MusicRadio - radio_ui.layout
//  Simple, hand editable layout. All sizes are pixels (hexactsize/vexactsize 1)
//  inside a fixed 900x620 panel so the menu looks identical on any monitor.
//
//  Widget names are referenced from MR_RadioMenu.c - keep them in sync.
// =============================================================================

FrameWidgetClass MR_Root {
 size 900 620
 hexactsize 1
 vexactsize 1
 halign center_ref
 valign center_ref
 priority 900
 color 0.05 0.05 0.06 1
 "no focus" 1

 {
  // ------------------------------------------------------------- background
  PanelWidgetClass MR_Background {
   position 0 0
   size 1 1
   hexactsize 0
   vexactsize 0
   color 0.07 0.08 0.09 0.94
  }

  // ------------------------------------------------------------------ header
  PanelWidgetClass MR_Header {
   position 0 0
   size 1 64
   hexactsize 0
   vexactsize 1
   color 0.12 0.14 0.16 1
   {
    TextWidgetClass MR_Title {
     position 20 0
     size 420 64
     hexactsize 1
     vexactsize 1
     text "RADIO / MUSIC"
     font "gui/fonts/sdf_MetronBook24"
     "text halign" left
     "text valign" center
     color 0.92 0.92 0.92 1
    }

    TextWidgetClass MR_NowPlaying {
     position 440 0
     size 400 64
     hexactsize 1
     vexactsize 1
     text "not playing"
     font "gui/fonts/sdf_MetronBook24"
     "text halign" right
     "text valign" center
     color 0.55 0.75 0.95 1
    }

    ButtonWidgetClass MR_CloseButton {
     position 850 12
     size 40 40
     hexactsize 1
     vexactsize 1
     text "X"
     style "Default"
    }
   }
  }

  // ------------------------------------------------------------------- tabs
  PanelWidgetClass MR_Tabs {
   position 0 64
   size 1 44
   hexactsize 0
   vexactsize 1
   color 0.10 0.11 0.13 1
   {
    ButtonWidgetClass MR_TabMusic {
     position 10 6
     size 130 32
     hexactsize 1
     vexactsize 1
     text "MUSIC"
     style "Default"
    }
    ButtonWidgetClass MR_TabAll {
     position 148 6
     size 130 32
     hexactsize 1
     vexactsize 1
     text "ALL TRACKS"
     style "Default"
    }
    ButtonWidgetClass MR_TabTapes {
     position 286 6
     size 130 32
     hexactsize 1
     vexactsize 1
     text "TAPES"
     style "Default"
    }

    EditBoxWidgetClass MR_SearchBox {
     position 440 8
     size 440 30
     hexactsize 1
     vexactsize 1
     text ""
     color 0.2 0.22 0.24 1
    }
   }
  }

  // ------------------------------------------------------------ track list
  PanelWidgetClass MR_ListPanel {
   position 16 118
   size 560 420
   hexactsize 1
   vexactsize 1
   color 0.09 0.10 0.11 1
   {
    ScrollWidgetClass MR_TrackScroll {
     position 0 0
     size 1 1
     hexactsize 0
     vexactsize 0
     "Scrollbar V" 1
     {
      WrapSpacerWidgetClass MR_TrackList {
       position 4 4
       size 1 1
       hexactsize 0
       vexactsize 0
       Padding 2
       Margin 2
       "Size To Content H" 1
      }
     }
    }
   }
  }

  // ---------------------------------------------------------------- controls
  PanelWidgetClass MR_Controls {
   position 588 118
   size 296 420
   hexactsize 1
   vexactsize 1
   color 0.09 0.10 0.11 1
   {
    TextWidgetClass MR_TrackInfo {
     position 12 12
     size 272 90
     hexactsize 1
     vexactsize 1
     text "nothing selected"
     font "gui/fonts/sdf_MetronBook24"
     "text halign" left
     "text valign" top
     wrap 1
     color 0.85 0.85 0.85 1
    }

    TextWidgetClass MR_StatusLabel {
     position 12 110
     size 272 24
     hexactsize 1
     vexactsize 1
     text ""
     font "gui/fonts/sdf_MetronBook16"
     "text halign" left
     "text valign" center
     color 0.95 0.75 0.4 1
    }

    TextWidgetClass MR_VolumeLabel {
     position 12 150
     size 272 20
     hexactsize 1
     vexactsize 1
     text "VOLUME 60%"
     font "gui/fonts/sdf_MetronBook16"
     "text halign" left
     "text valign" center
     color 0.8 0.8 0.8 1
    }

    SliderWidgetClass MR_VolumeSlider {
     position 12 172
     size 272 24
     hexactsize 1
     vexactsize 1
     "fill in" 1
     "listen to input" 1
    }

    TextWidgetClass MR_RadiusLabel {
     position 12 206
     size 272 20
     hexactsize 1
     vexactsize 1
     text "RANGE 40 m"
     font "gui/fonts/sdf_MetronBook16"
     "text halign" left
     "text valign" center
     color 0.8 0.8 0.8 1
    }

    SliderWidgetClass MR_RadiusSlider {
     position 12 228
     size 272 24
     hexactsize 1
     vexactsize 1
     "fill in" 1
     "listen to input" 1
    }

    ButtonWidgetClass MR_PlayButton {
     position 12 272
     size 272 38
     hexactsize 1
     vexactsize 1
     text "PLAY"
     style "Default"
    }

    ButtonWidgetClass MR_StopButton {
     position 12 316
     size 132 34
     hexactsize 1
     vexactsize 1
     text "STOP"
     style "Default"
    }

    ButtonWidgetClass MR_NextButton {
     position 152 316
     size 132 34
     hexactsize 1
     vexactsize 1
     text "NEXT"
     style "Default"
    }

    CheckBoxWidgetClass MR_BroadcastCheck {
     position 12 360
     size 272 28
     hexactsize 1
     vexactsize 1
     text "BROADCAST TO OTHERS"
     color 0.8 0.8 0.8 1
    }
   }
  }

  // ------------------------------------------------------------------ footer
  TextWidgetClass MR_Footer {
   position 16 550
   size 868 50
   hexactsize 1
   vexactsize 1
   text ""
   font "gui/fonts/sdf_MetronBook16"
   "text halign" left
   "text valign" top
   wrap 1
   color 0.6 0.62 0.65 1
  }
 }
}
```

### @MusicRadioCore/gui/layouts/radio_row.layout

`@MusicRadioCore/gui/layouts/radio_row.layout` - 63 строк, 1321 байт.

```xml
// =============================================================================
//  MusicRadio - radio_row.layout
//  One line of the track list. Instantiated by MR_RadioMenu.CreateRow().
//  Sized by the parent WrapSpacer (Size To Content H).
// =============================================================================

FrameWidgetClass MR_Row {
 size 540 30
 hexactsize 1
 vexactsize 1
 color 0.12 0.13 0.15 0.6

 {
  TextWidgetClass MR_RowIndex {
   position 8 0
   size 32 30
   hexactsize 1
   vexactsize 1
   text "1"
   font "gui/fonts/sdf_MetronBook16"
   "text halign" left
   "text valign" center
   color 0.5 0.52 0.55 1
  }

  TextWidgetClass MR_RowTitle {
   position 44 0
   size 330 30
   hexactsize 1
   vexactsize 1
   text "track"
   font "gui/fonts/sdf_MetronBook16"
   "text halign" left
   "text valign" center
   color 0.9 0.9 0.9 1
  }

  TextWidgetClass MR_RowArtist {
   position 380 0
   size 110 30
   hexactsize 1
   vexactsize 1
   text ""
   font "gui/fonts/sdf_MetronBook16"
   "text halign" left
   "text valign" center
   color 0.65 0.66 0.7 1
  }

  TextWidgetClass MR_RowDuration {
   position 494 0
   size 40 30
   hexactsize 1
   vexactsize 1
   text "--:--"
   font "gui/fonts/sdf_MetronBook16"
   "text halign" right
   "text valign" center
   color 0.6 0.62 0.66 1
  }
 }
}
```

### @MusicRadioCore/inputs.xml

`@MusicRadioCore/inputs.xml` - 43 строк, 1374 байт.

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<!--
    MusicRadio - Core
    inputs.xml : three actions, one tab in the settings, no vanilla conflicts.

    Registered from CfgMods -> defs -> inputs (see config.cpp). Nothing in the
    vanilla input files is patched, so two mods using this file never fight.

    DEFAULT KEYS
    ------------
        kRight / kLeft  -> next / previous track   (free in vanilla)
        kB              -> open the player menu    (free in vanilla)

    Deliberately NOT used: kJ (map) and kM (journal). All three reference mods
    - FoXyRadio, OMD_CarRadio and VOS_Radio - shipped "kJ" as their default and
    all three broke the map key for players who installed two of them.
-->
<modded_inputs>
	<inputs>
		<actions>
			<input name="MRNextTrack"     loc="#STR_MR_NextTrack"     />
			<input name="MRPreviousTrack" loc="#STR_MR_PreviousTrack" />
			<input name="MRTogglePlayer"  loc="#STR_MR_TogglePlayer"  />
		</actions>
		<sorting name="MusicRadio" loc="MusicRadio">
			<input name="MRNextTrack"/>
			<input name="MRPreviousTrack"/>
			<input name="MRTogglePlayer"/>
		</sorting>
	</inputs>
	<preset>
		<input name="MRNextTrack">
			<btn name="kRight"/>
		</input>
		<input name="MRPreviousTrack">
			<btn name="kLeft"/>
		</input>
		<input name="MRTogglePlayer">
			<btn name="kB"/>
		</input>
	</preset>
</modded_inputs>
```

### @MusicRadioCore/stringtable.csv

`@MusicRadioCore/stringtable.csv` - 21 строк, 5023 байт.

```text
﻿Language,original,english,czech,german,russian,polish,hungarian,italian,spanish,french,chinese,japanese,portuguese,chinesesimp
STR_MR_MemoryCard,Memory card,Memory card,Paměťová karta,Speicherkarte,Карта памяти,Karta pamięci,,,Tarjeta de memoria,Carte mémoire,,,,
STR_MR_MemoryCard_desc,A memory card for a radio. Insert it before playing.,A memory card for a radio. Insert it before playing.,Paměťová karta pro rádio. Vložte ji před přehráváním.,Speicherkarte für ein Radio. Vor dem Abspielen einsetzen.,Карта памяти для радио. Вставьте её перед воспроизведением.,Karta pamięci do radia. Włóż ją przed odtwarzaniem.,,,Tarjeta de memoria para una radio. Insértala antes de reproducir.,Carte mémoire pour une radio. Insérez-la avant la lecture.,,,,
STR_MR_CassettePlayer,Cassette player,Cassette player,Kazetový přehrávač,Kassettenplayer,Кассетный плеер,Odtwarzacz kasetowy,,,Reproductor de casetes,Lecteur de cassettes,,,,
STR_MR_CassettePlayer_desc,Plays one cassette. Needs a memory card and a 9V battery.,Plays one cassette. Needs a memory card and a 9V battery.,Přehrává jednu kazetu. Potřebuje paměťovou kartu a 9V baterii.,Spielt eine Kassette. Braucht eine Speicherkarte und eine 9V-Batterie.,Играет одну кассету. Нужны карта памяти и батарейка 9V.,Odtwarza jedną kasetę. Wymaga karty pamięci i baterii 9V.,,,Reproduce un casete. Necesita una tarjeta de memoria y una pila de 9V.,Lit une cassette. Nécessite une carte mémoire et une pile 9V.,,,,
STR_MR_CassettePlayerBlack,Cassette player (black),Cassette player (black),Kazetový přehrávač (černý),Kassettenplayer (schwarz),Кассетный плеер (чёрный),Odtwarzacz kasetowy (czarny),,,Reproductor de casetes (negro),Lecteur de cassettes (noir),,,,
STR_MR_MiniDiscPlayer,MiniDisc player,MiniDisc player,MD přehrávač,MD-Player,MD-плеер,Odtwarzacz MD,,,Reproductor de MiniDisc,Lecteur MiniDisc,,,,
STR_MR_MiniDiscPlayer_desc,Plays one MiniDisc. Needs a memory card and a 9V battery.,Plays one MiniDisc. Needs a memory card and a 9V battery.,Přehrává jeden MiniDisc. Potřebuje paměťovou kartu a 9V baterii.,Spielt eine MiniDisc. Braucht eine Speicherkarte und eine 9V-Batterie.,Играет один мини-диск. Нужны карта памяти и батарейка 9V.,Odtwarza jedną MiniDisc. Wymaga karty pamięci i baterii 9V.,,,Reproduce un MiniDisc. Necesita una tarjeta de memoria y una pila de 9V.,Lit un MiniDisc. Nécessite une carte mémoire et une pile 9V.,,,,
STR_MR_CassetteCase,Music cassette,Music cassette,Hudební kazeta,Musikkassette,Музыкальная кассета,Kaseta muzyczna,,,Casete de música,Cassette audio,,,,
STR_MR_CassetteCase_desc,Whole album on one side. Insert into a cassette player.,Whole album on one side. Insert into a cassette player.,Celé album na jedné straně. Vložte do kazetového přehrávače.,Ganzes Album auf einer Seite. In einen Kassettenplayer einlegen.,Весь альбом на одной стороне. Вставьте в кассетный плеер.,Cały album na jednej stronie. Włóż do odtwarzacza kasetowego.,,,Álbum completo en un lado. Insértalo en un reproductor de casetes.,Album complet sur une face. Insérez dans un lecteur de cassettes.,,,,
STR_MR_MiniDisc,MiniDisc,MiniDisc,MiniDisc,MiniDisc,Мини-диск,MiniDisc,,,MiniDisc,MiniDisc,,,,
STR_MR_MiniDisc_desc,One album per disc. Insert into an MD player.,One album per disc. Insert into an MD player.,Jedno album na disk. Vložte do MD přehrávače.,Ein Album pro Disc. In einen MD-Player einlegen.,Один альбом на диск. Вставьте в MD-плеер.,Jeden album na dysk. Włóż do odtwarzacza MD.,,,Un álbum por disco. Insértalo en un reproductor MD.,Un album par disque. Insérez dans un lecteur MD.,,,,
STR_MR_Slot_Cassette,Cassette,Cassette,Kazeta,Kassette,Кассета,Kaseta,,,Casete,Cassette,,,,
STR_MR_Slot_MiniDisc,MiniDisc,MiniDisc,MiniDisc,MiniDisc,Мини-диск,MiniDisc,,,MiniDisc,MiniDisc,,,,
STR_MR_Slot_MemoryCard,Memory card,Memory card,Paměťová karta,Speicherkarte,Карта памяти,Karta pamięci,,,Tarjeta de memoria,Carte mémoire,,,,
STR_MR_NextTrack,Next track,Next track,Další skladba,Nächster Titel,Следующий трек,Następny utwór,,,Pista siguiente,Piste suivante,,,,
STR_MR_PreviousTrack,Previous track,Previous track,Předchozí skladba,Vorheriger Titel,Предыдущий трек,Poprzedni utwór,,,Pista anterior,Piste précédente,,,,
STR_MR_TogglePlayer,Open music player,Open music player,Otevřít přehrávač,Player öffnen,Открыть плеер,Otwórz odtwarzacz,,,Abrir reproductor,Ouvrir le lecteur,,,,
STR_MR_Slot_Dash,Cassette slot,Cassette slot,Czech,German,Слот для кассеты,Polish,,,Spanish,French,,,,
STR_MR_Attach_Music,Music media,Music media,Czech,German,Музыка,Polish,,,Spanish,French,,,,
```

## 6. Server - config.cpp и скрипты

### @MusicRadioServer/config.cpp

`@MusicRadioServer/config.cpp` - 74 строк, 2179 байт.

```cpp
// =============================================================================
//  MusicRadio - Server (mod #1 of 3)
//  config.cpp   -   version 1.0.0
//
//  PBO layout (source folder for AddonBuilder):
//      config.cpp
//      Scripts/4_World/MusicRadio/*.c
//      Scripts/5_Mission/MusicRadio/*.c
//
//  Build: python3 tools/stage_pbo.py  ->  tools\build_pbo.bat
//  PBO prefix must be "MusicRadioServer". NEVER publish this mod.
//
//  INSTALL ON THE SERVER ONLY - clients must not have this mod.
//
//  Why a separate mod at all, if the core already runs server side?
//  ---------------------------------------------------------------
//  * the authoritative rules (who may play what, radii, cooldowns, audit log)
//    never reach a client machine, so a cheater cannot read or patch them;
//  * a server admin can update the rules without asking 60 players to
//    re-download anything;
//  * the client install stays small: only the core (logic) + the media pack.
//
//  This mod depends on the core mod but never the other way round.
// =============================================================================

class CfgPatches
{
	class MusicRadioServer
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = { "DZ_Data", "DZ_Sounds_Effects", "MusicRadioCore" };
	};
};

class CfgMods
{
	class MusicRadioServer
	{
		dir = "MusicRadioServer";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "MusicRadio - Server";
		credits = "MusicRadio project";
		author = "MusicRadio project";
		authorID = "0";
		version = "1.0.0";
		extra = 0;
		type = "mod";
		dependencies[] = { "World", "Mission" };

		class defs
		{
			// The engine loads world/mission modules on clients too if the mod is
			// present there - that is exactly why this mod is installed on the
			// server only: serverScriptModule is not a thing, so we rely on the
			// mod not being in the client's mod list.
			class worldScriptModule
			{
				value = "";
				files[] = { "MusicRadioServer/Scripts/4_World" };
			};
			class missionScriptModule
			{
				value = "";
				files[] = { "MusicRadioServer/Scripts/5_Mission" };
			};
		};
	};
};
```

### @MusicRadioServer/Scripts/4_World/MusicRadio/MR_ServerEngine.c

`@MusicRadioServer/Scripts/4_World/MusicRadio/MR_ServerEngine.c` - 1060 строк, 30886 байт.

```cpp
// ============================================================================
//  MusicRadio - Server
//  MR_ServerEngine.c : the authority. Everything a client asks for is checked
//  here before any audio is allowed to play anywhere.
//  Layer: 4_World (server process only)
//
//  Responsibilities
//  ----------------
//   1. load the config + media library
//   2. validate every play request (permissions, host, distance, cooldown)
//   3. keep a session table (who is playing what, where, how loud)
//   4. fan out PLAY / STOP / VOLUME to the clients that should hear it
//   5. advance to the next track when the client reports the media ended
//   6. audit log for moderation
// ============================================================================

class MR_ServerEngine
{
	protected static ref MR_Settings m_Settings;
	protected static ref MR_Library  m_Library;
	protected static ref map<string, ref MR_Session> m_Sessions;
	protected static ref map<string, int> m_LastRequest;   // playerId -> ms
	protected static bool m_Initialised;

	// ------------------------------------------------------------------ setup
	static void Init()
	{
		if (m_Initialised)
			return;

		if (!m_Sessions)
			m_Sessions = new map<string, ref MR_Session>();
		if (!m_LastRequest)
			m_LastRequest = new map<string, int>();

		m_Settings = MR_Config.LoadServer();
		MR_ConfigRuntime.Set(m_Settings);
		MR_Config.DumpSummary(m_Settings);

		m_Library = new MR_Library();
		m_Library.LoadAll();

		MR_AuditLog.Open(MR.DIR_MISSION + MR.LOG_FILE);

		m_Initialised = true;

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MR_ServerEngine.Tick, MR.SERVER_TICK_MS, true);
		MR.Log("Server engine ready (protocol " + MR.PROTOCOL.ToString() + ")");
	}

	static void Shutdown()
	{
		if (!m_Initialised)
			return;

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(MR_ServerEngine.Tick);
		StopAll(MR_StopReason.STOP_USER, false);
		MR_AuditLog.Close();
		m_Initialised = false;
	}

	static MR_Settings Settings()
	{
		if (!m_Settings)
			m_Settings = new MR_Settings();
		return m_Settings;
	}

	static MR_Library Library()
	{
		if (!m_Library)
		{
			m_Library = new MR_Library();
			m_Library.LoadAll();
		}
		return m_Library;
	}

	// ---------------------------------------------------------------- clients
	// ------------------------------------------------------------------ send
	// RPC идут на ОБЪЕКТ ИГРОКА, а не на объект игры.
	//
	// Причина - в клиенте: принять пакет можно только там, где есть OnRPC.
	// У объекта игры (DayZGame) наш слой его не видит, у Mission OnRPC нет
	// вообще (логи 01:12 и 01:23). А PlayerBase.OnRPC есть и работает -
	// именно так сервер получает наши запросы.
	//
	// Одно место на все отправки: 8 вызовов rpc.Send(null, ...) в этом файле
	// иначе легко забыть и оставить один "в никуда".
	static void SendTo(PlayerBase pb, int rpcType, string payload)
	{
		if (!pb || !pb.GetIdentity())
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(payload);
		rpc.Send(pb, rpcType, true, pb.GetIdentity());
	}

	static void SendToIdent(PlayerBase pb, PlayerIdentity to, int rpcType, string payload)
	{
		if (!pb || !to)
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(payload);
		rpc.Send(pb, rpcType, true, to);
	}

	static void SendConfig(PlayerIdentity to)
	{
		if (!to)
			return;

		string text;
		Settings().WriteJson(text);
		SendToIdent(PlayerBase.Cast(MR_ServerUtil.FindPlayer(to)), to, MR.RPC_CONFIG, text);
	}

	static void SendLibrary(PlayerIdentity to)
	{
		if (!to)
			return;

		// The manifest also carries the server's unix time: clients use it as the
		// broadcast clock, so every player hears the same second of a station.
		SendToIdent(PlayerBase.Cast(MR_ServerUtil.FindPlayer(to)), to, MR.RPC_LIBRARY, Library().SerializeManifest());
	}

	// Called on join / reconnect: give the client the config, the catalogue and
	// whatever is already playing close to it.
	static void OnPlayerJoined(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;

		SendConfig(player.GetIdentity());
		SendLibrary(player.GetIdentity());
		SyncActiveSessions(player);
	}

	static void OnPlayerDisconnected(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;

		string pid = player.GetIdentity().GetId();

		// Sessions owned by the leaving player keep playing only if they belong
		// to a vehicle (the car is still there); otherwise they die with them.
		TStringArray toStop = new TStringArray();
		foreach (string id, MR_Session s : m_Sessions)
		{
			if (s.m_OwnerId == pid && s.m_HostType == MR_HostType.HOST_ITEM)
				toStop.Insert(id);
		}
		foreach (string sid : toStop)
			StopSession(sid, MR_StopReason.STOP_HOST_GONE);
	}

	// ---------------------------------------------------------------- requests
	static void HandleRequest(PlayerBase player, string payload)
	{
		if (!player || !player.GetIdentity())
			return;

		MR_Request req = MR_Request.Parse(payload);
		if (req.m_Action == MR_Action.ACT_UNKNOWN)
			return;

		if (Settings().m_LogRequests)
			MR.Log("request from " + player.GetIdentity().GetName() + ": action=" + ((int)req.m_Action).ToString() + " track=" + req.m_TrackId);

		switch (req.m_Action)
		{
			case MR_Action.ACT_REQUEST_LIB:
			{
				SendLibrary(player.GetIdentity());
				return;
			}
			case MR_Action.ACT_PLAY_TRACK:
			{
				StartPlay(player, req);
				return;
			}
			case MR_Action.ACT_PLAY_ALBUM:
			{
				StartPlayAlbum(player, req);
				return;
			}
			case MR_Action.ACT_STOP:
			{
				StopByHost(req.m_HostType, req.m_HostId, req.m_HostIdHigh, MR_StopReason.STOP_USER);
				return;
			}
			case MR_Action.ACT_VOLUME:
			{
				ChangeVolume(player, req);
				return;
			}
			case MR_Action.ACT_NEXT:
			case MR_Action.ACT_PREV:
			{
				NextTrack(player, req, req.m_Action == MR_Action.ACT_NEXT);
				return;
			}
			case MR_Action.ACT_NOWPLAYING:
			{
				SendNowPlaying(player, req.m_TrackId);
				return;
			}
			case MR_Action.ACT_SEEK:
			{
				// The deck rewound / fast-forwarded. The server owns the
				// position on the carrier, so it takes the client's value,
				// clamps it and stores it - nothing is broadcast.
				MR_DeckState.SeekTo(req.m_HostType, req.m_HostId, req.m_HostIdHigh, req.m_Volume);
				return;
			}
		}
	}

	// --------------------------------------------------- play: one whole album
	// A cassette / MiniDisc is an album, so "play" means "start the album and
	// remember its id in the session". Autoplay then stays inside that album
	// (see NextTrack) - a tape never jumps to a different record.
	static void StartPlayAlbum(PlayerBase player, MR_Request req)
	{
		PlayerIdentity ident = player.GetIdentity();

		if (!Settings().m_AllowLocalTracks)
		{
			Reply(ident, false, "Music is disabled on this server");
			return;
		}

		Object hostObj = MR_Registry.Find(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		if (!hostObj)
		{
			Reply(ident, false, "Media player not found in the world");
			return;
		}

		string reason;
		if (!ValidateHost(player, hostObj, req.m_HostType, reason))
		{
			Reply(ident, false, reason);
			return;
		}

		// What is actually inserted? Never trust the id the client sent: the
		// server reads the attachment and derives the album from the class name.
		string albumId = string.Empty;

		// Car with a docked deck: the album comes from the deck on the dash.
		CarScript dockCar = CarScript.Cast(hostObj);
		if (dockCar)
			albumId = MR_MediaItems.AlbumInVehicle(dockCar);
		MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(hostObj);
		if (deck)
		{
			EntityAI tape = deck.MR_GetInsertedCassette();
			if (tape)
				albumId = MR_MediaItems.AlbumOf(tape.GetType());
		}
		MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(hostObj);
		if (md)
		{
			EntityAI disc = md.MR_GetInsertedDisc();
			if (disc)
				albumId = MR_MediaItems.AlbumOf(disc.GetType());
		}

		// Рация - тоже устройство: играет кассету/диск, вставленные в неё.
		PersonalRadio handheld = PersonalRadio.Cast(hostObj);
		if (handheld)
			albumId = handheld.MR_CurrentAlbum();

		if (albumId == string.Empty)
		{
			Reply(ident, false, "Nothing is inserted in the player");
			return;
		}
		if (albumId != req.m_TrackId)
		{
			MR.LogWarn("PLAY_ALBUM: client asked for '" + req.m_TrackId + "' but the deck holds '" + albumId + "' - using the deck");
		}

		array<ref MR_Track> album = Library().ByAlbum(albumId);
		if (album.Count() == 0)
		{
			Reply(ident, false, "Album '" + albumId + "' is not in the media pack");
			return;
		}

		// Does the tape remember where it stopped? Ask the CARRIER, not the
		// client: the client's idea of "the album is at 2:17" is only a hint.
		string resumeTrack = string.Empty;
		int resumeOffset = 0;
		int resumeNo = 0;
		EntityAI carrier = MR_DeckState.FindCarrier(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		MR_DeckState.ResumePoint(carrier, albumId, resumeTrack, resumeOffset);

		// The remembered track must still exist in this album (media pack may
		// have been rebuilt since the tape was last played).
		MR_Track startTrack = null;
		for (int ai = 0; ai < album.Count(); ai++)
		{
			MR_Track candidate = album.Get(ai);
			if (!candidate)
				continue;
			if (startTrack == null)
				startTrack = candidate;          // fallback: first track
			if (candidate.m_Id == resumeTrack)
			{
				startTrack = candidate;
				break;
			}
		}

		if (startTrack == null)
		{
			Reply(ident, false, "Album '" + albumId + "' has no playable tracks");
			return;
		}

		// Never resume past the end: a tape that ran on starts the next track.
		// ВАЖНО: MR_CarrierState.IsFinished(MR_CarrierMemory, int) ждёт объект
		// памяти, а не секунды. Вызов с int даёт сразу три ошибки компилятора:
		//   Only NULL assignment is allowed
		//   Cannot convert 'int' to 'MR_CarrierMemory' for argument '0'
		// Секунды сравниваем здесь.
		if (resumeOffset > 0 && startTrack.m_Duration > 0 && resumeOffset >= startTrack.m_Duration)
			resumeOffset = 0;

		// Start the remembered track through the normal path so all
		// permission / distance / cooldown rules apply unchanged.
		MR_Request play = new MR_Request();
		play.m_HostType  = req.m_HostType;
		play.m_HostId    = req.m_HostId;
		play.m_HostIdHigh = req.m_HostIdHigh;
		play.m_TrackId   = startTrack.m_Id;
		play.m_Volume    = req.m_Volume;
		play.m_Radius    = req.m_Radius;
		play.m_Broadcast = req.m_Broadcast;

		// StartPlay resets the offset to 0 by design (a fresh play button),
		// so the resume offset is carried on the request itself.
		play.m_StartOffset = resumeOffset;

		StartPlay(player, play);

		// Tag the session with the album so NextTrack keeps it inside the tape.
		string key = HostKey(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		MR_Session s = m_Sessions.Get(key);
		if (s)
		{
			s.m_AlbumId = albumId;
			BroadcastNowPlaying(s, album.Get(0));
			Reply(ident, true, "Playing album '" + albumId + "' (" + album.Count().ToString() + " tracks)");
		}
	}

	// Next (or previous) track of the same album, wrapping at the end so a tape
	// loops like it physically would.
	static MR_Track NextInAlbum(string albumId, string currentId, bool forward)
	{
		array<ref MR_Track> album = Library().ByAlbum(albumId);
		if (album.Count() == 0)
			return null;

		int index = -1;
		for (int i = 0; i < album.Count(); i++)
		{
			if (album.Get(i).m_Id == currentId)
			{
				index = i;
				break;
			}
		}

		if (index < 0)
			return album.Get(0);

		int next = index - 1;
		if (forward)
			next = index + 1;
		if (next >= album.Count())
			next = 0;
		if (next < 0)
			next = album.Count() - 1;

		return album.Get(next);
	}

	// ------------------------------------------------------------ play: checks
	static void StartPlay(PlayerBase player, MR_Request req)
	{
		PlayerIdentity ident = player.GetIdentity();

		// 1) feature flags
		if (!Settings().m_AllowLocalTracks)
		{
			Reply(ident, false, "Music is disabled on this server");
			return;
		}

		if (req.m_HostType == MR_HostType.HOST_VEHICLE && !Settings().m_EnableVehicleRadio)
		{
			Reply(ident, false, "Vehicle radio is disabled");
			return;
		}
		if (req.m_HostType == MR_HostType.HOST_ITEM && !Settings().m_EnableHandheldRadio)
		{
			Reply(ident, false, "Handheld radio is disabled");
			return;
		}

		// 2) cooldown
		string pid = ident.GetId();
		int now = GetGame().GetTime();
		int last = 0;
		if (m_LastRequest.Contains(pid))
			last = m_LastRequest.Get(pid);
		if (Settings().m_CooldownSeconds > 0 && now - last < Settings().m_CooldownSeconds * 1000)
		{
			Reply(ident, false, "Slow down a moment");
			return;
		}
		m_LastRequest.Set(pid, now);

		// 3) the track exists and is allowed
		MR_Track track = Library().GetById(req.m_TrackId);
		if (!track)
		{
			Reply(ident, false, "Unknown track: " + req.m_TrackId);
			return;
		}
		if (!track.m_Enabled)
		{
			Reply(ident, false, "That entry is disabled");
			return;
		}
		// 4) the host object
		Object hostObj = MR_Registry.Find(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		if (!hostObj)
		{
			Reply(ident, false, "Radio not found in the world");
			return;
		}

		if (vector.Distance(hostObj.GetPosition(), player.GetPosition()) > 6.0)
		{
			Reply(ident, false, "You are too far from that radio");
			return;
		}

		// 5) host specific rules
		string blockReason;
		if (!ValidateHost(player, hostObj, req.m_HostType, blockReason))
		{
			Reply(ident, false, blockReason);
			return;
		}

		// 6) one session per host: replace whatever was there
		string hostKey = HostKey(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		if (m_Sessions.Contains(hostKey))
			StopSession(hostKey, MR_StopReason.STOP_USER);

		// 7) build the session
		MR_Session s = new MR_Session();
		s.m_Id = hostKey;
		s.m_HostType = req.m_HostType;
		s.m_HostId = req.m_HostId;
		s.m_HostIdHigh = req.m_HostIdHigh;
		s.m_OwnerId = pid;
		s.m_OwnerName = ident.GetName();
		s.m_TrackId = track.m_Id;
		s.m_Provider = track.m_Provider;
		s.m_Title = track.DisplayName();
		s.m_StartTimeMs = now;
		// Resume: a request may carry an offset (a tape that was stopped at
		// 2:17 keeps going from 2:17 instead of restarting the album).
		s.m_StartOffset = MR_CarrierState.ClampPosition(req.m_StartOffset, track.m_Duration);
		s.m_Duration = track.m_Duration;
		s.m_Volume = MR.ClampInt(req.m_Volume, 0, Settings().m_MaxVolume);
		s.m_Radius = MR.ClampFloat(req.m_Radius, MR.RADIUS_MIN, Settings().m_MaxRadius);
		s.m_Loop = track.m_Loop;
		s.m_Broadcast = req.m_Broadcast && Settings().m_AllowBroadcast;

		m_Sessions.Set(hostKey, s);

		// 7b) the tape starts remembering. Only for hosts that hold a physical
		// carrier (cassette / MiniDisc in a deck or on a car dash) - a plain
		// handheld radio has nothing to write to and costs one lookup.
		MR_DeckState.TrackStart(s.m_HostType, s.m_HostId, s.m_HostIdHigh, s.m_TrackId, 0, s.m_StartOffset);

		// 8) tell the clients
		DispatchPlay(s, track, hostObj);
		Reply(ident, true, "Playing: " + s.m_Title);

		MR_AuditLog.Line("PLAY " + ident.GetName() + " (" + pid + ") host=" + hostKey + " track=" + track.m_Id + " vol=" + s.m_Volume.ToString() + " r=" + s.m_Radius.ToString());
	}

	static bool ValidateHost(PlayerBase player, Object hostObj, MR_HostType type, out string reason)
	{
		reason = string.Empty;

		if (type == MR_HostType.HOST_ITEM)
		{
			PersonalRadio radio = PersonalRadio.Cast(hostObj);
			if (!radio)
			{
				reason = "That is not a radio";
				return false;
			}
			if (radio.IsRuined())
			{
				reason = "The radio is ruined";
				return false;
			}
			if (!MR_VehicleRadio.HasPower(radio))
			{
				reason = "The radio has no working battery";
				return false;
			}

			// Единый контракт 20261174: рация играет только то, что в неё
			// вставили, и только с картой памяти - как дека.
			string notReady;
			if (Settings().m_RadioNeedsCarrier && !radio.MR_IsReady(notReady))
			{
				reason = "The radio is not ready: " + notReady;
				return false;
			}

			// The player must actually hold / carry it.
			if (!player.GetInventory().HasEntityInInventory(radio) && player.GetHumanInventory().GetEntityInHands() != radio)
			{
				reason = "You must hold the radio";
				return false;
			}
			return true;
		}

		if (type == MR_HostType.HOST_VEHICLE)
		{
			CarScript car = CarScript.Cast(hostObj);
			if (!car)
			{
				reason = "That is not a vehicle";
				return false;
			}
			if (car.IsRuined())
			{
				reason = "The vehicle is ruined";
				return false;
			}
			if (player.GetParent() != car)
			{
				reason = "You must sit in the vehicle";
				return false;
			}

			// Плеер ИЛИ кассета/диск в слоте машины заменяют правило
			// "рация в багажнике". Проверяем сам факт вложения, а не альбом:
			// положили кассету - источник уже есть, дальше решает сервер.
			bool hasDocked = (MR_MediaItems.DockedInVehicle(car) != null);
			if (!hasDocked && Settings().m_RequireRadioItemInCargo)
			{
				// Имя "radio" в этой функции уже занято PersonalRadio (ветка
				// HOST_ITEM выше). Объявлять здесь ещё одно "EntityAI radio"
				// нельзя: при этом компилятор пишет
				//   Incompatible parameter 'radio'   (лог 20:47, строка 527)
				EntityAI inCargo;
				if (!MR_VehicleRadio.HasRadioInCargo(car, inCargo))
				{
					reason = "No radio in the vehicle cargo and no music player on the dash";
					return false;
				}
				if (!MR_VehicleRadio.HasPower(inCargo))
				{
					reason = "The radio's battery is empty";
					return false;
				}
			}
			return true;
		}

		if (type == MR_HostType.HOST_PLAYER)
		{
			// Cassette deck / MD player: the physical prerequisites live in the
			// item's attachments, so they are checked here and not in the client.
			EntityAI player_item = EntityAI.Cast(hostObj);
			if (!player_item)
			{
				reason = "That is not a media player";
				return false;
			}
			if (!player.GetInventory().HasEntityInInventory(player_item) && player.GetHumanInventory().GetEntityInHands() != player_item)
			{
				reason = "You must hold the player";
				return false;
			}

			string why;
			MR_CassettePlayer_Base deck = MR_CassettePlayer_Base.Cast(player_item);
			if (deck)
			{
				if (!deck.MR_IsReady(why))
				{
					reason = "Cassette deck: " + why;
					return false;
				}
				return true;
			}

			MR_MiniDiscPlayer_Base md = MR_MiniDiscPlayer_Base.Cast(player_item);
			if (md)
			{
				if (!md.MR_IsReady(why))
				{
					reason = "MD player: " + why;
					return false;
				}
				return true;
			}

			reason = "Unsupported media player";
			return false;
		}

		reason = "Unsupported radio type";
		return false;
	}

	// ------------------------------------------------------------- play: fanout
	static void DispatchPlay(MR_Session s, MR_Track track, Object hostObj)
	{
		MR_PlayRequest req = new MR_PlayRequest();
		req.m_HostType = s.m_HostType;
		req.m_HostId = s.m_HostId;
		req.m_HostIdHigh = s.m_HostIdHigh;
		MR_Locator loc = new MR_Locator();
		if (loc)
		{
			loc.m_Provider = s.m_Provider;
			loc.m_TrackId = s.m_TrackId;
			loc.m_Volume = s.m_Volume;
			loc.m_Seed = Math.RandomInt(0, 100000);
		}
		req.m_Locator = loc;
		// Without this every client started at 0:00 while the server believed
		// the session was at 2:17 - the deck and the server disagreed.
		req.m_StartOffset = s.m_StartOffset;
		req.m_Radius = s.m_Radius;
		req.m_Volume = s.m_Volume;
		req.m_Loop = s.m_Loop;
		req.m_Broadcast = s.m_Broadcast;

		string payload = MR_Net.EncodePlay(req);

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;

			if (!s.m_Broadcast)
			{
				// Private listening: only the owner hears it.
				if (pb.GetIdentity().GetId() != s.m_OwnerId)
					continue;
			}
			else
			{
				// Otherwise everyone inside the radio radius hears it.
				float dist = vector.Distance(pb.GetPosition(), hostObj.GetPosition());
				if (dist > s.m_Radius)
					continue;
			}

			SendTo(pb, MR.RPC_PLAY, payload);
		}

		BroadcastNowPlaying(s, track);
	}

	static void BroadcastNowPlaying(MR_Session s, MR_Track track)
	{
		string payload = MR_Net.EncodeNowPlaying(track.m_Title, track.m_Artist, track.m_Provider);

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;

			SendTo(pb, MR.RPC_NOWPLAYING, payload);
		}
	}

	// ------------------------------------------------------------------ volume
	static void ChangeVolume(PlayerBase player, MR_Request req)
	{
		string key = HostKey(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		MR_Session s = m_Sessions.Get(key);
		if (!s)
			return;

		// Only the owner may change the volume of a broadcast session; a private
		// session can be adjusted by whoever hears it.
		bool owner = (player.GetIdentity().GetId() == s.m_OwnerId);
		if (s.m_Broadcast && !owner && !IsAdmin(player.GetIdentity()))
		{
			Reply(player.GetIdentity(), false, "Only the person who started it can change the volume");
			return;
		}

		s.m_Volume = MR.ClampInt(req.m_Volume, 0, Settings().m_MaxVolume);

		string payload = MR_Net.EncodeVolume(s.m_HostType, s.m_HostId, s.m_HostIdHigh, s.m_Volume);

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;
			if (!s.m_Broadcast && pb.GetIdentity().GetId() != s.m_OwnerId)
				continue;

			SendTo(pb, MR.RPC_VOLUME, payload);
		}
	}

	// -------------------------------------------------------------- next / prev
	static void NextTrack(PlayerBase player, MR_Request req, bool forward)
	{
		string key = HostKey(req.m_HostType, req.m_HostId, req.m_HostIdHigh);
		MR_Session s = m_Sessions.Get(key);
		if (!s)
			return;

		Object hostObj = MR_Registry.Find(s.m_HostType, s.m_HostId, s.m_HostIdHigh);
		if (!hostObj)
		{
			StopSession(key, MR_StopReason.STOP_HOST_GONE);
			return;
		}

		// Manual NEXT/PREV always works. Only the automatic advance (the client
		// reports "track ended") obeys the server switch: with autoplayNext
		// disabled the radio goes quiet instead of continuing the playlist.
		if (!forward && !Settings().m_AllowRepeat)
		{
			StopSession(key, MR_StopReason.STOP_ENDED);
			return;
		}

		MR_Track current = Library().GetById(s.m_TrackId);
		string kind = "music";
		if (current)
			kind = current.m_Kind;

		// A cassette / disc session stays inside its album, in index order;
		// everything else picks a random track of the same kind.
		MR_Track next = null;
		if (s.m_AlbumId != string.Empty)
		{
			next = NextInAlbum(s.m_AlbumId, s.m_TrackId, forward);
		}
		else
		{
			next = Library().RandomTrack(kind, s.m_TrackId);
		}
		if (!next)
		{
			StopSession(key, MR_StopReason.STOP_ENDED);
			Reply(player.GetIdentity(), true, "End of the playlist");
			return;
		}

		// Reuse StartPlay via a synthetic request so every permission check
		// applies to auto-advance as well.
		MR_Request play = new MR_Request();
		play.m_Action = MR_Action.ACT_PLAY_TRACK;
		play.m_HostType = s.m_HostType;
		play.m_HostId = s.m_HostId;
		play.m_HostIdHigh = s.m_HostIdHigh;
		play.m_TrackId = next.m_Id;
		play.m_Volume = s.m_Volume;
		play.m_Radius = s.m_Radius;
		play.m_Broadcast = s.m_Broadcast;

		// Auto advance must not be blocked by the player cooldown.
		m_LastRequest.Set(player.GetIdentity().GetId(), 0);
		StartPlay(player, play);
	}

	static void SendNowPlaying(PlayerBase player, string trackId)
	{
		MR_Track t = Library().GetById(trackId);
		if (!t)
			return;

		string payload = MR_Net.EncodeNowPlaying(t.m_Title, t.m_Artist, t.m_Provider);
			SendTo(player, MR.RPC_NOWPLAYING, payload);
	}

	// -------------------------------------------------------------------- stop
	static void StopByHost(MR_HostType type, int idLow, int idHigh, MR_StopReason reason)
	{
		string key = HostKey(type, idLow, idHigh);
		if (!m_Sessions.Contains(key))
			return;
		StopSession(key, reason);
	}

	static void StopSession(string key, MR_StopReason reason)
	{
		MR_Session s = m_Sessions.Get(key);
		if (!s)
			return;

		string payload = MR_Net.EncodeStop(s.m_HostType, s.m_HostId, s.m_HostIdHigh, reason);

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;
			if (!s.m_Broadcast && pb.GetIdentity().GetId() != s.m_OwnerId)
				continue;

			SendTo(pb, MR.RPC_STOP, payload);
		}

		// Final position on the tape BEFORE the session is forgotten: this is
		// what makes "stop, pull the tape out, resume later" work.
		MR_DeckState.TrackStop(s.m_HostType, s.m_HostId, s.m_HostIdHigh);

		m_Sessions.Remove(key);

		if (reason != MR_StopReason.STOP_USER)
			MR_AuditLog.Line("STOP host=" + key + " reason=" + ((int)reason).ToString());
	}

	static void StopAll(MR_StopReason reason, bool silent = true)
	{
		TStringArray keys = new TStringArray();
		foreach (string key, MR_Session s : m_Sessions)
			keys.Insert(key);

		foreach (string k : keys)
		{
			if (silent)
				m_Sessions.Remove(k);
			else
				StopSession(k, reason);
		}
	}

	// --------------------------------------------------------------- watchdog
	// Runs once a second. Cheap checks only: the host still exists, the radio is
	// still powered, and a vehicle session does not outlive its car.
	static void Tick()
	{
		if (!m_Sessions)
			return;

		MR_Registry.Prune();

		// Cassette / disc position -> carrier. Runs on the server only and is
		// the single writer of a tape's memory (see MR_DeckState).
		MR_DeckState.Tick();

		TStringArray dead = new TStringArray();

		foreach (string key, MR_Session s : m_Sessions)
		{
			Object hostObj = MR_Registry.Find(s.m_HostType, s.m_HostId, s.m_HostIdHigh);
			if (!hostObj)
			{
				dead.Insert(key);
				continue;
			}

			if (s.m_HostType == MR_HostType.HOST_ITEM)
			{
				PersonalRadio radio = PersonalRadio.Cast(hostObj);
				if (!radio || radio.IsRuined() || !MR_VehicleRadio.HasPower(radio))
				{
					dead.Insert(key);
					continue;
				}
				// Вынули кассету или карту на ходу - музыка должна смолкнуть,
				// а не продолжать играть из пустой рации.
				string gone;
				if (Settings().m_RadioNeedsCarrier && !radio.MR_IsReady(gone))
				{
					dead.Insert(key);
					continue;
				}
			}

			if (s.m_HostType == MR_HostType.HOST_VEHICLE)
			{
				CarScript car = CarScript.Cast(hostObj);
				if (!car || car.IsRuined())
				{
					dead.Insert(key);
					continue;
				}
				if (Settings().m_RequireRadioItemInCargo)
				{
					EntityAI item;
					if (!MR_VehicleRadio.HasRadioInCargo(car, item) || !MR_VehicleRadio.HasPower(item))
					{
						dead.Insert(key);
						continue;
					}
				}
			}

			// Finite tracks are ended by the client (it owns the audio clock);
			// this is the safety net for when every listener is gone.
			if (!s.m_Loop && s.m_Duration > 0)
			{
				if (s.CurrentOffset() > s.m_Duration + 5)
					dead.Insert(key);
			}
		}

		foreach (string k : dead)
			StopSession(k, MR_StopReason.STOP_HOST_GONE);
	}

	// Late joiner: replay the sessions that should currently be audible.
	static void SyncActiveSessions(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;

		foreach (string key, MR_Session s : m_Sessions)
		{
			Object hostObj = MR_Registry.Find(s.m_HostType, s.m_HostId, s.m_HostIdHigh);
			if (!hostObj)
				continue;

			if (s.m_Broadcast && vector.Distance(player.GetPosition(), hostObj.GetPosition()) > s.m_Radius)
				continue;
			if (!s.m_Broadcast && player.GetIdentity().GetId() != s.m_OwnerId)
				continue;

			MR_Track track = Library().GetById(s.m_TrackId);
			if (!track)
				continue;

			MR_PlayRequest req = new MR_PlayRequest();
			req.m_HostType = s.m_HostType;
			req.m_HostId = s.m_HostId;
			req.m_HostIdHigh = s.m_HostIdHigh;
			MR_Locator joinLoc = new MR_Locator();
			if (joinLoc)
			{
				joinLoc.m_Provider = s.m_Provider;
				joinLoc.m_TrackId = s.m_TrackId;
			}
			req.m_Locator = joinLoc;
			req.m_StartOffset = s.CurrentOffset();   // join mid-song, synced
			req.m_Radius = s.m_Radius;
			req.m_Volume = s.m_Volume;
			req.m_Loop = s.m_Loop;
			req.m_Broadcast = s.m_Broadcast;

			SendTo(player, MR.RPC_PLAY, MR_Net.EncodePlay(req));
		}
	}

	// ----------------------------------------------------------------- helpers
	static string HostKey(MR_HostType type, int idLow, int idHigh)
	{
		return ((int)type).ToString() + ":" + idLow.ToString() + ":" + idHigh.ToString();
	}

	static void Reply(PlayerIdentity to, bool ok, string message)
	{
		if (!to)
			return;

		SendToIdent(PlayerBase.Cast(MR_ServerUtil.FindPlayer(to)), to, MR.RPC_REPLY, MR_Net.EncodeReply(ok, message));
	}

	static bool IsAdmin(PlayerIdentity ident)
	{
		if (!ident)
			return false;

		// DayZ exposes no portable "is admin" for script; servers that use a
		// permission mod can extend this one function. Default: no.
		return false;
	}

	static PlayerBase FindPlayerByIdentity(PlayerIdentity ident)
	{
		return MR_ServerUtil.FindPlayer(ident);
	}

	static int ActiveSessionCount()
	{
		if (!m_Sessions)
			return 0;
		return m_Sessions.Count();
	}
}

// ---------------------------------------------------------------------------
//  Lookup helper used by the RPC entry point.
//  (Kept in the same file so the engine stays self contained.)
// ---------------------------------------------------------------------------
class MR_ServerUtil
{
	// GetId() is not exposed on every build; UID() always is.
	static string IdOf(PlayerIdentity ident)
	{
		if (!ident)
			return string.Empty;
		return ident.GetId();
	}

	static PlayerBase FindPlayer(PlayerIdentity ident)
	{
		if (!ident)
			return null;

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity())
				continue;
			if (pb.GetIdentity().GetId() == MR_ServerUtil.IdOf(ident))
				return pb;
		}
		return null;
	}
}
```

### @MusicRadioServer/Scripts/4_World/MusicRadio/MR_ServerHooks.c

`@MusicRadioServer/Scripts/4_World/MusicRadio/MR_ServerHooks.c` - 106 строк, 2763 байт.

```cpp
// ============================================================================
//  MusicRadio - Server
//  MR_ServerHooks.c : server-side reactions to world events.
//  Layer: 4_World
// ============================================================================

// A ruined car must not keep singing.
modded class CarScript
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (!GetGame().IsServer())
			return;

		int idLow; int idHigh;
		MR.NetId(this, idLow, idHigh);
		MR_ServerEngine.StopByHost(MR_HostType.HOST_VEHICLE, idLow, idHigh, MR_StopReason.STOP_RUINED);
	}

	// Some builds destroy vehicles through EEHitBy / deletion instead.
	override void EEDelete(EntityAI parent)
	{
		if (GetGame().IsServer())
			int idLow; int idHigh;
			MR.NetId(this, idLow, idHigh);
			MR_ServerEngine.StopByHost(MR_HostType.HOST_VEHICLE, idLow, idHigh, MR_StopReason.STOP_HOST_GONE);

		super.EEDelete(parent);
	}
}

// A destroyed handheld radio stops the music immediately.
modded class PersonalRadio
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (!GetGame().IsServer())
			return;

		int idLow; int idHigh;
		MR.NetId(this, idLow, idHigh);
		MR_ServerEngine.StopByHost(MR_HostType.HOST_ITEM, idLow, idHigh, MR_StopReason.STOP_RUINED);
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame().IsServer())
			int idLow; int idHigh;
			MR.NetId(this, idLow, idHigh);
			MR_ServerEngine.StopByHost(MR_HostType.HOST_ITEM, idLow, idHigh, MR_StopReason.STOP_HOST_GONE);

		super.EEDelete(parent);
	}
}

// ---------------------------------------------------------------------------
//  RPC entry point.
//  Clients send their requests with target = their own player object, so the
//  packet lands exactly here and nowhere else.
// ---------------------------------------------------------------------------
modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (!GetGame().IsServer())
			return;
		if (rpc_type != MR.RPC_REQUEST)
			return;

		// Never trust the "sender" field: resolve the player from the identity
		// that actually delivered the packet.
		if (!sender)
			return;

		string payload;
		if (!ctx.Read(payload))
			return;

		PlayerBase player = PlayerBase.Cast(MR_ServerUtil.FindPlayer(sender));
		if (!player)
		{
			MR.LogWarn("Request from unknown identity: " + sender.GetName());
			return;
		}

		MR_ServerEngine.HandleRequest(player, payload);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (!GetGame().IsServer())
			return;

		// The radio in their hands is gone with the body; the session is dropped
		// by the watchdog in MR_ServerEngine.Tick().
	}
}
```

### @MusicRadioServer/Scripts/5_Mission/MusicRadio/MR_ServerMission.c

`@MusicRadioServer/Scripts/5_Mission/MusicRadio/MR_ServerMission.c` - 75 строк, 2346 байт.

```cpp
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
```

## 7. DB - config.cpp (генерируется)

### @MusicRadioDB/config.cpp

`@MusicRadioDB/config.cpp` - 172 строк, 5507 байт.

```cpp
// =============================================================================
//  MusicRadio - @MusicRadioDB/config.cpp
//  GENERATED by tools/generate_db_config.py - do not edit, it will be rewritten.
//
//  What is here: CfgSoundShaders (one per track) + CfgSoundSets (one per track)
//  + the carrier item classes (one cassette and one MiniDisc per album).
//  The audio itself lives next to this file: Music/<Genre>/<trackId>.ogg
//
//  С 20261173 этот мод можно обновлять ОТДЕЛЬНО от @MusicRadioCore: все
//  альбомы (классы кассет/дисков) живут здесь. Core пересобирать не нужно.
//  Порядок загрузки: Core раньше DB (requiredAddons ниже).
//
//  Sound names used by the game code:
//     MusicRadioDB/Music/<Genre>/<trackId>       <- sample path (no extension)
//     MR_MUSICRADIODB_<TRACKID>_SoundShader
//     MR_MUSICRADIODB_<TRACKID>_SoundSet
// =============================================================================

class CfgPatches
{
	class MusicRadioDB
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = { "DZ_Data", "DZ_Sounds_Effects", "MusicRadioCore" };
	};
};

class CfgMods
{
	class MusicRadioDB
	{
		dir = "MusicRadioDB";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "MusicRadio - Database";
		credits = "MusicRadio project";
		author = "MusicRadio project";
		authorID = "0";
		version = "1.1.0";
		extra = 0;
		type = "mod";
		dependencies[] = { "Game", "World", "Mission" };

		// Only folders that actually exist: the media pack has scripts in
		// 4_World only. A defs entry pointing at a missing folder makes the
		// staging check (and the game log) complain, and adds nothing.
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = { "MusicRadioDB/Scripts/4_World" };
			};
		};
	};
};

class CfgSoundShaders
{
	// ---------------------------------------------------------------- TRACKS
	// ------------------------------------------------------------------
	// GENERATED - do not edit, run tools/generate_db_config.py
	// ------------------------------------------------------------------
	class MR_MUSICRADIODB_DEMO_TRACK_01_SoundShader
	{
		samples[] = { { "MusicRadioDB/Music/Rock/demo_track_01", 1 } };
		volume = 0.85;
		range = 60;
		limitation = 0;
	};

	class MR_MUSICRADIODB_DEMO_TRACK_02_SoundShader
	{
		samples[] = { { "MusicRadioDB/Music/Rock/demo_track_02", 1 } };
		volume = 0.80;
		range = 60;
		limitation = 0;
	};

};

class CfgSoundSets
{
	// ---------------------------------------------------------------- TRACKS
	// ------------------------------------------------------------------
	// GENERATED - do not edit, run tools/generate_db_config.py
	// ------------------------------------------------------------------
	class MR_MUSICRADIODB_DEMO_TRACK_01_SoundSet
	{
		soundShaders[] = { "MR_MUSICRADIODB_DEMO_TRACK_01_SoundShader" };
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
		occlusionFactor = 0.35;
		obstructionFactor = 0.5;
	};

	class MR_MUSICRADIODB_DEMO_TRACK_02_SoundSet
	{
		soundShaders[] = { "MR_MUSICRADIODB_DEMO_TRACK_02_SoundShader" };
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 0;
		occlusionFactor = 0.35;
		obstructionFactor = 0.5;
	};

};

// =============================================================================
//  MEDIA CARRIERS - классы носителей, по одному на альбом
// -----------------------------------------------------------------------------
//  Почему они здесь, а не в Core: альбом - это контент базы, поэтому добавили
//  альбом -> пересобирается только @MusicRadioDB, Core остаётся прежним.
//  requiredAddons выше гарантирует, что Core загрузится раньше: базовые классы
//  MR_CassetteCase_Base / MR_MiniDisc_Base объявлены в нём.
//
//  Ниже только объявления (без тел): в одном блоке CfgVehicles повторное
//  "class X : Base" даёт '/CfgVehicles.X: Member already defined'.
// =============================================================================
class CfgVehicles
{
	class Inventory_Base;

	class MR_CassetteCase_Base;
	class MR_MiniDisc_Base;

	// @@CARRIER_CLASSES_BEGIN@@
	class MR_Cassette_Rock01 : MR_CassetteCase_Base
	{
		scope = 2;
		displayName = "Cassette - Demo Rock A (Placeholder Artist)";
		descriptionShort = "Side A: Demo Rock A, ~7 min. Insert into a cassette player.";
		hiddenSelections[] = { "mr_cassette" };
		hiddenSelectionsMaterials[] = { "MusicRadioCore\data\tex\mr_cassette_rock01.rvmat" };
		class CfgCassette
		{
			isPlaylist = 1;
			soundSets[] =
			{
				"MR_MUSICRADIODB_DEMO_TRACK_01_SoundSet",
				"MR_MUSICRADIODB_DEMO_TRACK_02_SoundSet"
			};
		};
	};

	class MR_MiniDisc_Rock01 : MR_MiniDisc_Base
	{
		scope = 2;
		displayName = "MiniDisc - Demo Rock A (Placeholder Artist)";
		descriptionShort = "Demo Rock A, ~7 min. Insert into an MD player.";
		hiddenSelections[] = { "mr_minidisc" };
		hiddenSelectionsMaterials[] = { "MusicRadioCore\data\tex\mr_minidisc_rock01.rvmat" };
		class CfgCassette
		{
			isPlaylist = 1;
			soundSets[] =
			{
				"MR_MUSICRADIODB_DEMO_TRACK_01_SoundSet",
				"MR_MUSICRADIODB_DEMO_TRACK_02_SoundSet"
			};
		};
	};
	// @@CARRIER_CLASSES_END@@
};
```

## 8. DB - данные медиапака

### @MusicRadioDB/Music/index.json

`@MusicRadioDB/Music/index.json` - 48 строк, 1096 байт.

```json
{
  "provider": "MusicRadioDB",
  "version": 1,
  "generated_by": "tools/generate_db_config.py",
  "tracks": [
    {
      "id": "demo_track_01",
      "provider": "MusicRadioDB",
      "kind": "music",
      "title": "Demo Track One",
      "artist": "Placeholder Artist",
      "album": "Demo Rock A",
      "albumId": "rock01",
      "year": 2026,
      "duration": 222,
      "tags": [
        "demo",
        "rock"
      ],
      "file": "Music/Rock/demo_track_01.ogg",
      "volume": 0.85,
      "bitrate": 0,
      "enabled": true,
      "explicit": false,
      "loop": false,
      "soundset": ""
    },
    {
      "id": "demo_track_02",
      "provider": "MusicRadioDB",
      "kind": "music",
      "title": "Demo Track Two",
      "artist": "Placeholder Artist",
      "album": "Demo Rock A",
      "albumId": "rock01",
      "year": 2026,
      "duration": 215,
      "tags": "chill, night",
      "file": "Music/Rock/demo_track_02.ogg",
      "volume": 0.8,
      "bitrate": 0,
      "enabled": true,
      "explicit": false,
      "loop": false,
      "soundset": ""
    }
  ]
}
```

### @MusicRadioDB/Music/carriers.json

`@MusicRadioDB/Music/carriers.json` - 18 строк, 430 байт.

```json
{
  "provider": "MusicRadioDB",
  "note": "one carrier = one album; classes must exist in @MusicRadioCore/config.cpp",
  "carriers": [
    {
      "albumId": "rock01",
      "album": "Demo Rock A",
      "artist": "Placeholder Artist",
      "tracks": [
        "demo_track_01",
        "demo_track_02"
      ],
      "totalDuration": 437,
      "cassette": "MR_Cassette_Rock01",
      "minidisc": "MR_MiniDisc_Rock01"
    }
  ]
}
```

### @MusicRadioDB/Music/DatabaseConfig.json

`@MusicRadioDB/Music/DatabaseConfig.json` - 23 строк, 890 байт.

```json
{
  // ===========================================================================
  //  MusicRadio - media library configuration (server + client)
  //  Path inside the DB mod:  @MusicRadioDB/Music/DatabaseConfig.json
  //
  //  This file is OPTIONAL. It only declares things the game needs to know
  //  before it can read index.json (e.g. a second media pack).
  // ===========================================================================

  "provider": "MusicRadioDB",
  "version": 1,

  // Extra providers: if you run two media packs (e.g. a "core" pack and a
  // seasonal one), list them here. Each must ship its own index and soundsets.
  "providers": [
    "MusicRadioDB"
  ],

  // Reserved for future per-pack settings (e.g. a second provider listed in
  // "providers" above). Keep this file even if empty: the loader expects it.
  "notes": "media pack only, no streaming"
}
```

### @MusicRadioDB/Music/Rock/demo_playlist.json

`@MusicRadioDB/Music/Rock/demo_playlist.json` - 46 строк, 1344 байт.

```json
{
  // ---------------------------------------------------------------------------
  //  MusicRadioDB - merged media index
  //  This file is GENERATED by tools/generate_db_config.py - do not hand edit,
  //  edit the per-genre files instead and regenerate.
  //
  //  Shapes accepted by the parser (both work):
  //     "tracks": [ { ... }, { ... } ]
  //     "tracks": { "1": { ... }, "2": { ... } }
  //  Comments (# and //), trailing commas and single quotes are tolerated.
  // ---------------------------------------------------------------------------
  "provider": "MusicRadioDB",
  "version": 1,
  "tracks":
  [
    {
      "id": "demo_track_01",
      "kind": "music",
      "title": "Demo Track One",
      "artist": "Placeholder Artist",
      "album": "Demo Rock A",
      "albumId": "rock01",
      "year": 2026,
      "duration": "3:42",
      "tags": [ "demo", "rock" ],
      "file": "Music/Rock/demo_track_01.ogg",
      "volume": 0.85,
      "enabled": true
    },
    {
      "id": "demo_track_02",
      "kind": "music",
      "title": "Demo Track Two",
      "artist": "Placeholder Artist",
      "album": "Demo Rock A",
      "albumId": "rock01",
      "year": 2026,
      "duration": 215,
      "tags": "chill, night",
      "file": "Music/Rock/demo_track_02.ogg",
      "volume": 0.8,
      "enabled": true
    }
  ]
}
```

### @MusicRadioDB/CREDITS.txt

`@MusicRadioDB/CREDITS.txt` - 22 строк, 689 байт.

```text
MusicRadio - Database (media pack)
==================================

This pack contains no third party music yet: the two .ogg files are silent
placeholders used to test the pipeline.

When you add real music, list every track here with its source and licence:

  <file>            <title> - <artist>
  Source            : ...
  Licence           : ...
  Allowed in stream : yes / no

Rules
-----
1. Only add audio you have the right to redistribute.
2. Keep the json entry and this file in sync.
3. Tracks you may not want public broadcast can be marked "explicit": true
   and disabled server side with general.allowExplicit = false.

See docs/04_MEDIA_PACK.md for the whole workflow.
```

### @MusicRadioDB/Scripts/4_World/MR_DB_Placeholder.c

`@MusicRadioDB/Scripts/4_World/MR_DB_Placeholder.c` - 25 строк, 869 байт.

```cpp
// ============================================================================
//  MusicRadio - Database
//  MR_DB_Placeholder.c
//
//  The media pack needs no logic: all it does is declare CfgSoundShaders /
//  CfgSoundSets in config.cpp. This file exists only so the Scripts folder in
//  the workshop item is not empty (some tools warn about that) and to give
//  future per-pack extensions a home.
//
//  Do not add gameplay code here - it would be installed on every client and
//  the DB mod is meant to stay a pure content pack.
// ============================================================================

class MR_DB_Info
{
	static const string PROVIDER = "MusicRadioDB";
	static const string VERSION  = "0.1.0";
	static const int    SLOTS    = 8;

	static string Describe()
	{
		return "MusicRadio media pack, provider=" + PROVIDER + " v" + VERSION;
	}
}
```

## 9. Файлы для сервера

### server_files/profiles/MusicRadio/AudioConfig.json

`server_files/profiles/MusicRadio/AudioConfig.json` - 54 строк, 1939 байт.

```json
{
  // ===========================================================================
  //  MusicRadio - server configuration
  //  Path on the server:  mpmissions/<your_map>/MusicRadio/AudioConfig.json
  //  (the mod writes a copy of this file there on first start)
  //
  //  Comments and trailing commas are allowed, the parser tolerates them.
  // ===========================================================================

  "general": {
    // volume is 0..10 for players, 10 = 100%
    "defaultVolume": 6,
    // how far the music carries, meters
    "defaultRadius": 40,
    "maxVolume": 10,
    "maxRadius": 80,
    // allow tracks flagged "explicit": true in the media database
    "allowExplicit": false
  },

  "features": {
    // music in cars/trucks/helicopters
    "vehicleRadio": true,
    // handheld radio item (PersonalRadio) can play music too
    "handheldRadio": true,
    // a radio item must physically be in the vehicle cargo
    "requireRadioItemInCargo": true,
    // local .ogg tracks from the media pack
    "localTracks": true,
    // other players nearby hear the music; false = headphones mode (only you)
    "broadcastToOthers": true,
    // handheld radio transmits on the same frequency to nearby radios
    "radioTransmission": true,
    // when a track ends, play a random next one from the same category
    "autoplayNext": true,
    // 20261174: handheld radio is a real player - it needs a cassette/disc
    // inserted and a memory card, exactly like a deck. false = old behaviour
    // ("radio plays anything from the pack, no carrier needed")
    "radioNeedsCarrier": true
  },

  "limits": {
    // seconds between two play requests from the same player
    "cooldownSeconds": 3,
    // write every request into MusicRadio.log
    "logRequests": true
  },

  "database": {
    // index written by tools/generate_db_config.py, inside the DB mod
    "index": "MusicRadioDB/Music/index.json"
  }
}
```

### server_files/mpmissions/types_musicradio.xml

`server_files/mpmissions/types_musicradio.xml` - 80 строк, 2772 байт.

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!--
  MusicRadio types.xml snippet (GENERATED by tools/generate_db_config.py)
  Do not edit by hand: it is rebuilt from Music/carriers.json.
  Merge into your mission types.xml (the blocks are plain <type> entries),
  or copy this file next to it as types_musicradio.xml if your launcher
  supports several type files.
-->
<types>
    <type name="MR_MemoryCard_01">
        <nominal>25</nominal>
        <lifetime>14400</lifetime>
        <restock>600</restock>
        <min>8</min>
        <quantmin>-1</quantmin>
        <quantmax>-1</quantmax>
        <cost>100</cost>
        <flags count_in_cargo="0" count_in_hoarder="0" count_in_map="1" count_in_player="0" crafted="0" deloot="0"/>
        <category name="tools"/>
        <tag name="shelves"/>
        <usage name="Town"/>
        <usage name="Village"/>
        <usage name="Office"/>
        <usage name="School"/>
        <value name="Tier1"/>
        <value name="Tier2"/>
    </type>
    <type name="MR_CassettePlayer">
        <nominal>8</nominal>
        <lifetime>21600</lifetime>
        <restock>900</restock>
        <min>3</min>
        <quantmin>-1</quantmin>
        <quantmax>-1</quantmax>
        <cost>100</cost>
        <flags count_in_cargo="0" count_in_hoarder="0" count_in_map="1" count_in_player="0" crafted="0" deloot="0"/>
        <category name="tools"/>
        <tag name="shelves"/>
        <usage name="Town"/>
        <usage name="Village"/>
        <usage name="Office"/>
        <value name="Tier1"/>
        <value name="Tier2"/>
    </type>
    <type name="MR_CassettePlayer_Black">
        <nominal>4</nominal>
        <lifetime>21600</lifetime>
        <restock>900</restock>
        <min>1</min>
        <quantmin>-1</quantmin>
        <quantmax>-1</quantmax>
        <cost>100</cost>
        <flags count_in_cargo="0" count_in_hoarder="0" count_in_map="1" count_in_player="0" crafted="0" deloot="0"/>
        <category name="tools"/>
        <tag name="shelves"/>
        <usage name="Town"/>
        <usage name="Office"/>
        <usage name="School"/>
        <value name="Tier2"/>
        <value name="Tier3"/>
    </type>
    <type name="MR_MiniDiscPlayer">
        <nominal>5</nominal>
        <lifetime>21600</lifetime>
        <restock>900</restock>
        <min>2</min>
        <quantmin>-1</quantmin>
        <quantmax>-1</quantmax>
        <cost>100</cost>
        <flags count_in_cargo="0" count_in_hoarder="0" count_in_map="1" count_in_player="0" crafted="0" deloot="0"/>
        <category name="tools"/>
        <tag name="shelves"/>
        <usage name="Town"/>
        <usage name="Office"/>
        <usage name="School"/>
        <value name="Tier2"/>
        <value name="Tier3"/>
    </type>
</types>
```

### server_files/mpmissions/cfgspawnabletypes.xml

`server_files/mpmissions/cfgspawnabletypes.xml` - 52 строк, 1955 байт.

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!--
  MusicRadio - cfgspawnabletypes.xml snippet
  ---------------------------------------------------------------------------
  GENERATED by tools/generate_db_config.py - do not edit by hand.
  If your mission already has a cfgspawnabletypes.xml, do NOT replace it:
  copy the <type> blocks below into it (they merge by name).
  If it has none, copy this whole file next to your types.xml.

  What this does: a player in the world sometimes comes with a tape or
  disc inside, so a fresh player can try the mod immediately. Chances are
  low on purpose - carriers are "records", not ammo. Set them to 1.00 if
  you want every spawned player to be playable at once.
-->
<cfgspawnabletypes>
    <type name="MR_CassettePlayer">
        <attachments chance="1.00">
            <item name="BatteryD" chance="0.60"/>
        </attachments>
        <attachments chance="0.50">
            <item name="MR_MemoryCard_01" chance="1.00"/>
        </attachments>
        <attachments chance="0.40">
            <item name="MR_Cassette_Rock01" chance="1.00"/>
        </attachments>
    </type>

    <type name="MR_CassettePlayer_Black">
        <attachments chance="1.00">
            <item name="BatteryD" chance="0.60"/>
        </attachments>
        <attachments chance="0.50">
            <item name="MR_MemoryCard_01" chance="1.00"/>
        </attachments>
        <attachments chance="0.40">
            <item name="MR_Cassette_Rock01" chance="1.00"/>
        </attachments>
    </type>

    <type name="MR_MiniDiscPlayer">
        <attachments chance="1.00">
            <item name="BatteryD" chance="0.60"/>
        </attachments>
        <attachments chance="0.50">
            <item name="MR_MemoryCard_01" chance="1.00"/>
        </attachments>
        <attachments chance="0.40">
            <item name="MR_MiniDisc_Rock01" chance="1.00"/>
        </attachments>
    </type>
</cfgspawnabletypes>
```

## 10. Справочник: прокси для торпеды (не подключается)

### Models/config_proxy_snippet.cpp

`Models/config_proxy_snippet.cpp` - 85 строк, 2636 байт.

```cpp
// =============================================================================
//  MusicRadio - proxy snippet for VISIBLE docking
//
//  Only needed once the .p3d models from this folder exist and the car / radio
//  models have the matching selection names. Without it everything still works
//  logically (docked deck plays music), the deck is just not drawn on the dash.
//
//  How to use: paste the blocks below into @MusicRadioCore/config.cpp
//  (inside the existing CfgVehicles / CfgSlots / CfgNonAIVehicles sections),
//  then uncomment and fix the paths.
// =============================================================================

/* ---------------------------------------------------------------------------
   1) Slot points at a selection on the vehicle model.
      Add the selection "MR_DashSlot" to each supported car .p3d first, then
      the slot can tell the engine where to draw the proxy.
--------------------------------------------------------------------------- */
/*
class CfgSlots
{
	class Slot_MR_DashSlot
	{
		name = "MR_DashSlot";
		displayName = "Music player";
		selection = "MR_DashSlot";     // named selection on the car model
		ghostIcon = "missing";
	};

	class Slot_MR_MemoryCard
	{
		name = "MR_MemoryCard";
		displayName = "Memory card";
		selection = "MR_MemoryCard";   // named selection on PersonalRadio.p3d
		ghostIcon = "missing";
	};
};
*/

/* ---------------------------------------------------------------------------
   2) Proxy: what the engine renders inside the slot.
      inventorySlot[] on the proxy must match the slot name.
--------------------------------------------------------------------------- */
/*
class CfgNonAIVehicles
{
	class ProxyAttachment;

	class ProxyMR_MemoryCard : ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = { "MR_MemoryCard" };
		model = "MusicRadioCore\data\mr_memorycard.p3d";
	};

	class ProxyMR_DashPlayer : ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = { "MR_DashSlot" };
		model = "MusicRadioCore\data\mr_mdplayer.p3d";
	};

	class ProxyMR_DashDeck : ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = { "MR_DashSlot" };
		model = "MusicRadioCore\data\mr_cassettedeck.p3d";
	};
};
*/

/* ---------------------------------------------------------------------------
   3) Items that are drawn through the proxy need the slot on their side too
      (already set for the memory card in the main config).
--------------------------------------------------------------------------- */
/*
class CfgVehicles
{
	class MR_MemoryCard_Base;
	class MR_MemoryCard_01 : MR_MemoryCard_Base
	{
		inventorySlot[] = { "MR_MemoryCard" };
	};
};
*/
```

---

## Конец файла

Файлов: 47. Символов кода: ~274 КБ. Ревизия 20261175.
Проверить целостность: `python tools/make_single_md.py --check`
