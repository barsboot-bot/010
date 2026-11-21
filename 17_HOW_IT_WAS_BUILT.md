# Как этот мод был сделан: пошаговое руководство

Документ для того, кто хочет **повторить путь целиком** — от пустой папки до
рабочего мода с музыкой в машине, рации и кассетном плеере. Здесь нет теории
«как вообще модить DayZ»: здесь ровно то, что было проделано в этом проекте,
в том порядке, в котором это делалось, вместе с ошибками, которые пришлось
разгребать (и которые вы почти наверняка повторите).

---

## 0. Что получилось в итоге

Три отдельных мода, а не один:

| Мод | Кому | Что внутри | Почему отдельно |
|-----|------|------------|-----------------|
| `@MusicRadioCore` | сервер + клиент | 27 скриптов, `config.cpp`, UI (текстуры — в `@MusicRadioDB/data/tex`) | логика и предметы; меняется редко |
| `@MusicRadioDB` | сервер + клиент | `.ogg` + `CfgSoundShaders/SoundSets` + **классы кассет/дисков** | контент; меняется часто и **без пересборки Core** |
| `MusicRadioServer` | **только сервер** | 3 скрипта: права, лимиты, аудит | клиентам не нужен; лежит в `ServerMOD\Addons`, без подписи |

Главный принцип, который определил всю архитектуру:

> **Логика не знает путей к файлам.** Сервер отправляет `provider` + `trackId`,
> клиент сам собирает имя SoundSet: `MR_<PROVIDER>_<TRACKID>_SoundSet`.
> Поэтому музыку можно менять, не трогая код.

---

## 1. Подготовка окружения (30 минут, один раз)

1. **DayZ Tools** из Steam — там `AddonBuilder`, `Object Builder`, `TexView 2`,
   `DSSignFile`.
2. **Python 3.8+** (на Windows команда только `python`, не `python3`).
3. **ffmpeg** — для конвертации музыки в `.ogg`.
4. Смонтировать `P:` через DayZ Tools (`Mount P:`): на `P:\DZ\scripts` лежат
   ванильные скрипты — это **единственный достоверный справочник** по API.
   Всё, что «кажется, должно быть», проверять там, а не в интернете.

Что сразу завести в проекте:

```
MusicRadio/
├── @MusicRadioCore/     Scripts/{3_Game,4_World,5_Mission}/MusicRadio/ + config.cpp
├── @MusicRadioDB/       Music/ + Scripts/4_World/ + config.cpp
├── @MusicRadioServer/   Scripts/{4_World,5_Mission}/MusicRadio/ + config.cpp
├── tools/               python-скрипты (генерация, проверки, сборка)
├── server_files/        types.xml, cfgspawnabletypes, AudioConfig.json
├── docs/                документация
├── Models/              *.obj, tex_src/*.png, *.rvmat
└── *.bat                AUTO_BUILD / BUILD_PBO / DEPLOY / CHECK / REBUILD
```

---

## 2. Каркас: три `config.cpp`

### 2.1 `CfgPatches` — имя должно быть уникальным

```cpp
class CfgPatches
{
    class MusicRadioCore
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = { "DZ_Data", "DZ_Scripts" };
    };
};
```

`requiredAddons` — не украшение: если имя в списке не совпадает с реальным
`CfgPatches` чужого мода, ваш мод **молча** загрузится не в том порядке.

### 2.2 `CfgMods` + путь к скриптам

```cpp
class CfgMods
{
    class MusicRadioCore
    {
        dir = "MusicRadioCore";
        name = "MusicRadio - Core";
        type = "mod";
        dependencies[] = { "Game", "World", "Mission" };
        class defs
        {
            class gameScriptModule  { value=""; files[] = { "MusicRadioCore/Scripts/3_Game" }; };
            class worldScriptModule { value=""; files[] = { "MusicRadioCore/Scripts/4_World" }; };
            class missionScriptModule { value=""; files[] = { "MusicRadioCore/Scripts/5_Mission" }; };
        };
    };
};
```

**Грабли #1.** Папки, указанной в `files[]`, может не быть — движок ругнётся.
Держите `defs` в точном соответствии с реальными папками.

### 2.3 Порядок загрузки между своими модами

В `@MusicRadioDB/config.cpp`:

```cpp
requiredAddons[] = { "DZ_Data", "DZ_Sounds_Effects", "MusicRadioCore" };
```

