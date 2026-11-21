# Проверка сборки — что делать на Windows

Ревизия инструментов **20261174**. Проверка, что tools действительно новые: `python tools\check_tools.py` — он печатает таблицу «файл → ревизия». Что изменилось с прошлой сборки — `docs/12_CHANGES_VOS.md`.

Новое в 20261146: сверка «в PBO столько же файлов, сколько в staging»
(`check_pbo.py` печатает ещё и `скриптов .c: N`) и проверка `displayName` /
`model` у каждого класса CfgVehicles.

Новое в 20261145: `CHECK.bat` больше не пишет «всё чисто», если в последнем
логе сервера есть наши ошибки компиляции, и сам сверяет их с исходниками:
строка `Can't find variable 'TAPE_MARKER'` при живой константе в
`MR_Constants.c` = **компилируется старый `.pbo`**. Дампы `.mdmp` ищутся и в
`instance\`, а не только рядом с exe.

Новое в 20261144: проверка «`MR.КОНСТАНТА` объявлена в `class MR`» и сверка
**скриптов** внутри `.pbo` на сервере (раньше сверялся только `config.cpp` —
именно поэтому CHECK говорил «всё чисто», пока сервер падал на старом скрипте).

Новое в 20261143: `CHECK.bat --mdmp` — разбор дампа падения (`.mdmp`) без WinDbg;
`modded class` на своих классах; один класс в одном файле.

---

## Ревизия 20261174 — рация стала таким же плеером, как дека

Решение пользователя: **единый контракт** — рация и дека ведут себя одинаково,
карта памяти нужна и рации, и деке.

### Что добавлено в конфиг

`@MusicRadioCore/config.cpp`:

```cpp
class Transmitter_Base;
class PersonalRadio : Transmitter_Base
{
    attachments[] += { "MR_MemoryCard", "MR_Cassette", "MR_MiniDisc" };
    class GUIInventoryAttachmentsProps
    {
        class MR_MusicMedia { ... attachmentSlots[] = { карта, кассета, диск }; };
    };
};
```

Два важных момента:

1. **Повторён прямой парент `Transmitter_Base`.** У ванильной рации
   `attachments[]` (в нём `BatteryD`) объявлен на уровне прямого родителя, а
   `+=` дописывает именно к нему. Если написать `class PersonalRadio :
   ItemBase`, будет `/CfgVehicles.PersonalRadio: Member already defined` —
   та же ошибка, что была с машинами.
2. **`BatteryD` не перечислен** — он приходит по наследству, `+=` только
   добавляет три наших слота.

### Что добавлено в скрипты

* Новый `MR_MediaBay` (`MR_Decks.c`) — **одна** проверка «что вставлено» для
  всех устройств: `Carrier / AnyCarrier / HasCard / HasBattery / IsReady /
  Album`. Порядок причин: носитель → карта → батарейка.
* `MR_DeckBase.MR_IsReady()` / `MR_HasMemoryCard()` / `MR_CurrentAlbum()` теперь
  вызывают `MR_MediaBay` (поведение то же, код один).
* `modded class PersonalRadio` (`MR_MediaItems.c`): `MR_InsertedCarrier()`,
  `MR_GetInsertedCassette()`, `MR_GetInsertedDisc()`, `MR_GetMemoryCard()`,
  `MR_HasBattery()`, `MR_IsReady()`, `MR_CurrentAlbum()`, `MR_MediaStatus()`.
* `MR_ItemHost.MR_CanPlay()/MR_BlockReason()` — рация проверяется как плеер.
* Сервер: `ValidateHost` для `HOST_ITEM` требует готовности рации
  (`The radio is not ready: …`), `ACT_PLAY_ALBUM` берёт альбом из рации,
  `Tick` глушит сессию, если кассету/карту вынули на ходу.
* `MR_DeckState.FindCarrier` знает про рацию и про кассету, лежащую в
  `MR_DashSlot` — позиция «где остановились» теперь запоминается и там.
* Меню: вкладка **TAPES** для `HOST_ITEM` показывает альбом из рации
  (`MR_HeldAlbum()`).

### Настройка

`features.radioNeedsCarrier` (по умолчанию `true`). `false` возвращает старое
поведение — рация играет любой трек без носителя.

### Проверки

`tools/selftest_no_keys_in_servermod.py → check_radio_slots()`: падает, если
у `PersonalRadio` нет слотов, если объявлен не от `Transmitter_Base`, или если
дека/рация перестали использовать `MR_MediaBay.IsReady()`.
Негативные тесты прогнаны: подмена парента → `FAIL`, удаление
`attachments[] +=` → `FAIL`, восстановление → `OK`.

## Ревизия 20261173 — кассета в машину и @MusicRadioDB отдельно

Два запроса, обе части в одной сборке.

### 1. В инвентаре машины появился слот под кассету

Правило «слот виден, если он объявлен в `CfgSlots`» сработало и здесь:

* `@MusicRadioCore/config.cpp` → `CfgSlots → Slot_MR_DashSlot`
  (`name = "MR_DashSlot"`, `selection = "MR_DashSlot"`). Рядом со слотом
  карты памяти.
* В слот кладут **кассету, мини-диск или целый плеер** —
  `MR_MediaItems.IsCarDockItem()` (новый) = носитель ИЛИ плеер.
* `MR_MediaItems.AlbumInVehicle()` теперь умеет брать альбом **у самой
  кассеты** (`AlbumOf(type)`), а не только у пристыкованного плеера.
* `MR_VehicleStatusText()` пишет `docked cassette: album 'rock01'`.
* `MR_VehicleHost.MR_CanPlay()` и `ValidateHost()` (сервер) считают любую
  вещь в слоте источником музыки, поэтому правило «нужна рация в багажнике»
  для такой машины больше не применяется.
* Один слот на всё: что положили — то и играет.

### Почему в конфиге НЕТ ни одной машины

Первый заход был очевидным: `class CivilianSedan : CarScript { attachments[] +=
{ "MR_DashSlot" }; };`. Так делать нельзя — в Enforce-конфиге нет
«дополнить существующий класс»: `class X : Родитель` создаёт **новое**
определение, и парсер видит два члена с одним именем:

```
File MusicRadioCore\config.cpp, line 338:
/CfgVehicles.CivilianSedan: Member already defined.
```

Именно поэтому раздел CAR DASH когда-то был вырезан из генератора. Теперь он
вырезан по-другому: машин в конфиге нет вообще, слот живёт в `CfgSlots`.
Ловит это `tools/selftest_no_keys_in_servermod.py → check_car_dash_slot()`
(негативный тест: вернул строку — `FAIL - config.cpp снова определяет
CivilianSedan : ...`).

Что осталось: **предмет в слоте не виден на торпеде** — для этого нужна
селекция `MR_DashSlot` в `.p3d` машины и прокси-модель (`docs/07_MODELS.md`,
п. 6). Вставить, вынуть и слушать — работает.

### 2. @MusicRadioDB обновляется отдельно

* Классы носителей (`MR_Cassette_*`, `MR_MiniDisc_*`) генерируются теперь в
  `@MusicRadioDB/config.cpp` (`generate_db_config.py → write_db_carriers()`), а
  не в Core. `strip_core_carriers()` один раз убирает их из Core и дальше
  ничего не делает.
* В Core остались только базовые классы (`MR_CassetteCase_Base`,
  `MR_MiniDisc_Base`, `MR_MemoryCard_Base`) и проигрыватели.
* У `MusicRadioDB` в `CfgPatches` добавлен `requiredAddons = "MusicRadioCore"`,
  чтобы базовые классы гарантированно грузились раньше наследников.
* Core больше не ссылается на имена SoundSet медиапака: у бейз-кассеты
  `soundSets[] = {}`, имя трека выводится по контракту
  `MR_<PROVIDER>_<TRACKID>_SoundSet`. Значит, добавили/убрали альбом —
  пересобирается только DB.
* `tools/build_carrier_cfgset.py` пишет `CfgCassette.soundSets[]` в
  `@MusicRadioDB/config.cpp` (был Core).

Итог: `tools\import_music.bat` (или `python tools\generate_db_config.py`) →
`tools\stage_pbo.py --clean` → `BUILD_PBO.bat` пересобирает **только**
`MusicRadioDB.pbo`, Core можно не трогать.

## 1. Распаковать и пересобрать (3 шага)

```
1) MusicRadio_ARCHIVE.zip  ->  распаковать в папку MusicRadio (с заменой)
2) REBUILD.bat             ->  текстуры, config.cpp, staging, 33 проверки
3) BUILD_PBO.bat           ->  AddonBuilder + подпись (нужен DayZ Tools)
```

`REBUILD.bat` в конце сам печатает итог `check_project.py`. Ожидаемо:
**`OK - no errors, 0 warning(s)`**, 33 проверки. Ниже — что именно проверять в игре.

Если `REBUILD.bat` пишет «набор скриптов не тот, что ждут батники» — распаковался
не целиком, повторить распаковку с заменой.

---

## 2. Главный тест: кассета помнит место

Это то, ради чего всё делалось. Порядок:

| шаг | что сделать | что должно быть |
|---|---|---|
| 1 | заспавнить `MR_CassettePlayer` + `MR_Cassette_Rock01` + батарейку 9V + карту памяти | плеер собирается, все три слота видны |
| 2 | вставить кассету, включить | играет 1-й трек |
| 3 | дождаться ~30 секунд, вынуть кассету | **кассета не вынимается**, пока играет |
| 4 | выключить плеер, вынуть кассету | вынимается |
| 5 | вставить обратно, включить | играет **с того же места**, а не с 0:00 |
| 6 | отдать кассету второму игроку, он вставляет в свой плеер | продолжает с того же места |
| 7 | перезапустить сервер, вставить кассету | место сохранено |
| 8 | вынуть кассету на середине, подождать 2 минуты, вставить | позиция та же (время не «идёт» без плеера) |

В логе сервера при загрузке кассеты должно появиться:

```
[MusicRadio] Tape resumes at: <trackId> +<секунды>s
```

Если позиция всегда `+0s` — смотреть `MR_DeckState.Tick()` в `MR_ServerEngine.Tick()`
и `GetGame().IsServer()` внутри `MR_TapeSetPosition`.

---

## 3. Что ещё проверить (по одной минуте)

**Звук на плеере вообще.** Раньше его не было: плеер регистрировался как `HOST_PLAYER`,
а клиент умел резолвить только `HOST_ITEM`/`HOST_VEHICLE`. Теперь при включении должен
играть звук, а не тишина. Если тишина — в клиентском логе искать
`PLAY: host not in this client's bubble` (значит ветка `HOST_PLAYER` в
`MR_ClientRPC.ResolveHost()` не сработала).

