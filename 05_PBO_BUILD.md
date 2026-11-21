# Как упаковать в PBO

Короткая инструкция по шагам. Всё, что нужно, уже подготовлено в `release/`.

## 0. Что где лежит

```
release/
├── staging/                        <- содержимое будущих PBO (уже почищено)
│   ├── MusicRadioCore/             config.cpp + Scripts/ + gui/
│   ├── MusicRadioServer/           config.cpp + Scripts/
│   ├── MusicRadioDB/               config.cpp + Music/ + Scripts/ + CREDITS.txt
│   ├── MusicRadioCore.files.txt    список файлов (для AddonBuilder -include)
│   ├── MusicRadioServer.files.txt
│   └── MusicRadioDB.files.txt
├── BUILD_MANIFEST.txt              sha256 каждого файла + размер пака
├── report_stage.txt                отчёт подготовки, ошибки (если были)
├── keys/                           сюда положить KRaTos.biprivatekey + KRaTos.bikey
└── pbo/                            сюда AddonBuilder положит *.pbo и *.bisign
```

`staging/<Mod>/` — это **ровно** то, что попадёт внутрь PBO. Внутри не должно быть
папок с `@`: `@MusicRadioCore/Addons/MusicRadioCore.pbo` содержит `config.cpp`,
`Scripts/`, `gui/` — без `@MusicRadioCore/` в начале путей.

## 1. Перегенерировать базу (если меняли музыку)

```bash
python tools/generate_db_config.py
```

## 2. Проверить и разложить staging

```bash
python tools/check_project.py          # логика проекта
python tools/stage_pbo.py --clean      # подготовка к упаковке
```

`stage_pbo.py` сам проверяет то, на чём обычно ломается сборка:

* есть ли `config.cpp`, `CfgPatches`, `CfgMods`;
* существуют ли папки, на которые ссылается `CfgMods.defs` (`files[] = { ... }`);
* нет ли в путях PBO не-ASCII имён и папок, начинающихся с `@`;
* у каждого включённого трека есть `MR_<PROVIDER>_<ID>_SoundSet` **и** `.ogg`;
* выкидывает мусор (`__pycache__`, `*.pyc`, `Thumbs.db`, `*~`);
* пишет `release/BUILD_MANIFEST.txt` (sha256) и `release/report_stage.txt`.

Ожидаемый результат:

```
OK - staged for AddonBuilder, no problems
```

## 3. Собрать PBO (Windows + DayZ Tools)

Один раз: положить приватный ключ в `release/keys/KRaTos.biprivatekey`
(имя свободное: батники ищут `KRaTos.*` и старое `musicradio.*`, регистр не важен)
(DayZ Tools → DSSignFile → генератор ключей; публичный `.bikey` потом идёт в
`keys/` мода и на сервер).

```bat
BUILD_PBO.bat
```

Внутри — `AddonBuilder.exe` с `-prefix=<Mod> -clear -packonly -sign=...`.
На выходе:

```
release\pbo\MusicRadioCore.pbo     + .pbo.KRaTos.bisign
release\pbo\MusicRadioServer.pbo   + .pbo.KRaTos.bisign
release\pbo\MusicRadioDB.pbo       + .pbo.KRaTos.bisign
```

Если ключа нет, скрипт предупредит и соберёт неподписанные PBO.

## 4. Разложить по папкам модов

```bat
@MusicRadioCore\Addons\MusicRadioCore.pbo
@MusicRadioCore\keys\KRaTos.bikey

@MusicRadioServer\Addons\MusicRadioServer.pbo     REM только сервер
@MusicRadioServer\keys\KRaTos.bikey

@MusicRadioDB\Addons\MusicRadioDB.pbo
@MusicRadioDB\keys\KRaTos.bikey                   REM если пак подписан
```

Порядок загрузки: `MusicRadioCore` (+ `MusicRadioDB` на клиенте), серверный мод —
последним. Клиентам `@MusicRadioServer` не нужен вообще.

## 5. Проверка после сборки

* в клиентском логе `[MusicRadio] Client mission ready`;
* в серверном `[MusicRadio] Server engine ready (protocol 1)` и
  `Library loaded: 2 entries`;
* F7 в машине открывает меню, трек играет.

Если `Libraries`/`AddonBuilder` ругается на `CfgMods` — почти всегда причина в
`prefix`: имя префикса должно совпадать с именем папки внутри PBO
(`MusicRadioCore`), а не `@MusicRadioCore`.

