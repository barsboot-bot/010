# Готовый мод MusicRadio для сервера DayZ (Версия 1.30+)
## Музыка в машине (кассеты) + Персональная рация (MD Диски / SD Карты) + Патч машин и раций

**Ревизия 20261175. Полный исходный код из 18_FULL_SOURCE.md распакован в реальные файлы.**

---

## Что внутри (47 файлов из 18_FULL_SOURCE.md)

### @MusicRadioCore (логика, UI, патч машин/раций)
- `config.cpp` (464 строки) — CfgPatches, CfgMods, CfgSlots (MR_DashSlot, MR_MemoryCard, MR_Cassette, MR_MiniDisc), классы MR_MemoryCard_Base, MR_CassetteCase_Base, MR_MiniDisc_Base, MR_CassettePlayer, MR_MiniDiscPlayer, PersonalRadio (слоты MR_MemoryCard/MR_Cassette/MR_MiniDisc)
- `Scripts/3_Game/MusicRadio/` (13 файлов):
  - MR_Constants.c — константы, RPC id 784100+, helper'ы (StartsWith, NetId, VolumeStepToGain)
  - MR_Net.c — wire format (MR_Request, MR_Net.EncodePlay/DecodePlay, EncodeStop, EncodeVolume)
  - MR_Json.c — tolerant JSON parser (комментарии, trailing commas, single quotes)
  - MR_Track.c — трек (id, provider, albumId, file, soundset)
  - MR_Locator.c — LCT|1|provider|trackId|volume|seed
  - MR_MediaResolver.c — provider+trackId -> MR_<PROVIDER>_<ID>_SoundSet
  - MR_Library.c — библиотека треков, ByAlbum, Search, SerializeManifest
  - MR_Config.c — AudioConfig.json чтение/запись, MR_AuditLog
  - MR_AudioTypes.c — MR_Settings, MR_PlayRequest, MR_Session
  - MR_DeviceBase.c — базовый хост (MR_AudioStart, MR_AudioApplyVolume, MR_SetState)
  - MR_ClientAudio.c — реестр audible hosts, Tick, ReportEnded
  - MR_CarrierState.c — память кассеты (MRT1|marker|trackId|position|trackNo), CfgCassette
  - MR_SoundMix.c — modded EffectSound (MR_SetVolumeMax, MR_SetStartOffset)
- `Scripts/4_World/MusicRadio/` (9 файлов):
  - MR_Carriers.c — MR_CarrierBase (OnStoreSave/Load), MR_CassetteCase_Base, MR_MiniDisc_Base, MR_MemoryCard_Base
  - MR_Decks.c — MR_MediaBay (единый контракт: носитель -> карта -> батарейка), MR_CassettePlayer_Base, MR_MiniDiscPlayer_Base
  - MR_DeckResume.c — resume логика деки
  - MR_MediaItems.c — AlbumOf, IsCarrierPlayer, AlbumInVehicle, DockedInVehicle
  - MR_Hosts.c — MR_ItemHost, MR_VehicleHost (MR_CanPlay, MR_BlockReason)
  - MR_Registry.c — реестр хостов по network id, MR_VehicleRadio (HasRadioInCargo, HasPower)
  - MR_WorldHooks.c — modded PersonalRadio, CarScript, PlayerBase (MR_FindUsableHost)
  - MR_PlayerRPC.c — modded PlayerBase.OnRPC (клиентский мост в 5_Mission)
  - MR_Animations.c — modded ModItemRegisterCallbacks (IK профили для плееров)
- `Scripts/5_Mission/MusicRadio/` (5 файлов):
  - MR_ClientRPC.c — SendRequest, HandlePlay/Stop/Volume, ResolveHost (HOST_ITEM/VEHICLE/PLAYER)
  - MR_ClientState.c — Settings, Library, ApplyManifest
  - MR_LibraryIndex.c — singleton библиотеки
  - MR_RadioMenu.c — UI меню (F7), вкладки MUSIC/ALL/TAPES, слайдеры громкости/радиуса
  - MR_GameHooks.c — modded MissionGameplay (F7/F8, ToggleRadioMenu)
- `gui/layouts/radio_ui.layout`, `radio_row.layout` — UI
- `inputs.xml` — MRNextTrack (kRight), MRPreviousTrack (kLeft), MRTogglePlayer (kB)
- `stringtable.csv` — локализация (EN, RU, DE, CZ, PL, ES, FR)

### @MusicRadioServer (только сервер, клиентам не нужен)
- `config.cpp` — CfgPatches, CfgMods (world/mission modules)
- `Scripts/4_World/MusicRadio/MR_ServerEngine.c` (1059 строк) — authority: Init, HandleRequest, StartPlay, StartPlayAlbum, ValidateHost, DispatchPlay, ChangeVolume, NextTrack, StopSession, Tick, SyncActiveSessions
- `Scripts/4_World/MusicRadio/MR_ServerHooks.c` — modded CarScript/PersonalRadio/PlayerBase (EEKilled, EEDelete, OnRPC)
- `Scripts/5_Mission/MusicRadio/MR_ServerMission.c` — modded MissionServer (OnInit, DelayedJoinSync)

### @MusicRadioDB (контент, музыка)
- `config.cpp` (171 строка, генерируется) — CfgSoundShaders + CfgSoundSets (MR_MUSICRADIODB_DEMO_TRACK_01/02), классы MR_Cassette_Rock01, MR_MiniDisc_Rock01 (CfgCassette.isPlaylist=1, soundSets[])
- `Music/index.json` — 2 трека (demo_track_01, demo_track_02), albumId rock01
- `Music/carriers.json` — rock01 -> MR_Cassette_Rock01 / MR_MiniDisc_Rock01
- `Music/DatabaseConfig.json`, `Music/Rock/demo_playlist.json`, `CREDITS.txt`
- `Scripts/4_World/MR_DB_Placeholder.c` — MR_DB_Info

