# Готовый мод MusicRadio для сервера DayZ (Версия 1.30+)
## Музыка в машине (кассеты) + Персональная рация (MD Диски / SD Карты) + Патч машин и раций

**Создано по инструкциям из файлов 01–17 (версия 2026-11-21).**

---

## 1. Структура мода (согласно 17_HOW_IT_WAS_BUILT.md и 02_ARCHITECTURE.md)

```
MusicRadio_Server_Package/
├── @MusicRadioCore/              # Логика, UI, патч машин/раций (Версия 1.30+)
│   ├── Scripts/
│   │   ├── 3_Game/
│   │   │   ├── MR.c                 // Основная логика (пример из 17_HOW_IT_WAS_BUILT.md)
│   │   │   ├── MR_Locator.c
│   │   │   ├── MR_Json.c
│   │   │   ├── MR_Track.c
│   │   │   ├── MR_Library.c
│   │   │   ├── MR_Net.c
│   │   │   ├── MR_MediaResolver.c
│   │   │   ├── MR_DeviceBase.c
│   │   │   └── MR_AudioTypes.c
│   │   ├── 4_World/
│   │   │   ├── MR_Registry.c
│   │   │   ├── MR_Hosts.c
│   │   │   ├── CarScript_modded.c    // ПАТЧ МАШИН (версия 1.30+)
│   │   │   ├── PersonalRadio_modded.c // ПАТЧ РАЦИИ (версия 1.30+)
│   │   │   └── PlayerBase_modded.c
│   │   └── 5_Mission/
│   │       ├── MR_ClientRPC.c
│   │       ├── MR_RadioMenu.c
│   │       └── MR_GameHooks.c
│   ├── config.cpp           // CfgPatches, CfgMods, патч слотов (пример из 17_HOW_IT_WAS_BUILT.md и 13_CHECK_BUILD.md)
│   └── data/
│       └── tex/             // .rvmat (пример из 07_MODELS.md)
├── @MusicRadioDB/           # Музыка (.ogg) + Контент (кассеты, MD, SD)
│   ├── Music/               // .ogg (пример из 03_MEDIA_PACK.md)
│   ├── Scripts/
│   │   └── 4_World/
│   └── config.cpp           // CfgSoundSets, классы кассет/дисков (пример из 17_HOW_IT_WAS_BUILT.md)
├── @MusicRadioServer/       # Права, лимиты, аудит (только сервер)
│   ├── Scripts/
│   │   ├── 4_World/
│   │   └── 5_Mission/
│   │       ├── MR_ServerEngine.c
│   │       ├── MR_RPC.c
│   │       └── MR_Limits.c
│   └── config.cpp           // серверный мод (пример из 17_HOW_IT_WAS_BUILT.md)
├── tools/                   # Python-скрипты (пример из 17_HOW_IT_WAS_BUILT.md)
│   ├── generate_db_config.py
│   ├── stage_pbo.py
│   ├── check_project.py
│   ├── check_bat.py
│   ├── check_fresh.py
│   ├── make_textures.py
│   ├── import_music.py
│   ├── update_tools.py
│   └── autobuild.py
├── server_files/            # types.xml, cfgspawnabletypes, AudioConfig.json (пример из 17_HOW_IT_WAS_BUILT.md)
│   ├── types.xml
│   ├── cfgspawnabletypes.xml
│   └── AudioConfig.json     // пример из 01_INSTALL.md
├── docs/                    # Документация (01-17)
│   ├── 01_INSTALL.md
│   ├── 02_ARCHITECTURE.md
│   ├── 03_MEDIA_PACK.md
│   ├── 04_STATUS.md
│   ├── 05_PBO_BUILD.md
│   ├── 06_CARRIERS.md
│   ├── 07_MODELS.md
│   ├── 08_NEXT_STEPS.md
│   ├── 09_TEXTURES.md
│   ├── 10_LIMITS.md
│   ├── 11_TROUBLESHOOTING.md
│   ├── 12_CHANGES_VOS.md
│   ├── 13_CHECK_BUILD.md
│   ├── 14_BAT_FILES.md
│   ├── 15_TEST_IN_GAME.md
│   ├── 16_TODO.md
│   └── 17_HOW_IT_WAS_BUILT.md
├── Models/                  # .obj, tex_src/*.png, .rvmat (пример из 07_MODELS.md)
│   ├── src/
│   │   ├── mr_cassette.obj
│   │   ├── mr_minidisc.obj
│   │   ├── mr_mdplayer.obj
│   │   ├── mr_cassettedeck.obj
│   │   └── mr_memorycard.obj
│   ├── preview/
│   ├── tex_src/
│   └── README.md
└── *.bat                    // AUTO_BUILD, BUILD_PBO, DEPLOY, CHECK, EXPLAIN_STALE, IMPORT_MUSIC, REBUILD (пример из 14_BAT_FILES.md)
```

## 2. Реализация патча машин и раций (Версия 1.30+)
Согласно **13_CHECK_BUILD.md** (ревизия 20261174) и **01_INSTALL.md**:

### Патч машин (`CarScript_modded.c` в `@MusicRadioCore/Scripts/4_World/`)
*   **Слот `MR_DashSlot`** (пример из `13_CHECK_BUILD.md`):
    ```cpp
    class CfgSlots {
        class Slot_MR_DashSlot {
            name = "MR_DashSlot";
            selection = "MR_DashSlot";
        };
    };
    ```