## 6. Архив проекта

```bash
python tools/stage_pbo.py --zip
```

Создаёт **`MusicRadio_ARCHIVE.zip` в корне проекта**: весь проект (исходники,
доки, инструменты, staging) без `.git`, `release/pbo` и старых архивов. Внутри
лежит `ARCHIVE_INFO.txt` с датой сборки и списком «с чего начать». Этот zip — то,
что имеет смысл отдать на машину с Windows, где стоит DayZ Tools.

## Перед сборкой: staging должен быть свежим

`AddonBuilder` пакует **`release\staging`**, а не `@MusicRadio*`. `BUILD_PBO.bat`
теперь сам проверяет это (`tools\check_fresh.py --stage`) и при расхождении
запускает `stage_pbo.py --clean`, но знать про этот шаг полезно: если правили
`@MusicRadio*` и собираете PBO другим способом (вручную, через PBO Manager),
сначала обновите staging.

Проверить в любой момент:

```bat
CHECK.bat                            :: всё сразу, ничего не меняет
python tools\check_fresh.py          :: где рассинхрон
python tools\check_db_config.py      :: дубли блоков в @MusicRadioDB/config.cpp
```

Подробно про «в игре осталась старая ошибка» — `docs/11_TROUBLESHOOTING.md`.

## Если CfgConvert ругается

DayZ печатает ошибку и прекращает сборку — вот две, которые уже случались:

| Сообщение AddonBuilder | Причина | Лечение |
|---|---|---|
| `File ...config.cpp, line N: CfgSoundSets.X: Undefined base class 'personalradio_staticnoise_SoundSet'` | класс наследовался от ванильного SoundSet, которого нет в этой сборке DayZ | исправлено: `MR_RadioFallback_SoundSet` больше ни от чего не наследуется. Чекер теперь ловит любое наследование от неизвестного класса (`config: N inheritance(s) checked`) |
| `Config : some input after EndOfFile` | лишний текст **после** последней `};` файла (обычно случайная вставка при правке) | открыть `config.cpp`, убедиться, что после закрывающей `};` ничего нет; проверить `python tools\check_project.py` |

| `File MusicRadioDB\config.cpp, line N: .CfgSoundSets: Member already defined` | в файле два `class CfgSoundSets` (бывает после ручной правки или склейки файлов), из-за чего один и тот же член объявлен дважды | `@MusicRadioDB/config.cpp` теперь **генерируется целиком из шаблона**: `python tools\generate_db_config.py` перезаписывает файл и убирает дубли. Чекер ловит такое заранее |

`Member already defined` на строке, которой в файле уже нет (например, 796 при
109 строках), — это **старый PBO**, а не ошибка конфига: сначала `CHECK.bat`.