**Клавиши.** Настройки → вкладка **MusicRadio**: три действия
(`Next track`, `Previous track`, `Open music player`), дефолты `→`, `←`, `B`.
Нажать `J` — должна открыться **карта**, а не радио (это и была ошибка всех трёх модов).

**Локализация.** Переключить язык игры на русский: названия предметов и слотов должны
стать русскими (`Кассета`, `Карта памяти`, `Кассетный плеер`). Если видно `$STR_MR_...` —
`stringtable.csv` не подхватился (проверить, что файл в PBO: `inspect_pbo.py`).

**Громкость.** Дека должна звучать примерно вдвое тише, чем машина/рация на той же
настройке громкости (потолок 0.5 в `MR_DeckHost.MR_GetVolumeCeiling()`).

**Поза в руках.** Взять плеер в руки — держится как рация/кассета, а не как кирка.
Если как кирка — `animClass` не подхватился или `ModItemRegisterCallbacks` не вызвался.

**Перемотка.** `ACT_SEEK` реализован на сервере, но UI-кнопки перемотки пока нет —
проверять нечего, это следующий шаг.

---

## 4. Что заведомо НЕ готово (не считать багом)

| | почему |
|---|---|
| **иконки слотов пустые** | `ghostIcon = "missing"`: нужна `.edds`, её делает только TexView 2 на Windows. См. `docs/09_TEXTURES.md` |
| **текстуры белые** | 36 PNG готовы, `.paa` ещё не сконвертированы (`TexView 2`) |
| **кассета не видна в плеере** | у ванильных моделей нет прокси; нужна модель с `proxy:\dz\gear\radio\cassette.001` (рецепт в `VOS_PART7_CASSETTES.md` §4) |
| **слота в машинах нет** | `MR_DashSlot` вынесен из `config.cpp` намеренно: `check_class_redeclaration` запрещает `class CivilianSedan : CarScript` |
| **зомби не слышат музыку** | `CfgNoises` — из OMD, план в `OMD_PART6_SOURCE.md` §10.1, нужен баланс шума |
| **своя модель плеера** | пока ванильный `WalkieTalkie.p3d`, классы при замене не меняются |

