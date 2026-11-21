# Установка MusicRadio

Три мода, две машины. Порядок важен только на сервере.

| Мод | Сервер | Клиент | Зачем |
|-----|:------:|:------:|-------|
| `@MusicRadioCore` | да | да | логика, UI, воспроизведение |
| `@MusicRadioServer` | да | **нет** | правила, права, лог (клиенты его не видят) |
| `@MusicRadioDB` | да | да | музыка: `.ogg` + `CfgSoundSets` |

> Клиенту нужны **два** мода: `@MusicRadioCore` и `@MusicRadioDB`.
> Серверу — **все три**.

## 1. Сборка PBO (Windows + DayZ Tools)

Готовить проект можно где угодно (нужен только python3), а собирать PBO — на
Windows с DayZ Tools.

1. Steam → *DayZ Tools* → установить.
2. Сгенерировать ключ: `DayZ Tools → DSSignFile` → создать
   `KRaTos.biprivatekey` (или любое своё имя - `BUILD_PBO.bat` ищет
   `KRaTos` и старое `musicradio`), положить в `release/keys/` рядом с `.bikey`.
3. Подготовить staging (проверяет контракт индекс ↔ config.cpp ↔ `.ogg` и
   раскладывает файлы так, как они лягут в PBO):

   ```bash
   python tools/generate_db_config.py
   python tools/stage_pbo.py --clean
   ```

4. `BUILD_PBO.bat` в корне сам находит DayZ Tools; если не нашёл —
   спросит путь. Старая строка про путь `TOOLS` (по умолчанию —
   `C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\Bin\AddonBuilder\AddonBuilder.exe`).
5. Запустить `BUILD_PBO.bat` (двойным щелчком). Результат: `release/pbo/*.pbo` (+ `.bisign`).

Пошагово с разбором ошибок: [05_PBO_BUILD.md](05_PBO_BUILD.md). Альтернатива без
скрипта: в DayZ Tools запустить **Addon Builder**, указать как source
`release/staging/MusicRadioX`, `-prefix=MusicRadioX`, output — `Addons` внутри
папки мода.

## 2. Сервер

```
DayZServer/
├── @MusicRadioCore/Addons/MusicRadioCore.pbo (+ .bisign)
├── @MusicRadioDB/Addons/MusicRadioDB.pbo     (+ .bisign)
├── ServerMOD/Addons/MusicRadioServer.pbo     (без подписи)
├── ServerCARS/                               <- ЧУЖАЯ папка, не трогаем
├── keys/KRaTos.bikey
└── mpmissions/dayzOffline.chernarusplus/
    └── MusicRadio/AudioConfig.json      <- положить из server_files/
```

Запуск:

```
DayZServer_x64.exe -config=serverDZ.cfg -port=2302 ^
  "-mod=@KGB.LIB;@CF;@...;@MusicRadioCore;@MusicRadioDB;@VPPAdminTools" ^
  "-servermod=ServerMOD;ServerCARS" ^
  -dologs -adminlog -netlog
```

Правила, которые уже проверяются батниками и тестами:

* `@MusicRadioCore` и `@MusicRadioDB` — **в `-mod`** (иначе клиенты не получат мод),
  и они подписаны;
* `MusicRadioServer.pbo` лежит **только** в `ServerMOD\Addons` и грузится строкой
  `-servermod=ServerMOD;ServerCARS`; подписи у него нет и не нужно;
* папки `@MusicRadioServer` в корне сервера быть не должно (`DEPLOY.bat` её убирает);
* `ServerCARS` — чужая папка (фиксы машин), наш мод туда не кладём никогда.

### Конфиг сервера

`mpmissions/<map>/MusicRadio/AudioConfig.json` — если файла нет, мод создаст его
с дефолтами при первом старте. Все ключи описаны в самом файле
(`server_files/profiles/MusicRadio/AudioConfig.json`).

Самое важное:

```json
"features": {
  "vehicleRadio": true,              // музыка в машинах
  "requireRadioItemInCargo": true,   // нужна рация в грузе машины
  "radioNeedsCarrier": true,         // рация играет только вставленную кассету
  "localTracks": true,               // музыка из медиа-пака
  "broadcastToOthers": true,         // остальные рядом слышат музыку
  "autoplayNext": true               // после трека включается следующий
}
```

### Логи

* `mpmissions/<map>/MusicRadio/MusicRadio.log` — аудит: кто что включил, когда,
  с какой громкостью (включено флагом `limits.logRequests`).
* Консоль сервера: строки с префиксом `[MusicRadio]`.

## 3. Клиент

1. Подписаться на воркшоп-версии `@MusicRadioCore` и `@MusicRadioDB`
   (или положить PBO в `DayZ/@...`).
2. В лаунчере включить оба мода.
3. Зайти на сервер.

**Проверка, что всё на месте:** в чате при заходе появляется
`[Radio] Library synced: N entries ...`.

## 4. Управление в игре

| Клавиша | Действие |
|---------|----------|
| **F7** | открыть/закрыть меню радио (рация с кассетой в руках или место в машине) |
| **F8** | стоп (последняя открытая рация) |
| ЛКМ по треку | выбрать |
| СКМ по треку | выбрать + играть |
| ESC | закрыть меню |

Радио доступно, когда:

* в руках `PersonalRadio` с рабочей батарейкой (`Battery9V`), **или**
* вы сидите в машине, и в её грузе лежит `PersonalRadio` с батарейкой
  (если сервер не выключил `requireRadioItemInCargo`).

## 5. Чек-лист после установки

- [ ] в клиентских логах `[MusicRadio] Client mission ready`
- [ ] в серверных `[MusicRadio] Server engine ready (protocol 1)`
- [ ] `Library loaded: N entries` — N больше нуля
- [ ] F7 в машине открывает меню (не «no radio in the vehicle cargo»)
- [ ] трек играет, громкость и радиус меняются
- [ ] второй игрок рядом слышит музыку, а на 100 м — уже нет
- [ ] повторный вход в игру: музыка продолжается с той же секунды
- [ ] после конца трека включается следующий из той же категории (`autoplayNext`)

## 6. Частые проблемы

| Симптом | Причина / решение |
|---------|-------------------|
| Меню не открывается, в чате «put a radio in the vehicle cargo» | нет `PersonalRadio` в грузе, или у рации нет батарейки |
| В чате «The radio is not ready: no cassette or disc inserted» | в рацию не вставлена кассета/диск (с 20261174 рация — плеер, см. `docs/15_TEST_IN_GAME.md`, шаг 4) |
| В чате «The radio is not ready: no memory card» | карта памяти не вставлена в кассету/диск |
| Треки в списке серые, «pack missing» | у клиента нет `@MusicRadioDB` или его версия старее серверной |
| `[Radio] Unknown track: ...` | сервер и клиент смотрят в разные индексы — обновите `@MusicRadioDB` и на сервере, и на клиенте |
| Музыка слышна всем на карте | проверьте `defaultRadius` и `maxRadius` в конфиге |
| После рестарта музыка не продолжается | это норма: сессия живёт в памяти сервера, см. `docs/02_ARCHITECTURE.md` |
| Сервер пишет `cannot open file` | нет папки `mpmissions/<map>/MusicRadio/` — создайте или дайте права на запись |