Это гарантирует, что `MR_CassetteCase_Base` (объявлен в Core) уже существует,
когда DB объявляет наследников `MR_Cassette_Rock01`. Без этого — `Undefined
base class`.

---

## 3. Слои скриптов и **почему нельзя иначе**

| Слой | Что можно | Что нельзя |
|------|----------|------------|
| `3_Game` | `MR`, константы, JSON, трек, библиотека, типы пакетов | ссылаться на `PlayerBase`, `CarScript`, `Mission` |
| `4_World` | `MR_Registry`, хосты, `modded CarScript/PersonalRadio/PlayerBase` | ссылаться на `MissionGameplay`, виджеты |
| `5_Mission` | RPC-обработчики, меню, хуки миссии | — |

**Грабли #2 (реальные из логов проекта).** `PlayerBase` из `3_Game` — не
компилируется. `MR_ClientRPC` из `3_Game` — тоже. Оба случая поймал
`check_project.py`, а не игра.

Правило: **`3_Game` не знает о `4_World` и `5_Mission` никогда.** Направление
зависимостей — только вниз.

---

## 4. Протокол: как сервер говорит клиенту «играй вот это»

Никогда не отправляйте путь к файлу. Отправляйте **локатор**:

```
LCT|1|MusicRadioDB|wasteland_01|6|48311
     │  │            │          │ └ seed
     │  │            │          └ громкость 0..10
     │  │            └ trackId
     │  └ provider
     └ версия протокола
```

Клиент собирает `MR_MUSICRADIODB_WASTELAND_01_SoundSet`, **проверяет** его через
`GetGame().ConfigIsExisting("CfgSoundSets ...")` и только потом играет. Нет
сета — трек в меню серый, с подписью `pack missing`, в чат приходит понятная
причина. Никаких «тихих» отказов.

RPC-id держите в одном месте и вне ванильного диапазона:

```cpp
static const int RPC_BASE       = 784100;
static const int RPC_CONFIG     = 784101;   // server -> client
static const int RPC_PLAY       = 784103;
static const int RPC_REQUEST    = 784107;   // client -> server
```

**Грабли #3.** `Mission`/`DayZGame` **не имеют** `OnRPC`. Клиент→сервер пакеты
надо принимать в `modded class PlayerBase { override void OnRPC(...) }` — это
единственная точка, которая есть на обеих сторонах.

**Грабли #4 (Enforce Script).** `case` не создаёт область видимости, поэтому
`string payload;` в двух ветках `switch` = `Multiple declaration`. Объявляйте
переменные **до** `switch`. Тернарник `?:` в аргументах и перенос выражения
(`&&` в конце строки) тоже не перевариваются: `Broken expression`, `строка
кончается на '&'`. Один оператор = одна строка.

---

## 5. Модель данных: предметы и носители

### 5.1 Что где объявляется

```
CfgSlots        — слоты: MR_Cassette, MR_MiniDisc, MR_MemoryCard, MR_DashSlot
CfgVehicles     — предметы: карта, кассета, диск, плеер, дека
```

**Грабли #5 — «самая дорогая» ошибка проекта.** Внутри **одного** `CfgVehicles`
класс нельзя объявить дважды, а «дополнить чужой класс» конструкции нет:

```cpp
// ТАК НЕЛЬЗЯ для ванильной машины/рации:
class CivilianSedan : CarScript { attachments[] += { "MR_DashSlot" }; };
//  -> /CfgVehicles.CivilianSedan: Member already defined  -> мод не грузится
```

Правильно — **дописывать к прямому родителю** (`+=` добавляет к массиву,
объявленному на уровне прямого парента):

```cpp
class Transmitter_Base;
class PersonalRadio : Transmitter_Base
{
    attachments[] += { "MR_MemoryCard", "MR_Cassette", "MR_MiniDisc" };
};
```

А там, где прямого парента «нельзя тронуть» без риска (ванильные машины),
слот добавляют **только через `CfgSlots`** — предмет в него всё равно
вставляется, просто не рисуется на модели. Именно так сделан `MR_DashSlot`.

### 5.2 Носитель = ключ альбома, а не контейнер

Кассета ничего не хранит. Имя класса — это id альбома:

```
MR_Cassette_Rock01  ->  AlbumOf() -> "rock01"  ->  треки из index.json
```

Один носитель = один альбом. Позиция «где остановились» пишется в
`OnStoreSave/OnStoreLoad` самого предмета (`MR_CarrierBase`), а не в сеть.