---

## 5. Если что-то упало

`CHECK.bat` собирает всё: RPT рядом с exe, логи модулей, staging, PBO.
Прислать:

* вывод `REBUILD.bat` целиком (не только последние строки);
* `build/report.txt`;
* `release/report_stage.txt`;
* RPT сервера и клиента с моментом входа в мир.

Первая строка, которую стоит искать в RPT: `SCRIPT (E)` — это ошибка нашего кода,
номер файла и строки в ней настоящие (в отличие от диалога игры).

## Падение сервера после «Can't compile "World" script module!» (ревизия 20261172)

Симптом в RPT:

```
MusicRadioCore/Scripts/4_World/musicradio/mr_decks.c(76): Function 'EEInit' is marked as override,
    but there is no function with this name in the base class
...
Can't compile "World" script module!
DayZServer_x64_2026-11-11_19-19-35.mdmp   ACCESS_VIOLATION
```

Причина: классы носителей и плееров были объявлены **без `extends`**. `config.cpp`
даёт предмету конфиг-родителя, но не базовый класс скрипта, поэтому `override`
нечего перекрывать, а `super.` некому передавать. Правка:

| класс | было | стало |
|---|---|---|
| `MR_CassetteCase_Base`, `MR_MiniDisc_Base`, `MR_MemoryCard_Base` | `class X` | `class X extends MR_CarrierBase` (`extends ItemBase`) |
| `MR_CassettePlayer_Base`, `MR_MiniDiscPlayer_Base` | `class X` | `class X extends MR_DeckBase` (`extends ItemBase`) |

Плюс `class EnergyManager` в config.cpp обоих плееров: без него `OnSwitchOn`/
`OnSwitchOff` не вызываются никогда.

Проверки (обе входят в `AUTO_BUILD.bat`, раздел 2b, и в ИТОГ):

* `check_project.py` → `check_script_overrides` — ERROR с текстом ошибки
  компилятора, если у класса нет родителя, а внутри `override`/`super.`;
  отдельно — если `extends X`, а класса `X` нет.
* `check_project.py` → `check_deck_config` — у spawn-класса плеера (scope = 2)
  нет `class EnergyManager` ни у него, ни у предков.
* `tools/selftest_override.py` — ломает `extends` в `MR_Decks.c`, проверяет, что
  проверка краснеет, и возвращает файл. Запускается в `doctors.py`.

Важно: `check_pbo` и `check_fresh` такой баг **не видят** — они сравнивают
файлы, а не семантику. «PBO = staging = сервер» не значит «компилируется».

## Подпись: MusicRadioServer — без ключа (ревизия 20261172)

| мод | где живёт | подпись |
|---|---|---|
| `MusicRadioCore` | клиент + сервер (`@MusicRadioCore\Addons`) | `.bisign` ключом |
| `MusicRadioDB` | клиент + сервер (`@MusicRadioDB\Addons`) | `.bisign` ключом |
| `MusicRadioServer` | только сервер (`-servermod=ServerMOD`, `ServerMOD\addons`) | **нет** |

Почему: серверный мод не входит в клиентский мод-лист, подпись ему не нужна, а
оставшийся от прошлых сборок `.bisign` рядом с ним заставляет `verifySign`
проверять не тот ключ.

Где закреплено:

* `tools/autobuild.py` → `UNSIGNED_MODS = ("MusicRadioServer",)` — `-sign=` в
  команду AddonBuilder не попадает, при `--deploy` старые `.bisign` удаляются из
  `release\pbo` и с сервера;
* `BUILD_PBO.bat` → в `:pack` для этого мода `MRSIGN` пустой, после сборки
  удаляются `MusicRadioServer.pbo.*.bisign`;
* `DEPLOY.bat` → для серверного мода подпись не копируется, старая удаляется
  (`:copy_one`, `:copy_extra`);
* `check_project.py` → `check_server_signature`: найденный `.bisign` = ERROR;
* `tools/selftest_unsigned.py` → эмулятор AddonBuilder + deploy, проверяет, что
  `-sign=` нет у серверного мода и есть у Core/DB.

Проверить руками на своей машине: `python tools\check_pbo.py` — строка
`подпись : нет (так и задумано: серверный мод не подписывается)`.

## Строка запуска сервера: `-mod` vs `-servermod` (ревизия 20261172)

Раскладка сервера с одним серверным модом и двумя клиентскими:

```
StartServer.bat ... "-servermod=ServerMOD;ServerCARS" "-mod=@KGB.LIB;...;@MusicRadioCore;@MusicRadioDB;@VPPAdminTools"
```

| мод | где лежит | в какой строке | подпись |
|---|---|---|---|
| `MusicRadioServer` | `ServerMOD\addons` | `-servermod` | нет |
| `MusicRadioCore` | `@MusicRadioCore\Addons` | `-mod` | `.bisign` |
| `MusicRadioDB` | `@MusicRadioDB\Addons` | `-mod` | `.bisign` |

