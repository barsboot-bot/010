# MusicRadio — Готовый мод для сервера (создано по MD инструкциям)

## 1. Структура проекта (согласно 17_HOW_IT_WAS_BUILT.md и 02_ARCHITECTURE.md)

```
MusicRadio/
├── @MusicRadioCore/
│   ├── Scripts/
│   │   ├── 3_Game/
│   │   │   ├── MR.c                 # логика (пример из 17_HOW_IT_WAS_BUILT.md)
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
│   │   │   ├── CarScript_modded.c
│   │   │   ├── PersonalRadio_modded.c
│   │   │   └── PlayerBase_modded.c
│   │   └── 5_Mission/
│   │       ├── MR_ClientRPC.c
│   │       ├── MR_RadioMenu.c
│   │       └── MR_GameHooks.c
│   ├── config.cpp           # CfgPatches, CfgMods (пример из 17_HOW_IT_WAS_BUILT.md)
│   └── data/
│       └── tex/             # .rvmat (пример из 07_MODELS.md)
├── @MusicRadioDB/
│   ├── Music/               # .ogg (пример из 03_MEDIA_PACK.md)
│   ├── Scripts/
│   │   └── 4_World/
│   └── config.cpp           # CfgSoundSets, классы кассет/дисков (пример из 17_HOW_IT_WAS_BUILT.md)
├── @MusicRadioServer/
│   ├── Scripts/
│   │   ├── 4_World/
│   │   └── 5_Mission/
│   │       ├── MR_ServerEngine.c
│   │       ├── MR_RPC.c
│   │       └── MR_Limits.c
│   └── config.cpp           # серверный мод (пример из 17_HOW_IT_WAS_BUILT.md)
├── tools/                   # python-скрипты (пример из 17_HOW_IT_WAS_BUILT.md)
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
│   └── AudioConfig.json     # пример из 01_INSTALL.md
├── docs/                    # документация (01-17)
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
└── *.bat                    # AUTO_BUILD, BUILD_PBO, DEPLOY, CHECK, EXPLAIN_STALE, IMPORT_MUSIC, REBUILD (пример из 14_BAT_FILES.md)
```

## 2. Конфигурация сервера (согласно 01_INSTALL.md и 13_CHECK_BUILD.md)

### serverDZ.cfg (фрагмент)
```cfg
mod=@MusicRadioCore;@MusicRadioDB;@VPPAdminTools;...
servermod=ServerMOD;ServerCARS
```

### mpmissions/dayzOffline.chernarusplus/MusicRadio/AudioConfig.json (пример из 01_INSTALL.md)
```json
{
  "features": {
    "vehicleRadio": true,
    "requireRadioItemInCargo": true,
    "radioNeedsCarrier": true,
    "localTracks": true,
    "broadcastToOthers": true,
    "autoplayNext": true
  },
  "limits": {
    "maxVolume": 100,
    "logRequests": true,
    "maxDistance": 50
  }
}
```

## 3. Скрипты C++ (примеры из 17_HOW_IT_WAS_BUILT.md и 02_ARCHITECTURE.md)

### @MusicRadioCore/Scripts/3_Game/MR.c (пример)
```c
#include "Script_Component.h"
class MR {
    // логика, UI, воспроизведение (пример из 17_HOW_IT_WAS_BUILT.md)
};
```

### @MusicRadioCore/config.cpp (пример из 17_HOW_IT_WAS_BUILT.md)
```cpp
class CfgPatches {
    class MusicRadioCore {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = { "DZ_Data", "DZ_Scripts" };
    };
};
class CfgMods {
    class MusicRadioCore {
        dir = "MusicRadioCore";
        name = "MusicRadio - Core";
        type = "mod";
        dependencies[] = { "Game", "World", "Mission" };
        class defs {
            class gameScriptModule  { value=""; files[] = { "MusicRadioCore/Scripts/3_Game" }; };
            class worldScriptModule { value=""; files[] = { "MusicRadioCore/Scripts/4_World" }; };
            class missionScriptModule { value=""; files[] = { "MusicRadioCore/Scripts/5_Mission" }; };
        };
    };
};
```

## 4. Инструкция по развертыванию (согласно 01_INSTALL.md и 05_PBO_BUILD.md)

1. **Подготовка ключей** (DayZ Tools → DSSignFile → `KRaTos.biprivatekey` + `KRaTos.bikey`).
2. **Сборка PBO** (`BUILD_PBO.bat` → `release/pbo/*.pbo` + `.bisign`).
3. **Развертывание** (`DEPLOY.bat` → сервер + `release/deploy`).
4. **Запуск сервера** (`DayZServer_x64.exe -config=serverDZ.cfg -port=2302 "-mod=@MusicRadioCore;@MusicRadioDB;@VPPAdminTools" "-servermod=ServerMOD;ServerCARS" -dologs -adminlog -netlog`).

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
