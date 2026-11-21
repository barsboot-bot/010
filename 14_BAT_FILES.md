# Что делает каждый .bat файл

Коротко: **7 батников в корне** и **2 в `tools\`**. Пять из них — разные входы в
одну и ту же цепочку, остальные — диагностика и импорт музыки.

```
исходники (@MusicRadio*)  ->  release\staging  ->  release\pbo  ->  сервер
        stage_pbo.py            AddonBuilder        Deploy
```

Кто что делает на этом пути:

| файл | одна строка | нужен DayZ Tools? | меняет файлы? |
|---|---|---|---|
| `REBUILD.bat` | текстуры + база музыки + staging + архив + проверки | нет | **да** (много) |
| `BUILD_PBO.bat` | `staging` → `.pbo` + подпись | **да** (AddonBuilder) | да |
| `DEPLOY.bat` | `release\pbo` → сервер + `release\deploy` | нет (собирает через BUILD_PBO) | да |
| `AUTO_BUILD.bat` | всё выше одной командой | да (если не `--no-build`) | да |
| `CHECK.bat` | диагностика «почему игра не видит правок» | нет | **нет** |
| `EXPLAIN_STALE.bat` | объясняет, почему `tools\` устарели | нет | только с `--fix` |
| `IMPORT_MUSIC.bat` | перетащить папку с треками → альбом в моде | нет (нужен ffmpeg) | да |
| `tools\build_pbo.bat` | заглушка → вызывает `..\BUILD_PBO.bat` | — | — |
| `tools\import_music.bat` | настоящая работа для `IMPORT_MUSIC.bat` | нет | да |

---

## REBUILD.bat — «пересобрать проект без DayZ Tools»

Самый полезный файл, когда правили конфиги/текстуры/музыку, а не скрипты.
Самодостаточен: ищет Python, проверяет Pillow/numpy, при необходимости
предлагает поставить.

Четыре шага (плюс проверки):

1. `tools\make_textures.py` — текстуры носителей (кассета, MD), обложки;
2. `tools\generate_db_config.py` — `@MusicRadioDB\config.cpp`, `index.json`,
   `carriers.json`, `types_musicradio.xml`, кассета/MD в `@MusicRadioCore`;
3. `make_textures.py --albums` — обложки для альбомов, которые появились на
   шаге 2 (второй вызов не лишний — это из-за него когда-то не хватало
   `mr_cassette_uns.rvmat`);
4. `tools\stage_pbo.py --clean --zip` — `release\staging` + `MusicRadio_ARCHIVE.zip`;

затем `check_project.py`, `check_bat.py`, `check_fresh.py`.

**PBO не собирает** — в конце пишет «дальше `BUILD_PBO.bat`».

## BUILD_PBO.bat — сборка и подпись PBO

Единственный, кому нужны **DayZ Tools** (AddonBuilder). Ищет их во всех
библиотеках Steam, если не нашёл — спрашивает путь. Окно не закрывается до
нажатия клавиши (частая причина «не увидел ошибку»).

- вход: `release\staging\MusicRadioCore|MusicRadioServer|MusicRadioDB`;
- выход: `release\pbo\<Mod>.pbo` + `.bisign`;
- **`MusicRadioServer` всегда без подписи** (так задумано, он серверный);
- сам проверяет, не устарел ли `staging`, и при необходимости обновляет его
  через `stage_pbo.py` — поэтому «собрал, а в игре старое» здесь почти
  невозможно;
- ключ ищется как `release\keys\KRaTos.biprivatekey`, старый
  `musicradio.biprivatekey`, в корне проекта или рядом с `.bat`.

## DEPLOY.bat — разложить по серверу

1. проверяет, что PBO собраны и свежие (зовёт `check_fresh.py --pbo`);
2. `release\pbo` → `release\deploy\@<Mod>\Addons\` + `keys` + `meta.cpp`;
3. `MusicRadioServer` — **без** `.bisign` и **без** `keys`;
4. если найден сервер (переменная `SERVER_DIR`, `server_path.txt`,
   `E:\DayZServer`, `DayZServer_x64.exe` в папках на дисках) — копирует:
   - `@MusicRadioCore`, `@MusicRadioDB` → `<сервер>\@<Mod>\Addons`;
   - `MusicRadioServer.pbo` → `<сервер>\ServerMOD\addons` (единственное место,
     откуда его берёт `-servermod=ServerMOD;ServerCARS`);
   - `KRaTos.bikey` → `<сервер>\keys` (только для клиентских модов);
   - и убирает наше из чужих/старых мест: `ServerCARS`, `@MusicRadioServer` в
     корне сервера, `ServerMOD\keys` (последние две правки — 20261167/68).

## AUTO_BUILD.bat — вся цепочка одной командой

Тонкая обёртка: ищет Python и передаёт всё в `tools\autobuild.py`.

| запуск | что делает |
|---|---|
| `AUTO_BUILD.bat` | staging → PBO → проверки |
| `AUTO_BUILD.bat --deploy` | то же + раскладка на сервер (`@Mod` + ключ) |
| `AUTO_BUILD.bat --no-build` | только staging и проверки, без AddonBuilder |

Перед стартом смотрит на `TOOLS_STALE.flag`: если `tools\` старой ревизии —
останавливается и говорит, что делать (`EXPLAIN_STALE.bat --fix` + распаковать
архив), потому что иначе проверки «слепые».

## CHECK.bat — диагностика, ничего не меняет

Отвечает на вопрос «почему игра не видит исправлений». Показывает свежесть всей
цепочки (исходники → staging → PBO → deploy), проверяет `config.cpp`, весь
проект и **читает логи последнего запуска сервера**:

- `DayZServer_x64_*.RPT` (рядом с exe), `*.txt` там же;
- `script_*.log` / `error_*.log` в `instance` и `instance\Users\*`;
- с ключом `--mdmp` — дамп падения `*.mdmp`.

Именно этот файл даёт строки вида «ошибок 307 (наших 5, чужих 302)» и вердикт
«наша ошибка компиляции». Путь к серверу ищет сам; можно задать один раз
`server_path.txt` или на запуск `set DAYZ_SERVER=E:\DayZServer`.
Полный отчёт пишется в `logs\check_<дата>.log` — его и удобно присылать.

## EXPLAIN_STALE.bat — почему «CHECK сказал ОК, а игра падает»

Срабатывает, когда в `tools\` лежат скрипты старой ревизии: показывает, что
именно устарело и какие проверки из-за этого не работают.

- без ключей — только читает и печатает;
- `EXPLAIN_STALE.bat --fix` — переименовывает `tools\` в `tools_old_<время>`,
  чтобы новый архив распаковался на её место, а не рядом
  (`...\MusicRadio\MusicRadio\tools` — так и было 11.11.2026).

Порядок лечения: `EXPLAIN_STALE.bat --fix` → распаковать
`MusicRadio_ARCHIVE.zip` в корень с заменой.

## IMPORT_MUSIC.bat — добавить альбом

Перетащите **папку** с треками на этот файл. Дальше `tools\import_music.bat`
(в корне только поиск проекта):

1. каждый аудиофайл → Ogg Vorbis 44.1 кГц стерео (через `ffmpeg`);
2. рядом с `.ogg` — playlist json;
3. `generate_db_config.py` пересобирает `@MusicRadioDB\config.cpp`, `index.json`,
   `carriers.json`, `types_musicradio.xml` и добавляет кассету/MD в
   `@MusicRadioCore\config.cpp`;
4. `release\staging` обновляется, чтобы PBO собрался из свежих данных.

Нужен Python 3.8+ и `ffmpeg` (ищется в `tools\ffmpeg\`, затем в `PATH`; если
исходники уже `.ogg` — не нужен). Оба может поставить через `winget`.

## tools\build_pbo.bat — заглушка

Оставлен для совместимости со старыми инструкциями: просто вызывает
`BUILD_PBO.bat` из корня. Отдельно запускать не нужно.

## tools\import_music.bat — рабочая часть импорта

Настоящий импорт музыки (см. `IMPORT_MUSIC.bat`). Запускать напрямую можно, но
проще перетащить папку на `IMPORT_MUSIC.bat` в корне.

---

## Какой файл когда запускать

| ситуация | команда |
|---|---|
| правил `.c` (скрипты) | `python tools\stage_pbo.py --clean --zip` → `BUILD_PBO.bat` → `DEPLOY.bat` |
| правил текстуры / конфиги / музыку | `REBUILD.bat` → `BUILD_PBO.bat` → `DEPLOY.bat` |
| «сделай всё сам» | `AUTO_BUILD.bat --deploy` |
| сервер упал / игра не видит правок | `CHECK.bat` (лог в `logs\`) |
| «CHECK говорит ОК, а падает» | `EXPLAIN_STALE.bat` |
| добавить альбом | перетащить папку на `IMPORT_MUSIC.bat` |

## Правила, из-за которых батники выглядят странно

- **Никаких многострочных `if`/`for` в скобках.** Такие блоки разваливаются,
  если файл случайно сохранён с LF, и в окне появляется «… is not recognized as
  an internal or external command». Поэтому логика вынесена в `tools\*.py`, а в
  `.bat` остались метки `goto`/`call`.
- **Все файлы должны быть CRLF.** `python tools\check_bat.py` это проверяет
  (и ещё скобки, и склейку путей) — он входит в `REBUILD.bat`.
- **`%~dp0` уже заканчивается на `\`** — свой слэш не добавляем.
- **Проверка `pushd "%ROOT%" || goto :no_root`** не для красоты: без неё `cmd`
  печатает «The system cannot find the path specified», а следующие строки
  выполняются в чужой папке.
- **`pause` в конце** — чтобы окно не закрывалось раньше, чем прочитаете ошибку.