**Грабли #6.** `OnStoreSave` обязан писать поле **всегда**, даже пустым:
`OnStoreLoad` сопоставляет поля по позиции, и «пропущенная строка» тихо
портит сохранённые данные.

### 5.3 Классы: `class` vs `modded class`

* свой предмет из `config.cpp` → в скриптах `class MR_CassetteCase_Base extends MR_CarrierBase`;
* ванильный класс (`PersonalRadio`, `CarScript`, `PlayerBase`) → `modded class`.

**Грабли #7.** `modded class` на **своём** классе = `Unknown type`. И наоборот:
`override` в классе без реального скриптового родителя = `Function 'EEInit' is
marked as override, but there is no function with this name in the base class`
— и падение сервера в `ACCESS_VIOLATION`. Конфиг-родитель (`Inventory_Base`)
скриптам родителя **не даёт**: класс в скриптах должен наследовать `ItemBase`.

---

## 6. Единый контракт готовности (то, к чему пришли в 20261174)

Одна проверка на **все** устройства — `MR_MediaBay`:

```
носитель (кассета/диск)  ->  карта памяти  ->  батарейка  ->  можно играть
```

Используют: кассетный плеер, MD-плеер, **рация**, кассета в слоте машины.
Причина в чате всегда конкретная:

| Сообщение | Чего не хватает |
|-----------|-----------------|
| `no cassette or disc inserted` | носителя |
| `no memory card` | карты памяти ( она вставляется **в кассету** ) |
| `no battery` | батарейки |

Правило, которое себя оправдало: **игрок никогда не должен гадать, почему
тихо.** Каждый отказ — строка в чат с причиной.

---

## 7. Генерация контента, а не ручное редактирование

Руками `config.cpp` с музыкой не правят. Цепочка:

```
трек .mp3/.wav
   │ tools/import_music.py (ffmpeg -> .ogg, обложка, json)
   ▼
@MusicRadioDB/Music/<Genre>/<track>.ogg + <album>.json
   │ tools/generate_db_config.py
   ▼
@MusicRadioDB/config.cpp:  CfgSoundShaders + CfgSoundSets
                           + классы MR_Cassette_<Album> / MR_MiniDisc_<Album>
                           + index.json + carriers.json + types-сниппет
   │ tools/build_carrier_cfgset.py
   ▼
CfgCassette.soundSets[] у каждого класса носителя
```

Почему классы носителей переехали в **DB** (решение 20261173): добавили
альбом — пересобирается только DB, Core остаётся прежним. Условие
безопасности — `requiredAddons = "MusicRadioCore"` и базовые классы в Core
**выше** наследников.

---

## 8. Конвейер проверок (главное, что экономит вечера)

Каждый скрипт — самостоятельный, запускается отдельно:

| Скрипт | Что ловит |
|--------|-----------|
| `check_project.py` | баланс скобок, дубли классов, нарушение слоёв, дубль `CfgVehicles`, `class X:CarScript` для ванильных машин, несуществующие методы движка, `ref EffectSound`, контракт `index.json ↔ config.cpp ↔ .ogg`, виджеты из `.layout` |
| `check_db_config.py` | то, из-за чего падает `AddonBuilder` на DB |
| `check_tools.py` | `.bat` и `.py` из одной ревизии |
| `check_archive.py` | диск и архив — одной ревизии |
| `check_fresh.py` | PBO на сервере действительно свежие (по содержимому, а не по времени) |
| `check_pbo.py` | что реально лежит внутри собранного PBO |
| `check_bat.py`, `check_log.py`, `doctors.py` | батники, разбор лога игры, сводная диагностика |
| `selftest_*.py` (16 шт.) | сами проверки: каждый чекер тестируется негативным примером |
| `stage_pbo.py` | раскладка файлов так, как они лягут в PBO (+ `--zip`) |
| `autobuild.py` | весь конвейер одной командой |

**Правило проекта:** если ошибка из лога есть в текущем `.c` — это наша
ошибка, чинится в коде. Если проверка не имеет негативного теста — она
считается несуществующей.

---

## 9. Сборка PBO и подпись

1. `python tools/generate_db_config.py` — пересобрать базу.
2. `python tools/stage_pbo.py --clean` — staging без старых файлов.
3. `BUILD_PBO.bat` — находит `AddonBuilder`, собирает `release/pbo/*.pbo`.
4. Ключ `KRaTos.biprivatekey` из `DSSignFile` → `release/keys/`:
   **Core и DB подписываются**, `MusicRadioServer` — **нет**.