`check_fresh.py` → `check_launch_chain()` читает батники в папке сервера (и на
уровень глубже), разбирает оба вида записи (`"-mod=@A;@B"` и `-mod="@A;@B"`) и
проверяет:

* `@MusicRadioCore` и `@MusicRadioDB` есть в `-mod` — иначе ERROR «клиенты мод
  не получат» (в игре не будет ни кассет, ни звука, ни меню);
* папка каждого мода из `-mod` реально существует на сервере;
* `MusicRadioServer` **не** стоит в `-mod` (он локальный, его место —
  `-servermod`) — иначе warning;
* нашего файла нет в `ServerCARS` (**чужая папка**: там фиксы машин
  `byALEX_ServerMod` и др. со своими `.bisign`). Нашлись — ERROR с командой
  `del`. Наш мод живёт только в `ServerMOD\addons`;
* копия в папке, которой нет ни в `-mod`, ни в `-servermod`, — заметка:
  сервер её не читает, а правят руками обычно именно её.

Тест разбора (ваша строка как образец): `python tools\selftest_launch.py`.

## ServerCARS — не наша папка (ревизия 20261172)

`ServerCARS\addons` на сервере занята фиксами машин (`byALEX_ServerMod` и
подобные, у каждого свой `.bisign`). Наш `MusicRadioServer` кладём **только в
`ServerMOD\addons`** — оттуда его и берёт `-servermod=ServerMOD;ServerCARS`.

Что изменено:

| файл | было | стало |
|---|---|---|
| `tools/autobuild.py` | `target_dirs` возвращал `ServerCARS\addons` третьим адресом | только `ServerMOD\addons` (+ устаревшая `@MusicRadioServer\Addons`); при `--deploy` наша старая копия из `ServerCARS` **удаляется** |
| `DEPLOY.bat` | `call :copy_extra "%SRV%\ServerCARS\addons"` | вместо копирования — `:drop_cars` (удаляет наш `.pbo`/`.bisign` из `ServerCARS`, чужие файлы не трогает) |
| `tools/check_fresh.py` | искал наш `.pbo` и в `ServerCARS` | ищет только в `ServerMOD`; найденный в `ServerCARS` наш файл = **ERROR** с командой `del` |
| `tools/check_project.py` | — | `check_foreign_dirs`: копирование в `ServerCARS` из скриптов/батников = ERROR |
| `tools/selftest_cars.py` | — | тест: раскладка кладёт `.pbo` только в `ServerMOD`, старая копия удаляется, чужие файлы фиксов остаются целы |

Проверка на живом макете: в `ServerCARS\addons` положили
`byALEX_ServerMod.pbo`, `MotorHome_byALEX_ServerMod.pbo`, их `.bisign` и нашу
старую копию → после раскладки в папке остались только чужие файлы, наш `.pbo`
появился в `ServerMOD\addons`.

## Лог 20:01 — три разные вещи в одном отчёте (ревизия 20261172)

`AUTO_BUILD.bat` и `REBUILD.bat` из 20:01 показали три симптома. Два были
ложными, один — настоящим.

| симптом из лога | что это было | что сделано |
|---|---|---|
| `PBO не читается (... 'config.cpp' не совпал с исходником ...)` на MusicRadioCore и MusicRadioDB | сломанный разборщик: staging-индекс складывал пути всех модов в один список по имени файла, и `config.cpp` от Core «совпадал» с `config.cpp` от DB | индекс ключуется по `(имя, мод)`; плюс второй, структурный проход: если содержимое отличается, PBO **читается** и пишется «собран из старых файлов», а не «не читается» |
| `исходник новее staging: MR_Decks.c` сразу после `stage_pbo --clean` | `TOLERANCE` = 2 с: на сетевом диске `P:\` и при распаковке архива mtime округляется | `TOLERANCE` = 3 с, комментарий с причиной |
| `AUTO_BUILD`: «Есть проблемы» вместе с «—deploy раскладывает» | раздел 4 отвечает про сервер, а вердикт считал это поломкой проекта | расхождения **с сервером** больше не валят ИТОГ: это работа для `--deploy`; валят только реальные ERROR |
| `textures: album carrier art is missing for mr_cassette_uns.rvmat` | `REBUILD.bat` рисовал обложки ДО `generate_db_config.py`, а новые альбомы появлялись после | шаг **2b/4**: `make_textures.py --albums` после генерации базы |

Проверки на это: `python tools\selftest_multiroot.py` (три PBO с РАЗНЫМИ
`config.cpp` — каждый читается и отдаёт свой) и `selftest_pbo.py` (9 вариантов
заголовка).

## Лог 20:12 — «исходник новее staging» и «config.cpp ДРУГОЙ» на верной сборке

Проект на сетевом диске `P:\`. Два сообщения выглядели как поломка, хотя
`config.cpp` в PBO совпадал побайтово, а скрипты — тоже:

```
! MusicRadioCore  staging: есть  pbo: есть
    -> исходник новее staging: MR_Decks.c