| `File MusicRadioCore\config.cpp, line N: /CfgVehicles.MR_MiniDisc_X: Undefined base class 'MR_MiniDisc_Base'` | класс-наследник объявлен **выше** своей базы. DayZ читает `config.cpp` сверху вниз | исправлено: генератор держит носителей в отдельном разделе `MEDIA CARRIERS` после баз и **сам** переносит блок, если находит его ниже; `check_project.py` ловит порядок (`config: N base class(es) declared before their children`) |
| `МиниДиск из лога называется `MR_MiniDisc_Kostya`, а в проекте `MR_MiniDisc_Rock01` | имена классов носителей генерируются из альбомов вашей базы | это нормально: класс на каждый альбом. Правьте не `config.cpp`, а альбом в `@MusicRadioDB/Music/...json` и запускайте `REBUILD.bat` |

### Ошибки самого батника (не AddonBuilder)

| Сообщение в консоли | Причина | Лечение |
|---|---|---|
| `The filename, directory name, or volume label syntax is incorrect` + путь вида `P:\mods\MusicRadiotools\stage_pbo.py` | в `.bat` потерялся `\` при склейке: `%ROOT%tools\...` вместо `%ROOT%\tools\...` | исправлено; ловится `python tools\check_bat.py` (проверка входит в `check_project.py` и в `REBUILD.bat`) |
| `… was unexpected at this time` | непарная скобка в `if`/`for` | `python tools\check_bat.py` показывает строку |
| `'not' is not recognized as an internal or external command` (и другие обрывки слов) | `.bat` сохранён с **LF** вместо CRLF: cmd рвёт многострочные `if (...)` | `python tools\check_bat.py` сам исправляет LF -> CRLF; при ручной правке сохраняйте «Windows (CRLF)» |
| `The system cannot find the path specified` от самого батника | `cd`/`pushd` в несуществующую папку | исправлено: переход проверяется (`pushd ... \|\| goto :no_root`) и объясняет ошибку |
| первая строка выполняется как мусор | BOM в начале `.bat` | сохранить без BOM; чекер это видит |
| метки не работают, `goto` уводит не туда | файл сохранён с LF вместо CRLF | `tools/check_bat.py` проверяет CRLF во всех `.bat` |

Если ошибка не из этой таблицы — пришлите строку `!> File ...` из окна AddonBuilder:
в ней указан файл и номер строки, этого обычно достаточно.

## Копирование сразу на сервер (`server_path.txt`)

`DEPLOY.bat` умеет сам положить @Моды на сервер — путь берётся по порядку:

1. переменная окружения `SERVER_DIR`
2. первая строка файла **`server_path.txt`** в корне проекта
3. автопоиск: `<диск>:\DayZServer` с `DayZServer_x64.exe` внутри (спросит
   только подтверждение и запомнит ответ)
4. вопрос в консоли (Enter — не копировать)

При первом запуске просто введите путь один раз:

```
Куда копировать @Моды? Например: E:\DayZServer
Путь к папке сервера: E:\DayZServer
```

он сохранится в `server_path.txt`, и дальше `DEPLOY.bat` копирует молча:

```
=== Копирую на сервер: "E:\DayZServer" ===
  @MusicRadioCore\Addons\MusicRadioCore.pbo  -> сервер
  @MusicRadioServer\Addons\MusicRadioServer.pbo  -> сервер
  @MusicRadioDB\Addons\MusicRadioDB.pbo  -> сервер
  keys\KRaTos.bikey (публичный ключ)
```

Что важно в этой секции:

* старый `<Мод>.pbo` на сервере **удаляется перед копированием** — иначе
  Windows может оставить вчерашний файл, а это ровно то, что даёт «в игре
  старая ошибка»;
* вместе с `.pbo` едут `.bisign`, `meta.cpp` и `keys\*.bikey`;
* `MusicRadioServer.pbo` кладётся в `@MusicRadioServer` **и**, если такие папки
  на сервере уже есть, в `ServerMOD\addons` и `ServerCARS\addons` — именно
  оттуда его грузит строка `-servermod=ServerMOD;ServerCARS`. Новых папок
  деплой не создаёт, обновляет только существующие;
* `CHECK.bat` теперь показывает и состояние сервера:

```
  сервер: E:/DayZServer
    - MusicRadioCore     СТАРЫЙ на сервере
    . MusicRadioDB       совпадает
      -> DEPLOY.bat скопирует свежие @Моды на сервер
```

`release\deploy` остаётся: это то, что отдаётся клиентам.

## Где папка `Addons`

Её нет в исходниках: `@MusicRadioCore` в репозитории — это папка **источника**
для AddonBuilder (`config.cpp`, `Scripts/`, `gui/`, `data/`). `Addons/` — часть
**готового** мода, и создаётся она на шаге развёртывания:

```bat
BUILD_PBO.bat     ->  release\pbo\MusicRadioCore.pbo (+ .bisign)
DEPLOY.bat        ->  release\deploy\@MusicRadioCore\
                          Addons\MusicRadioCore.pbo
                          keys\KRaTos.bikey
                          meta.cpp
```

Так же раскладываются `@MusicRadioDB` (клиент + сервер) и `@MusicRadioServer`
(только сервер). Переносить на сервер/в Steam нужно именно папки из
`release\deploy`, а не исходники.

## Временная папка

Все временные файлы лежат **в корне проекта**, в `release\temp\`:

| Кто | Что кладёт |
|---|---|
| `REBUILD.bat`, `BUILD_PBO.bat`, `DEPLOY.bat` | `tools_rev.txt`, `stage.txt`, `pbo_state.txt` (вывод проверок, читается через `for /f`) |
| `check_bat.py` | `*.lf.bak` - копия батника до исправления переводов строк |
| `selftest_pbo.py` | `selftest_*` - тестовые PBO (удаляются сами) |

Плюс `BUILD_PBO.bat` использует `release\temp` для работы AddonBuilder.
Папка в `.gitignore`, в архивы не попадает, удаляется вместе с `release\`.