5. `DEPLOY.bat` — раскладка на сервер.

Раскладка на сервере (проверено на живом сервере):

```
@MusicRadioCore/Addons/MusicRadioCore.pbo (+ .bisign)
@MusicRadioDB/Addons/MusicRadioDB.pbo     (+ .bisign)
ServerMOD/Addons/MusicRadioServer.pbo     (без подписи)
keys/KRaTos.bikey
```

Запуск:

```
-mod=@KGB.LIB;@CF;...;@MusicRadioCore;@MusicRadioDB;@VPPAdminTools
-servermod=ServerMOD;ServerCARS
```

**`ServerCARS` — чужая папка** (фиксы машин). Наш мод туда не кладём никогда:
только `ServerMOD\Addons`. И `@MusicRadioServer` не подписываем — у него нет
пары «ключ/подпись».

---

## 10. Порядок работ, который сработал

1. **Скелет трёх модов** + `CfgPatches`/`CfgMods` → собрать пустые PBO,
   убедиться, что мод виден в лаунчере.
2. **`4_World`-хуки** (`modded CarScript/PersonalRadio`) + реестр хостов.
3. **Протокол RPC** (локатор, 10 id'ов) и клиентский резолвер звука.
4. **UI** (меню на `.layout`, F7/F8) — без него нечего проверять.
5. **Носители**: `CfgSlots`, кассета/диск/карта, `CfgCassette`.
6. **Генератор базы** (json → `config.cpp` → `.ogg`-контракт).
7. **Серверная часть**: права, лимиты, громкость, аудит.
8. **Слот в машине** (`MR_DashSlot`) и **рация как плеер** — уже поверх
   готового контракта.
9. **Модели/текстуры** (`obj → p3d`, `png → paa`) — последними, потому что
   без них всё уже играет.
10. **Проверки и батники** — параллельно каждому шагу, а не «потом».

---

## 11. Чек-лист «делай так с самого начала»

- [ ] Один класс — один файл слоя; имя класса в реестре `check_class_homes`.
- [ ] `3_Game` не ссылается на `4_World`/`5_Mission`.
- [ ] Ванильные классы — только `modded`; свои — только `class ... extends`.
- [ ] `attachments[] +=` — **только** от прямого парента.
- [ ] Ни одного `ref EffectSound` — звуком владеет движок.
- [ ] Каждая проверка имеет негативный `selftest`.
- [ ] `--clean` перед каждой сборкой PBO.
- [ ] `.bat` и `.py` всегда из одного архива/ревизии.
- [ ] Ревизия (`20261174`) поднимается в один день с правкой кода и печатается
      в `check_tools.py`, `stage_pbo.py`, README архива.
- [ ] После правки `.c` — `stage_pbo.py --clean`, иначе чекер честно скажет
      `staging is out of date`.

---

## 12. Куда смотреть дальше (в этом же проекте)

| Документ | О чём |
|----------|-------|
| `02_ARCHITECTURE.md` | почему три мода, как летят пакеты |
| `03_MEDIA_PACK.md` | как добавить альбом |
| `06_CARRIERS.md` | носители, слоты, контракт «один альбом = один предмет» |
| `07_MODELS.md` | `obj → p3d`, LOD'ы, селекции, прокси для торпеды |
| `09_TEXTURES.md` | `png → paa`, `.rvmat`, DXT1/DXT5 |
| `11_TROUBLESHOOTING.md` | все реальные ошибки игры и их причины |
| `13_CHECK_BUILD.md` | журнал по ревизиям: что и почему менялось |
| `14_BAT_FILES.md` | назначение каждого `.bat` |
| `15_TEST_IN_GAME.md` | как проверить в игре по шагам |
| `16_TODO.md` | что осталось |

---

## 13. Главные выводы

1. **Контент отдельно от кода.** Медиапак обновляется без пересборки Core.
2. **Проверки важнее фич.** Половина ошибок этого проекта была поймана
   чекерами до запуска игры, а не логами.
3. **Не догадываться про API.** `P:\DZ\scripts` — истина; «на форуме писали» —
   нет.
4. **Один оператор — одна строка.** Enforce Script не прощает переносов.
5. **Причина в чат.** Любой отказ игроку объясняется словами, а не тишиной.