- MusicRadioCore  config.cpp в PBO ДРУГОЙ (старая сборка), 399 строк
```

Причина: **время файла**. Правка байтов в существующем файле на Windows
сохраняет время создания, а `selftest_override.py` перезаписывал `MR_Decks.c`
и тем самым «омолаживал» его (замерено: +549 с за один прогон).

Что изменено (ревизия 20261172) — сначала содержимое, потом время:

| место | правило |
|---|---|
| `check_fresh` (исходник → staging) | если staging побайтово равен исходнику, «исходник новее» не считается рассинхроном (строка-пояснение в выводе) |
| `check_fresh` (staging → PBO) | `_pbo_matches_staging`: `config.cpp` и все `.c` из PBO сравниваются с проектом; совпали — PBO собран из этих файлов, mtime не важен |
| `selftest_override.py` | возвращает `MR_Decks.c` его прежний mtime |
| `check_project.check_file_bandwidth` | предупреждение, если содержимое совпадает, а время файла «убежало» (так выглядит `copy /b` и тесты) |
| `autobuild.finish` | отдельный вердикт: «Проект собран правильно, но на сервере прошлая сборка → `AUTO_BUILD.bat --deploy`»; и «PBO НЕ СОБРАНЫ» вместо ложного «согласовано» |

Тест: `python tools\selftest_mtime.py` — «состаривает» корректный PBO на час и
проверяет, что проверка всё равно говорит «собран из этих файлов», плюс
контролирует, что `selftest_override` не сдвигает mtime.

## Лог 20:27 — пустой раздел 4 и ложная ошибка про `-mod`

В отчёте раздел «4. ЧТО КОМПИЛИРУЕТСЯ НА СЕРВЕРЕ» выглядел так:

```
  ERROR Start.bat: в -mod указан @MusicRadioCore, но папки
        E:\DayZServer\@MusicRadioCore нет - сервер не запустится с этой строкой
  ERROR Start.bat: в -mod указан @MusicRadioDB, но папки ... нет
  2 error(s) в строке запуска сервера (-mod/-servermod)
```

Список «сервер: … MusicRadioCore СТАРЫЙ на сервере» пропал целиком. Две
причины, обе исправлены (ревизия 20261172):

| причина | было | стало |
|---|---|---|
| батником запуска считался любой `.bat` со словом `-mod` | `KEY.BAT` («ключи от машины») и `BACKUP.bat` попадали в разбор, у них «нет папок `@MusicRadioCore`» | `looks_like_launch()`: нужны настоящие флаги (`-config=`, `-mission=`, `-port=`, `-servermod=`, `dayzserver`) |
| отсутствие папки мода считалось ошибкой | «сервер не запустится» — при том, что мод просто **ещё не разложен** | «ещё не разложено: @MusicRadioCore, @MusicRadioDB → `AUTO_BUILD.bat --deploy`» — заметка, код возврата не меняет |
| ошибки печатались с ранним `return` | раздел «запуск сервера» терялся, а вместе с ним и список PBO на сервере | строка запуска печатается **до** любых выходов, затем обычный раздел про сервер |

ERROR остаётся только для настоящей поломки: папка мода **есть, но в другом
месте** (`find_mod_copies`) — тогда строка запуска действительно смотрит в
пустоту.

Тест: `python tools\selftest_launch_scope.py` — подкладывает `KEY.BAT` и
`BACKUP.bat`, проверяет, что они не считаются запуском, и что отсутствие папки
даёт заметку, а не ошибку.

## Лог 20:47 — «668 ошибок, наших 21»: пять настоящих причин

RPT `DayZServer_x64_2026-11-11_20-47-19`: `Can't compile`, стадия «компиляция
скриптов прервана». Компилятор показал 8 строк, а причин пять, и две из них
проверяются статически (теперь это `python tools\check_project.py`):

| строка лога | сообщение | настоящая причина | правка |
|---|---|---|---|
| mr_serverhooks.c 17/24/40/46 | `Undefined function 'CarScript#3545#5.GetNetworkIDLow'` | метода `GetNetworkIDLow()` в Enforce **нет**; есть `proto void GetNetworkID(out int low, out int high)`. Имена собираются на этапе компиляции, поэтому одна строка в базовом классе ломает всех наследников | `MR.NetId(obj, idLow, idHigh)` в `MR_Constants.c`, вызовы переведены на него |
| mr_mediaitems.c 53 | `Incompatible parameter '#return'` | `ToLower()` меняет строку **на месте** и возвращает `int` — `return s.ToLower()` возвращает int из функции со string | `s.ToLower(); return s;` |
| mr_decks.c 178/193 | `FIX-ME: Overriding function 'MR_CarrierSlotName' but not marked as 'override'` | метод есть в `MR_DeckBase`, в наследниках нужен `override` (а в самом базовом классе — наоборот, без него) | `override` в двух наследниках, снят в базовом классе |
| mr_deckresume.c 221 | `Undefined function 'EntityAI.MR_TapeSetPosition'` | `m_Carrier` объявлен `EntityAI` (так его отдают `Cast`-ы), метод живёт в `MR_CarrierBase` | `MR_CarrierBase carrier = MR_CarrierBase.Cast(...)` |
| mr_serverengine.c 527 | `Incompatible parameter 'radio'` | имя `radio` в этой же функции уже занято `PersonalRadio` (ветка `HOST_ITEM`), второе объявление в той же области | переменная переименована в `inCargo` |

Новые проверки (`tools/check_project.py`):

* `check_engine_api()` — `GetNetworkIDLow/High`, `ToLower()/ToUpper()` как значение;
* `check_item_method_receivers()` — наш item-метод, вызванный от `EntityAI/Object`;
  тип ищется **от строки вызова вверх**, чтобы одно имя в двух функциях файла
  не давало ложного срабатывания.

Тест: `python tools\selftest_engine_api.py` — ловит все три случая и не ругается
на правильный код. **Важно:** `check_fresh.py`/`check_pbo.py` такие ошибки не
видят, компилятора у них нет; проверка ловит только известные ловушки API.

## Скриншот 11.11 — `@MusicRadioServer` в корне сервера

В `E:\DayZServer` рядом с `@MusicRadioCore`, `@MusicRadioDB` и настоящим
`ServerMOD\addons\MusicRadioServer.pbo` лежала папка `@MusicRadioServer`
с тем же `.pbo`. Виноват `DEPLOY.bat`/`autobuild.step_deploy`: они клали
`.pbo` в корневую `@`-папку для **любого** мода, а `MusicRadioServer` — тоже мод.

Почему это вредно:

* строке запуска нужен только `-servermod=ServerMOD`, то есть
  `ServerMOD\addons`; копия в `@`-папке не читается вообще;
* но её находят глазами и правят руками — и правят **не тот файл**;
* в `-mod=...` её добавить нельзя: `MusicRadioServer` намеренно без подписи,
  клиент такой мод не примет, а серверный код клиенту и не нужен.

