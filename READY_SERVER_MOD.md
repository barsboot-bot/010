# Готовый мод MusicRadio для сервера (сборка 2026-11-21)

## Состав мода (три компонента)

1. `@MusicRadioCore` — логика, UI, воспроизведение (сервер + клиент)
2. `@MusicRadioServer` — правила, права, аудит (только сервер)
3. `@MusicRadioDB` — музыка `.ogg` + `CfgSoundSets` (сервер + клиент)

## Структура репозитория (готовая к развертыванию)

```
MusicRadio_Server_Package/
├── @MusicRadioCore/
│   ├── Addons/
│   │   └── MusicRadioCore.pbo
│   │   └── MusicRadioCore.pbo.KRaTos.bisign
│   └── keys/
│       └── KRaTos.bikey
├── @MusicRadioDB/
│   ├── Addons/
│   │   └── MusicRadioDB.pbo
│   │   └── MusicRadioDB.pbo.KRaTos.bisign
│   └── keys/
│       └── KRaTos.bikey
├── ServerMOD/
│   └── Addons/
│       └── MusicRadioServer.pbo
└── keys/
    └── KRaTos.bikey
```

## Файлы конфигурации сервера

### 1. `serverDZ.cfg` (фрагмент)

```cfg
mod=@MusicRadioCore;@MusicRadioDB;@VPPAdminTools;...
servermod=ServerMOD;ServerCARS
```

### 2. `mpmissions/dayzOffline.chernarusplus/MusicRadio/AudioConfig.json`

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

## Инструкция по развертыванию на сервере

1. **Подготовка ключей**
   - Создать `KRaTos.biprivatekey` через DayZ Tools → DSSignFile
   - Сгенерировать `KRaTos.bikey`
   - Разместить в `keys/` каждого мода и в корне сервера

2. **Размещение PBO**
   - `@MusicRadioCore/Addons/MusicRadioCore.pbo` (+ .bisign)
   - `@MusicRadioDB/Addons/MusicRadioDB.pbo` (+ .bisign)
   - `ServerMOD/Addons/MusicRadioServer.pbo` (без подписи)

3. **Конфигурация**
   - Скопировать `AudioConfig.json` в `mpmissions/<map>/MusicRadio/`
   - Обновить `serverDZ.cfg` с указанием модов

4. **Запуск сервера**
   ```bash
   ./DayZServer_x64.exe -config=serverDZ.cfg -port=2302 \
     "-mod=@MusicRadioCore;@MusicRadioDB;@VPPAdminTools" \
     "-servermod=ServerMOD;ServerCARS" \
     -dologs -adminlog -netlog
   ```

## Проверка готовности

- В логе сервера: `[MusicRadio] Server engine ready (protocol 1)`
- В логе сервера: `Library loaded: 2 entries`
- В клиенте: `[MusicRadio] Client mission ready`
- F7 в машине открывает меню, трек играет

## Инструменты сборки (для разработчиков)

```bash
# Перегенерировать базу (если меняли музыку)
python tools/generate_db_config.py

# Проверить и разложить staging
python tools/check_project.py
python tools/stage_pbo.py --clean

# Собрать PBO (Windows + DayZ Tools)
BUILD_PBO.bat
```

## Особенности ревизии 20261174

- Рация стала таким же плеером, как дека
- Единый контракт: рация и дека ведут себя одинаково
- Карта памяти нужна и рации, и деке
- Новый `MR_MediaBay` — одна проверка «что вставлено» для всех устройств

## Файлы документации

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