### server_files
- `profiles/MusicRadio/AudioConfig.json` — general (defaultVolume 6, defaultRadius 40), features (vehicleRadio, handheldRadio, requireRadioItemInCargo, localTracks, broadcastToOthers, radioTransmission, autoplayNext, radioNeedsCarrier), limits (cooldownSeconds 3, logRequests)
- `mpmissions/types_musicradio.xml` — спавн MR_MemoryCard_01, MR_CassettePlayer, MR_CassettePlayer_Black, MR_MiniDiscPlayer
- `mpmissions/cfgspawnabletypes.xml` — аттачменты (BatteryD, MR_MemoryCard_01, MR_Cassette_Rock01 / MR_MiniDisc_Rock01)

### Models
- `config_proxy_snippet.cpp` — прокси для торпеды (MR_DashSlot selection, ProxyMR_MemoryCard/DashPlayer/DashDeck)
- `src/*.p3d.txt` — планы создания .p3d в Object Builder (07_MODELS.md): mr_cassette (100×63.8×12мм), mr_minidisc (72×68×4мм), mr_memorycard (32×24×2.1мм), mr_mdplayer (118×72×17мм), mr_cassettedeck (140×110×35мм)


### Текстуры (перенесены из Core в DB)
- Исходники PNG: `Models/tex_src/` (mr_cassette_*, mr_minidisc_*, mr_memorycard_*, mr_mdplayer_*, mr_cassettedeck_*, mr_plastic_*, mr_metal_* и т.д.) — в PBO не идут
- Материалы `.rvmat` + бинарные `.paa`: `@MusicRadioDB/data/tex/` (идут в PBO @MusicRadioDB)
- Связка: `hiddenSelectionsMaterials[] = { "MusicRadioDB\\data\\tex\\mr_cassette_rock01.rvmat" }` в `@MusicRadioDB/config.cpp`
- Модели `.p3d` остаются в `@MusicRadioCore/data/` (mr_cassette.p3d, mr_minidisc.p3d и т.д.)
- Контактный лист: `Models/preview/textures_sheet.png`

### tools/ (16 python-скриптов)
- generate_db_config.py, stage_pbo.py, check_project.py, check_bat.py, check_fresh.py, make_single_md.py, import_music.py, make_textures.py, autobuild.py, check_db_config.py, check_pbo.py, check_log.py, doctors.py, update_tools.py, check_mdmp.py, build_carrier_cfgset.py

---

## Как проверить (13_CHECK_BUILD.md, 05_PBO_BUILD.md)

```bash
python tools/check_project.py          # баланс скобок, слои, дубли, modded vs class
python tools/check_db_config.py        # дубли CfgSoundSets/Shaders
python tools/generate_db_config.py     # пересобрать базу из index.json
python tools/stage_pbo.py --clean      # staging в release/staging (как для AddonBuilder)
python tools/check_bat.py              # CRLF в .bat
python tools/make_single_md.py --check # целостность 18_FULL_SOURCE.md
```

На Windows с DayZ Tools:
```bat
REBUILD.bat      :: текстуры + база + staging + проверки
BUILD_PBO.bat    :: AddonBuilder -> release\pbo\*.pbo (+ .bisign)
DEPLOY.bat       :: разложить по @Модам и на сервер (server_path.txt)
CHECK.bat        :: диагностика + логи
```

Ожидаемые логи в игре:
- Клиент: `[MusicRadio] Client mission ready`
- Сервер: `[MusicRadio] Server engine ready (protocol 1)`, `Library loaded: 2 entries`
- F7 в машине / с рацией в руках — меню, трек играет

---

## Патч машин и раций (Версия 1.30+)

**Машины:** слот `MR_DashSlot` в `CfgSlots` (без переобъявления ванильных машин — иначе `Member already defined`). В слот кладут кассету/диск ИЛИ плеер. Проверка — `MR_MediaItems.AlbumInVehicle()` / `DockedInVehicle()`, хост — `MR_VehicleHost` в `MR_WorldHooks.c` (modded CarScript).

**Рация:** `PersonalRadio : Transmitter_Base` в `config.cpp` (прямой парент!), `attachments[] += { "MR_MemoryCard", "MR_Cassette", "MR_MiniDisc" }`. Единый контракт — `MR_MediaBay` в `MR_Decks.c`: носитель -> карта памяти -> батарейка. Хост — `MR_ItemHost` в `MR_Hosts.c`.

**Плееры:** `MR_CassettePlayer` (слоты MR_MemoryCard + MR_Cassette + BatteryD), `MR_MiniDiscPlayer` (MR_MemoryCard + MR_MiniDisc + BatteryD). Карта вставляется в кассету/диск (у носителя есть слот MR_MemoryCard).

---

## Что НЕ включено (осознанно, из 18_FULL_SOURCE.md)

- `tools/` в оригинале — 35 файлов, здесь 16 базовых (остальные — selftest_*.py, check_archive.py и т.д.)
- `.ogg` музыка, `.png/.paa` текстуры, `.p3d` модели — бинарники, создаются через ffmpeg / Object Builder / TexView
- Ванильный `server_files/mpmissions/types.xml` (896 КБ) — в него только добавляются 4 записи из `types_musicradio.xml`

---

**Готовый мод для сервера DayZ (Версия 1.30+) — реальный код из 18_FULL_SOURCE.md (ревизия 20261175), 47 файлов, ~274 КБ кода.**