Правка:

| где | было | стало |
|---|---|---|
| `autobuild.target_dirs` | `[ServerMOD\addons, @MusicRadioServer\Addons]` — второй путь выбирался, если папка уже существует | ровно один путь, `ServerMOD\addons` |
| `autobuild.step_deploy` | — | убирает старую `@MusicRadioServer` из корня (файлы `.pbo`/`.bisign`, затем пустые папки) |
| `DEPLOY.bat` | `:push_mod` звал `:copy_one` для любого мода | для `MusicRadioServer` — сразу `:server_only` → `ServerMOD\addons`, плюс новая подпрограмма `:drop_root` |
| `check_fresh.py` | такая папка молча не замечалась | «`@MusicRadioServer` в корне сервера - лишняя копия: сервер грузит `ServerMOD\addons`… Удалить: `AUTO_BUILD.bat --deploy`» |

Тест: `python tools\selftest_root_copy.py` — проверяет, что `--deploy` не создаёт
корневую папку, убирает старую и что `check_fresh` это называет.

## Лог 21:16 — шесть наших ошибок, три причины

| строка | сообщение | причина | правка |
|---|---|---|---|
| `mr_serverengine.c` 57, 168 | `Can't find variable 'STOP_USER'` | константа enum без имени класса: в Enforce они живут **внутри** enum | `MR_StopReason.STOP_USER` |
| `mr_serverengine.c` 298 | `Only NULL assignment is allowed` + `Cannot convert 'int' to 'MR_CarrierMemory'` — это **одна** ошибка | `MR_CarrierState.IsFinished(MR_CarrierMemory, int)` позвали как `IsFinished(int, int)` | сравнение секунд на месте: `resumeOffset >= startTrack.m_Duration` |
| `mr_serverengine.c` 314, 450 | `Can't find variable 'm_StartOffset'` | в `MR_PlayRequest` (MR_AudioTypes.c) поля `m_StartOffset` не было, а код его читал и писал | поле объявлено и обнулено в конструкторе |

Новые статические проверки:

* `check_enum_qualifier()` — наши enum-константы (`STOP_*`, `HOST_*`, `ACT_*`) без префикса класса;
* `check_overload_arity()` — наш метод, которому передали число там, где ждут объект `MR_*`.

Тест: `python tools\selftest_log_traps.py`.

## Лог-файл диагностики

`python tools\doctors.py` (и `CHECK.bat`) теперь пишет всё в
`logs\check_YYYY-MM-DD_HH-MM-SS.log`:

* шапка: время, папка проекта, версия Python, ОС;
* каждый вложенный скрипт — блоком `$ python tools/<имя>.py` + код возврата + весь вывод (stdout **и** stderr);
* итог и код возврата.