*   **Модель машины** (пример из `07_MODELS.md`): селекшен `MR_DashSlot` на торпеде.
*   **Логика** (пример из `13_CHECK_BUILD.md`): `MR_ItemHost.MR_CanPlay()`, `MR_BlockReason()`, `MR_MediaBay.IsReady()`.

### Патч рации (`PersonalRadio_modded.c` в `@MusicRadioCore/Scripts/4_World/`)
*   **Наследник `Transmitter_Base`** (пример из `13_CHECK_BUILD.md`):
    ```cpp
    class PersonalRadio : Transmitter_Base {
        attachments[] += { "MR_MemoryCard", "MR_Cassette", "MR_MiniDisc" };
        class GUIInventoryAttachmentsProps {
            class MR_MusicMedia { ... attachmentSlots[] = { карта, кассета, диск }; };
        };
    };
    ```
*   **Слоты**: карта памяти, кассета, MD-диск (пример из `13_CHECK_BUILD.md` и `06_CARRIERS.md`).
*   **Логика**: `MR_MediaBay.IsReady()`, `MR_InsertedCarrier()`, `MR_GetInsertedCassette()`, `MR_GetInsertedDisc()`, `MR_GetMemoryCard()`, `MR_HasBattery()`, `MR_IsReady()`, `MR_CurrentAlbum()`, `MR_MediaStatus()` (пример из `13_CHECK_BUILD.md`).

### Музыка в машине (кассеты) и рации (MD/SD)
*   **Кассеты** (`@MusicRadioDB/config.cpp`): классы `MR_Cassette_Rock01` и др. (пример из `07_MODELS.md` и `13_CHECK_BUILD.md`).
*   **MD Диски / SD Карты** (`@MusicRadioDB/config.cpp`): классы `MR_MiniDisc`, `MR_MemoryCard` (пример из `07_MODELS.md` и `13_CHECK_BUILD.md`).
*   **Контракт «трек → звук»** (пример из `02_ARCHITECTURE.md`): `LCT|1|MusicRadioDB|wasteland_01|6|48311` → `MR_MUSICRADIODB_WASTELAND_01_SoundSet`.

## 3. Конфигурация сервера (Версия 1.30+)
### serverDZ.cfg (фрагмент из 01_INSTALL.md)
```cfg
mod=@MusicRadioCore;@MusicRadioDB;@VPPAdminTools;...
servermod=ServerMOD;ServerCARS
```

### mpmissions/dayzOffline.chernarusplus/MusicRadio/AudioConfig.json (пример из 01_INSTALL.md)
```json
{
  "features": {
    "vehicleRadio": true,              // музыка в машинах
    "requireRadioItemInCargo": true,   // нужна рация в грузе машины
    "radioNeedsCarrier": true,         // рация играет только вставленную кассету
    "localTracks": true,               // музыка из медиа-пака
    "broadcastToOthers": true,         // остальные рядом слышат музыку
    "autoplayNext": true               // после трека включается следующий
  },
  "limits": {
    "maxVolume": 100,
    "logRequests": true,
    "maxDistance": 50
  }
}
```

## 4. Инструкция по развертыванию (Версия 1.30+)
1. **Подготовка ключей** (DayZ Tools → DSSignFile → `KRaTos.biprivatekey` + `KRaTos.bikey`) — согласно `05_PBO_BUILD.md`.
2. **Сборка PBO** (`BUILD_PBO.bat` → `release/pbo/*.pbo` + `.bisign`) — согласно `05_PBO_BUILD.md` и `14_BAT_FILES.md`.
3. **Развертывание** (`DEPLOY.bat` → сервер + `release/deploy`) — согласно `14_BAT_FILES.md`.
4. **Запуск сервера** (версия 1.30+) — согласно `01_INSTALL.md`:
   ```bash
   ./DayZServer_x64.exe -config=serverDZ.cfg -port=2302 \
     "-mod=@MusicRadioCore;@MusicRadioDB;@VPPAdminTools" \
     "-servermod=ServerMOD;ServerCARS" \
     -dologs -adminlog -netlog
   ```

## 5. Файлы документации (01-17)
- [01_INSTALL.md](01_INSTALL.md) — Установка MusicRadio
- [02_ARCHITECTURE.md](02_ARCHITECTURE.md) — Архитектура
- [03_MEDIA_PACK.md](03_MEDIA_PACK.md) — Медиа-пак
- [04_STATUS.md](04_STATUS.md) — Статус проекта
- [05_PBO_BUILD.md](05_PBO_BUILD.md) — Как упаковать в PBO
- [06_CARRIERS.md](06_CARRIERS.md) — Носители
- [07_MODELS.md](07_MODELS.md) — Модели
- [08_NEXT_STEPS.md](08_NEXT_STEPS.md) — Следующие шаги
- [09_TEXTURES.md](09_TEXTURES.md) — Текстуры
- [10_LIMITS.md](10_LIMITS.md) — Лимиты
- [11_TROUBLESHOOTING.md](11_TROUBLESHOOTING.md) — Решение проблем
- [12_CHANGES_VOS.md](12_CHANGES_VOS.md) — Изменения
- [13_CHECK_BUILD.md](13_CHECK_BUILD.md) — Проверка сборки
- [14_BAT_FILES.md](14_BAT_FILES.md) — Батники
- [15_TEST_IN_GAME.md](15_TEST_IN_GAME.md) — Тестирование в игре
- [16_TODO.md](16_TODO.md) — Что сделать
- [17_HOW_IT_WAS_BUILT.md](17_HOW_IT_WAS_BUILT.md) — Как было сделано