Зачем: окно CHECK.bat закрывается по «Press any key», и в переписку попадает
только хвост. Файл можно приложить целиком. Если писать некуда (нет прав на
`logs\`) — диагностика не падает, просто сообщает об этом в stderr.

## Лог 00:53 (12.11) — две наших строки и сломанный раздел 6

| где | сообщение | причина | правка |
|---|---|---|---|
| `mr_serverengine.c` 319, 455 | `Can't find variable 'm_StartOffset'` | поле жило в `MR_PlayRequest`, а сервер работает с `MR_Request` — две похожие структуры, одна переменная | `m_StartOffset` добавлен в `MR_Request` (+ сериализация: поле идёт последним, старые пакеты читаются как 0) |
| чекер, раздел 6 | `не читается (cannot access local variable 'mod' …)` для **каждой** копии | в `check_pbo.scan_server` печатался `mod` из другой ветки функции | имя мода берётся из имени файла; раздел снова показывает строки/размер/«совпадает с проектом» |

Важно: правка 20261162 (`m_StartOffset` в `MR_PlayRequest`) была **не той** — я
поправил вторую структуру, а компилятор указывал на `MR_Request`. Теперь поле в
обеих, и проверка ловит именно «поля нет в структуре»:

* `check_struct_fields()` — `var.m_Поле`, где `var` объявлена как наш класс, а поля нет;
* тест `python tools\selftest_struct_fields.py` (в нём же — что `check_pbo --scan` не падает).

Ограничение: проверка смотрит только поля с префиксом `m_` и только однозначные
объявления переменной (одно имя — один тип), поэтому ложных срабатываний не даёт,
но и не заменяет компилятор.

## Лог 01:12 (12.11) — `Unknown type 'DayZGame'`, `Can't compile "Mission"`

```
MusicRadioCore/Scripts/5_Mission/musicradio/mr_gamehooks.c(134): Unknown type 'DayZGame'
```

`DayZGame` — класс **миссии**. Он объявлен в ванильном `5_Mission`, а мод грузит
в `5_Mission` только свои mission scripts: ванильный тип оттуда не виден, файл
стоял в правильном слое, но класса всё равно нет. Ошибка одна и появляется
только на этапе Mission — то есть после ~20 минут запуска сервера.

Правка: входящие RPC переехали с `DayZGame.OnRPC` на `MissionGameplay.OnRPC`
(`MR_ClientRPC.HandleFromMission`). `MissionGameplay` виден всегда, мы его и так
патчим, и он есть и на клиенте, и на listen-сервере. Серверные запросы идут с
`target = объект игрока` и приходят в `PlayerBase.OnRPC` (`@MusicRadioServer`) —
этот путь не изменился.

Новая проверка `check_modded_layer()`: наши `modded class` для mission-классов
(`DayZGame`, `Mission*`, `LoginQueueBase`) — только из `5_Mission`.
Тест: `python tools\selftest_mission_types.py`.

## Урок по этим двум логам

Ошибки слоя скриптов стоят дорого: сервер тратит 20+ минут, чтобы сказать одну
строку. Поэтому проверки в `check_project.py` намеренно статические и дешёвые
(`check_modded_layer`, `check_struct_fields`, `check_enum_qualifier`,
`check_engine_api`, `check_item_method_receivers`) — они ловят известные ловушки
`Unknown type` / `Can't find variable` / `Cannot convert` до сборки PBO. Полной
заменой компилятору они не являются.

## Лог 01:23 (12.11) — `override OnRPC` и дубль `OnHide`

```
mr_gamehooks.c,128: Function 'OnRPC' is marked as override, but there is no function with this name in the base class
mr_radiomenu.c,560: Multiple declaration of function 'OnHide'
```

Что было не так:

| попытка | результат |
|---|---|
| `modded class DayZGame { override OnRPC }` | `Unknown type 'DayZGame'` — тип из ванильной 5_Mission не виден |
| `modded class MissionGameplay { override OnRPC }` | `no function with this name in the base class` — **у Mission OnRPC нет вообще** |
| второй `OnHide` в `MR_RadioMenu` | `Multiple declaration` — при копировании блока задвоился метод |

Рабочая схема (проверена на этом же сервере в обратную сторону):

```
клиент -> сервер:  rpc.Send(player, RPC_REQUEST, true, null)
                   принимает PlayerBase.OnRPC в @MusicRadioServer
сервер -> клиент:  rpc.Send(pb, RPC_PLAY, true, identity)
                   принимает PlayerBase.OnRPC в @MusicRadioCore
```

* новый `@MusicRadioCore/Scripts/4_World/MusicRadio/MR_PlayerRPC.c` —
  `PlayerBase.OnRPC`, первым делом проверяет диапазон `RPC_CONFIG..RPC_STATE`
  и только потом `IsClient()`, чтобы не читать чужие пакеты;
* разбор живёт в `5_Mission` (`MR_ClientRPC.Handle`), а 4_World не имеет права
  звать верхний слой — мост `MR_MissionBridge` (`ScriptCaller`), подставляется
  из `MissionGameplay.OnInit`;
* на сервере все `rpc.Send(null, …)` заменены на `SendTo`/`SendToIdent` —
  иначе пакет уходит объекту игры, у которого нет обработчика (это и была
  причина «клиент ничего не слышит»).

Тест: `python tools\selftest_rpc_hooks.py` — запрещает `modded class DayZGame`,
`override OnRPC` вне `PlayerBase`, два `OnRPC` в одном моде, `rpc.Send(null, …)`
на сервере и прямой вызов `MR_ClientRPC` из 4_World.

## Лог 01:33 — `Method 'Object' is private` (ревизия 20261172)

```
mr_radiomenu.c,100..107: Method 'Object' is private   (31 наша ошибка, интервал 100-107)
```

Лог указывал на строки с `Hook("MR_CloseButton")` и т. д. — но дело было не в
вызовах: **`Hook` — это protected-метод самого `Object`**, и `void Hook(string)`
в `MR_RadioMenu` перекрывал его. Компилятор ругался на каждую строку, где
`Hook(...)` вызывается, поэтому в логе 8 (а с повторами — 31) одинаковых строк.

Переименовано в `SetWidgetHandler(string)`.

Мораль для будущих правок: свой метод нельзя называть так же, как любой член
`Object` (`Hook`, `Unhook`, `SetHandler`, `GetGame`, `ToString`, `Delete`,
`ClassName`, `Type`, `GetInstance`, ...). `tools/selftest_rpc_hooks.py` теперь
проверяет это списком `OBJECT_MEMBERS` и назовёт файл и метод.

## Лог 01:41 — `Types ... are unrelated` (ревизия 20261172)

```
mr_radiomenu.c,112/130/307-311: Types 'MR_RadioMenu' and 'ScriptedWidgetEventHandler'
                                are unrelated / Cannot convert ...
mr_radiomenu.c,138:             FIX-ME: Undefined function 'UIManager.SetFocus'
```

20 наших ошибок, и снова все строки — вызовы. Причина в базовом классе:

```
proto native void SetHandler(ScriptedWidgetEventHandler eventHandler);  // enwidgets.c:172
class UIScriptedMenu extends UIMenuPanel   // UIMenuPanel : Managed, uiscriptedmenu.c:2
```

`UIScriptedMenu` **не является** обработчиком виджетов: он сам ловит события,
потому что ванильные окна дополнительно наследуют `ScriptedWidgetEventHandler`
(`OptionsMenu` — `m_Root.SetHandler(this)` на строке 98 через `ScriptedWidgetEventHandler`-наследование
своих табов, `OptionsMenuGame`, `PlayerListScriptedWidget`), либо их обработчик —
отдельный объект.

Как в ванили сделано у окон:

| класс | база | как ловит клики |
|---|---|---|
| `OptionsMenu` | `UIScriptedMenu` | `override bool OnClick(...)`, обработчик — табы |
| `InGameMenu` | `UIScriptedMenu` | `override bool OnClick(...)` |
| `BookMenu` | `UIScriptedMenu` | `override bool OnClick(...)` по `w.GetUserID()` |
| `OptionsMenuGame` | `ScriptedWidgetEventHandler` | `m_Root.SetHandler(this)` (строка 140) |

Выбран второй путь без смены базы: меню осталось `UIScriptedMenu` (иначе
`UIManager.ShowScriptedMenu` его не примет и `Init`/`Update` не вызовутся),
а на виджеты вешается `MR_MenuEvents extends ScriptedWidgetEventHandler`,
который пересылает `OnClick/OnChange/OnDoubleClick` в меню. Поле `ref`, иначе
обработчик соберёт сборщик мусора вместе с виджетами.

Фокус: `GetGame().GetUIManager().SetFocus(...)` — такого метода у `UIManager` нет
(в нём только `ShowScriptedMenu`, `HideScriptedMenu`, `GetMenu`, `ShowCursor`...).
`SetFocus(Widget)` — глобальная функция Widget API (`enwidgets.c:698`), её и зовём.

Тест: `python tools\selftest_ui_handler.py` — запрещает `this` в `SetHandler`
из наследника `UIScriptedMenu`, `override Init/OnShow/OnHide/Update` в наследнике
`ScriptedWidgetEventHandler`, `ShowScriptedMenu(this)` из обработчика и
`GetUIManager().SetFocus(...)`.

## Порядок модов (файл `MOD_ORDER.txt`)

Отдельным файлом в корне: клиентский `-mod`, серверные `-servermod`/`-mod`,
раскладка PBO и почему ошибки выше порядком не лечатся.

## Лог 02:00 — `Multiple declaration of variable 'payload'` (ревизия 20261172)

```
mr_clientrpc.c,167/175/183/191/206: Multiple declaration of variable 'payload'
```

Пять наших ошибок, все — объявления в ветках одного `switch`:

```c
switch (rpcType)
{
    case MR.RPC_CONFIG:  { string payload; ... }
    case MR.RPC_PLAY:    { string payload; ... }   // то же имя!
```

В Enforce Script область видимости даёт `switch`, а не блок `{ }` вокруг `case`
(обычная C-привычка здесь не работает). Объявление поднято однажды — до
`switch`, как в ванильных обработчиках.

Тест: `python tools\selftest_no_keys_in_servermod.py` — ищет одно имя,
объявленное дважды внутри одного `switch`.

## `ServerMOD\keys` в `AUTO_BUILD.bat --deploy`

`step_deploy()` копировал публичный ключ «рядом с модом»,
`os.path.dirname(target)` — а для `MusicRadioServer` целевая папка
`ServerMOD\addons`, значит ключ приезжал в `ServerMOD\keys`. Ключ нужен
серверу для **клиентских** модов, а наш `MusicRadioServer` не подписан и в
`-mod` не входит.

* `tools/autobuild.py`: `is_signed_mod(mod)` — ключ копируется только для
  подписываемых (`Core`, `DB`); для `MusicRadioServer` старая
  `ServerMOD\keys` удаляется, если осталась от прошлых запусков;
* `DEPLOY.bat`: `:copy_one` больше не тащит `keys` для `MusicRadioServer`,
  добавлена `:drop_servermod_keys` — убирает уже созданную папку.

## Лог 02:13 — наших ошибок 0, но инструменты падали (ревизия 20261172)

Главное из прогона:

```
стадия : компиляция скриптов прервана
ошибок : 285 (наших: 0, чужих модов: 285)
```

**Строк с MusicRadioCore в логе больше нет** — наши `.c` компилируются. Пять
правок подряд (OnRPC → Hook → SetHandler → payload → это) закрыли скриптовую
часть. Остались только чужие моды.

При этом сам отчёт показал три дефекта в наших же проверках — на Windows:

| симптом в логе | причина | правка |
|---|---|---|
| `ValueError: path is on mount 'C:', start on mount 'P:'` в `selftest_struct_fields`, `selftest_log_traps`, `selftest_engine_api` | тесты подсовывают временные файлы на `C:`, проект на `P:`, а `check_project` звал `os.path.relpath` без защиты | `_rel_in(path, mod)`: при разных дисках возвращает абсолютный путь |
| `FAIL - selftest_override.py упал` + `The system cannot find the path specified.` | `selftest_mtime` запускал `selftest_override` через `os.system(... >/dev/null)`: `NUL` на Windows, рабочая папка не задана | `subprocess.run(..., cwd=ROOT, capture_output=True)` и вывод причины в тексте ошибки |
| в отчёте «5. РАСКЛАДКА НА СЕРВЕР» печатались строки с путём `C:\Users\KONSTA~1\AppData\Local\Temp\mr_cars_*` | `selftest_cars`/`selftest_unsigned`/`selftest_root_copy` вызывали `step_deploy` прямо в общий лог | вывод тестов уходит в буфер (`contextlib.redirect_stdout`) |

Плюс новая проверка в `selftest_ui_handler.py`: `Widget Init()` у наследника
`UIScriptedMenu` обязан быть `override` — `Init` объявлен в
`uiscriptedmenu.c:163`, без `override` компилятор скажет
`Method 'Init' is private` (следующая строка после той, что мы уже починили).

## Как проверить музыку в игре

Отдельный файл: [`15_TEST_IN_GAME.md`](15_TEST_IN_GAME.md) — что заспавнить
(`MR_CassettePlayer` + `MR_Cassette_Rock01` + `MR_MemoryCard_01` + `Battery9V`),
как собрать плеер (карта + кассета + батарейка), клавиши (F7 — меню, F8 — стоп)
и какие строки `[MusicRadio]` должны появиться в логе на каждом шаге.
Там же — почему «музыка в машине» пока требует `requireRadioItemInCargo: false`
(селекция `MR_DashSlot` на моделях машин не пропатчена).

## Скриншот плеера (лог 02:37/03:09): «кассета есть, карты и батарейки нет» (20261172)

На скриншоте инвентаря: `MR_CassettePlayer` + одна кассета в слоте `Cassette`,
слот `BATTERY` пуст. Две находки:

1. **батарейки не было** — плеер требует `BatteryD` (слот `BatteryD` в
   `attachments[]`), без неё `MR_IsReady()` отвечает `no 9V battery`;
2. **карту памяти было физически некуда вставить** — слот `MR_MemoryCard` есть
   у плеера, но у кассеты/диска слотов не было вовсе. Плеер навсегда оставался
   с `no memory card`.

Правка:

* `@MusicRadioCore/config.cpp`: у `MR_CassetteCase_Base` и `MR_MiniDisc_Base`
  появился `attachments[] = { "MR_MemoryCard" }` — карта вставляется в НОСИТЕЛЬ;
* `MR_Decks.c`: новый `MR_HasMemoryCard()` — ищет карту в кассете, а для уже
  заспавненных предметов принимает и карту в самом плеере; `MR_IsReady()`
  сначала проверяет носитель, потом карту (сообщение `no memory card`
  осталось прежним).

Порядок сборки для игрока: карта → в кассету, кассета → в плеер, батарейка → в
плеер. См. `15_TEST_IN_GAME.md`.
