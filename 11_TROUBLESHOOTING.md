# 11. «В игре осталась старая ошибка» — почему и что делать

Самая частая ситуация в этом проекте: в исходниках ошибку уже исправили,
`REBUILD.bat` отработал, а игра всё равно падает с тем же текстом (классика —
`File MusicRadioDB\config.cpp, line 796: .CfgSoundSets: Member already defined`).

Причина почти всегда одна: **игра грузит не то, что вы правите**.

```
@MusicRadioDB\config.cpp      <- вы правите здесь
        |  stage_pbo.py копирует
        v
release\staging\MusicRadioDB  <- отсюда AddonBuilder берёт файлы
        |  BUILD_PBO.bat пакует
        v
release\pbo\MusicRadioDB.pbo  <- готовый мод
        |  DEPLOY.bat раскладывает
        v
release\deploy\@MusicRadioDB  <- то, что нужно скопировать на сервер/в лончер
        |  руками
        v
DayZServer\@MusicRadioDB      <- вот это читает игра
```

Разрыв на любом шаге = игра видит старый файл. Ошибка при этом называет
строку, которой в исходниках уже нет (например, 796 в файле из 109 строк) —
это верный признак, что вы читаете старый `.pbo`.

## Быстрая диагностика: `CHECK.bat`

Запустите в корне проекта. Нужен только Python, DayZ Tools не нужны.
Он показывает три вещи и ничего не меняет:

```
1. СВЕЖЕСТЬ: исходники -> staging -> PBO -> deploy
2. САМ config.cpp в @MusicRadioDB (дубли блоков)
3. ПРОВЕРКА ПРОЕКТА (скрипты, база, текстуры, staging)
```

Как читать:

| Строка | Что значит | Что сделать |
|---|---|---|
| `исходник новее staging` | правили `@MusicRadio*`, staging старый | `BUILD_PBO.bat` (сам обновит staging) или `REBUILD.bat` |
| `staging новее PBO` | PBO собран из старых файлов | `BUILD_PBO.bat` |
| `release\deploy\@... старше свежего PBO` | раскладка устарела | `DEPLOY.bat` |
| `PBO не собран` | ещё не пакали | `BUILD_PBO.bat` |
| `OK - исходники, staging и PBO согласованы` | всё свежее | значит проблема **вне** проекта — см. ниже |

То же самое из консоли:

```bat
python tools\check_fresh.py          :: полная картина
python tools\check_fresh.py --stage  :: только «исходники -> staging»
python tools\check_fresh.py --pbo    :: только «staging -> PBO»
python tools\check_db_config.py      :: @MusicRadioDB/config.cpp на дубли
```

## Батник печатает «… is not recognized as an internal or external command»

Строки вида

```
'k_fresh.py)' is not recognized as an internal or external command,
operable program or batch file.
'not' is not recognized as an internal or external command,
```

означают одно: **`.bat` сохранён с LF вместо CRLF** (или в нём BOM).
При LF `cmd` разрывает многострочные блоки `if (...)` / `for (...)` и
начинает выполнять обрывки строк как команды — отсюда и «команда `not`».

Почему это вообще случилось: файл, пройдя через любой редактор, git-настройку
или распаковку, может получить LF. В этом проекте `.gitattributes` задаёт
`*.bat -text` (байты не трогаются), а `tools\check_bat.py` **сам исправляет**
LF -> CRLF и говорит об этом:

```
! CHECK.bat
    нет CRLF (файл целиком с LF): cmd ломает метки и многострочные if
    ИСПРАВЛЕНО: LF -> CRLF (копия: CHECK.bat.lf.bak)
```

`stage_pbo.py` дополнительно перепаковывает `.bat` в архиве, так что из
`MusicRadio_ARCHIVE.zip` они всегда выходят в CRLF. Если правите `.bat` сами —
сохраняйте как «Windows (CRLF)»; `check_project.py` это проверит (раздел
`bat files: N checked`).

`CHECK.bat` после этого случая специально сделан коротким: в нём нет ни одного
многострочного блока в скобках, вся работа — в `tools\doctors.py`.

## Сервер падает: `Undefined base class 'MR_MiniDisc_Base'`

```
File MusicRadioCore\config.cpp, line 169: /CfgVehicles.MR_MiniDisc_Kostya:
Undefined base class 'MR_MiniDisc_Base'
```

Тот же текст бывает и с `MR_CassetteCase_Base` — причина одна.
DayZ разбирает `config.cpp` **сверху вниз**: если `class MR_Cassette_Kostya :
MR_CassetteCase_Base` встречается раньше, чем объявлен сам
`MR_CassetteCase_Base`, база «не существует».

С 11.11.2026 генератор делает раскладку детерминированной: он находит **оба**
базовых класса где угодно в файле, вынимает их и ставит внутрь раздела
`MEDIA CARRIERS`, прямо перед маркером генерации. То есть файл приводится к
одному и тому же виду независимо от того, что в нём было раньше, и повторный
запуск ничего не меняет (md5 тот же).

Лечение в проекте автоматическое:

* базовые классы (`MR_CassetteCase_Base`, `MR_MiniDisc_Base`) объявлены
  выше, в разделе `MEDIA CARRIERS`;
* `tools/generate_db_config.py` при запуске проверяет раскладку и, если блок
  носителей оказался ниже баз, **переносит его** и пишет в консоль
  `carrier block moved below the base classes`;
* `tools/check_project.py` показывает порядок:
  `config: 18 base class(es) declared before their children`; при нарушении —
  `ERROR …: 'X' inherits 'Y', which is declared BELOW it`.

Руками в `@MusicRadioCore/config.cpp` править не нужно — он частично
генерируется. Добавили альбом `Kostya`? Появится `MR_MiniDisc_Kostya`
и `MR_Cassette_Kostya` — это нормально, класс на каждый альбом.

## `'MR_Cassette_Rock01' is not declared in any config.cpp` после нового импорта

`types.xml` в репозитории - статический файл на 1947 типов, а `config.cpp`
перегенерируется под текущий медиа-пак. После импорта нового альбома вместо
`rock01` в конфиге остаётся только `uns`, а блоки старого альбома в
`types.xml` и `cfgspawnabletypes.xml` остаются - и указывают на классы,
которых больше нет.

Теперь `generate_db_config.py` при каждом запуске:

* `sync_types_xml()` - удаляет из `server_files\mpmissions\types.xml` блоки
  альбомов, которых нет в текущем паке, и дописывает недостающие. Файл
  переписывается **в той же кодировке переводов строк** (CRLF), иначе git
  показывает 24 тысячи изменённых строк;
* `purge_spawnable()` - то же для `cfgspawnabletypes.xml`, включая ссылки
  `<item name="MR_Cassette_..." />` внутри блоков проигрывателей.

Проверено: если убрать альбом из базы, оба файла чистятся сами, и
`check_project.py` снова даёт `OK - no errors`.

## `! album 'uns' is not in the media pack` при импорте

Арт рисовался **до** перегенерации базы, поэтому нового альбома в
`index.json` ещё не было. Теперь порядок в `import_music.py` такой:
конвертация -> `generate_db_config.py` -> арт альбома -> staging.

## `Staging: проверить не удалось (check_fresh.py молчит)` + `The system cannot find the path specified.`

Причина одна и та же, и она в батниках: `%MR_TMP%` (путь к временному файлу)
задавался и использовался **внутри одного блока `(...)`**. cmd раскрывает
`%VAR%` при разборе всего блока, то есть до того, как `set` внутри него
выполнится, - получается пустая строка:

```bat
if defined PYF (
  set "MR_TMP=%ROOT%\release\temp\stage.txt"
  %PYF% "%ROOT%\tools\check_fresh.py" --stage > "%MR_TMP%"   <- пусто
)

исправлено:

  set "MR_TMP=...\stage.txt"
  %PYF% "...\check_fresh.py" --stage > "!MR_TMP!"
```

Отсюда же `> ""` -> `The system cannot find the path specified.` и пустая
переменная `STALE` -> «check_fresh.py молчит», после чего staging
пересобирался зря. В `REBUILD.bat`, `BUILD_PBO.bat` и `DEPLOY.bat` все
`%MR_TMP%`/`%TREVFILE%` заменены на `!MR_TMP!`/`!TREVFILE!`.

## `config.cpp в PBO не совпадает с проектом` при свежей сборке

PBO отдаёт файл как есть, а проектный может быть с BOM, CRLF или лишним
переводом строки в конце - побайтовый sha256 такие файлы считает разными.
Теперь сравнение нормализованное (`same_config()`), а при реальном
расхождении печатается **строка** и первые байты с обеих сторон:

```
    совпадает с проектом: НЕТ (PBO 13249 байт, проект 13256 байт)
    первое расхождение: строка 262
      в PBO   : 'class MR_CassetteCase_Base'
      в проекте: 'class Slot_MR_MiniDisc'
```

## `Публичный ключ: НЕ НАЙДЕН`, хотя `.bikey` лежит в `release\keys\`

Было ровно так: оба файла (`KRaTos.biprivatekey`, `KRaTos.bikey`) в
`release\keys\`, а сборка писала «НЕ НАЙДЕН».

Причина в `BUILD_PBO.bat`, в секции 3:

```bat
set "PUBKEY="
set "PUBEXPECT="      <- вот это
call :find_pubkey
```

`PUBEXPECT` - имя парного ключа, его вычисляет `:find_key` из имени
приватного (`KRaTos.biprivatekey` -> `KRaTos.bikey`). Строка `set
"PUBEXPECT="` стирала его прямо перед поиском, поэтому `:probe_pub_dir`
всегда получал пустое имя и выходил. Строка убрана; поиск `.bikey` теперь
ещё и в той папке, где лежит сам приватный ключ.

`check_bat.py` ловит этот класс ошибок: «set "PUBEXPECT=" стирает имя
парного ключа после его вычисления».

Важно для сервера: **имя `.bikey` должно совпадать с `.bisign`**. Если
подпись `MusicRadioCore.pbo.KRaTos.bisign`, то и ключ должен быть
`KRaTos.bikey` - переименование ключа в другое имя сломает проверку подписи.

## `The filename, directory name, or volume label syntax is incorrect.`

Это не про пути и не про проект. Так cmd отвечает на кавычки внутри
`for /f ... in (`команда "с аргументом"`)`. Было:

```bat
for /f "usebackq delims=" %%S in (`%PYF% "%ROOT%\tools\check_tools.py" --quiet`) do ...
```

Теперь команда пишется в временный файл, а `for /f` читает уже его:

```bat
set "MR_TMP=%TEMP%\mr_rev.txt"
%PYF% "%ROOT%\tools\check_tools.py" --quiet > "%MR_TMP%" 2>nul
for /f "usebackq delims=" %%S in ("%MR_TMP%") do set "TREV=%%S"
del "%MR_TMP%" >nul 2>&1
```

Побочный эффект старого варианта - ложное `[ВНИМАНИЕ] скрипты в tools\
старее этих батников` и `Staging: проверить не удалось (check_fresh.py
молчит)`, хотя скрипты свежие: переменная просто оставалась пустой.

`check_bat.py` теперь ловит кавычки в `for /f`/`in (`...`)`.

## `.CfgVehicles: Member already defined` (line 384 и любая другая)

Дословно из игры:

```
File MusicRadioCore\config.cpp, line 384: .CfgVehicles: Member already defined.
```

Речь **не о конкретном классе** внутри блока, а о самом блоке: в файле
оказалось несколько объявлений `class CfgVehicles`. В игре путь
`/CfgVehicles/...` может встречаться в разных файлах (движок сливает
`config.cpp` мода с ванильным), но **два одинаковых блока внутри одного
файла** — это дубль, и загрузка останавливается на втором.

Откуда их было три:

| строки | что это |
|---|---|
| 101 | носители: `MR_MemoryCard_Base`, проигрыватели (там же `class Inventory_Base;`) |
| 289 | `MEDIA CARRIERS` — кассеты/MiniDisc по альбомам |
| 398 | `CAR DASH` — слот `MR_DashSlot` для машин (удалён: машин в конфиге больше нет) |

Каждый блок появился «чтобы объявление было видно рядом» — так лечили
`Undefined base class`. Лекарство неверное: видимость ограничена блоком, но
сам блок не обязан быть новым.

Что теперь делает `tools/generate_db_config.py`:

* `merge_root_blocks()` — сводит **все** блоки `class CfgVehicles` в первый,
  сохраняя порядок (базовые классы остаются выше генерируемых наследников), а
  повторяющиеся пустые объявления (`class Inventory_Base;`) не дублирует;
* `_make_section()` больше не создаёт второй `CfgVehicles`: если маркерный
  блок носителей лежит вне его, содержимое переносится в существующий блок;
  если маркеры случайно оказались внутри чужого блока (например, `CarScript`),
  этот блок переименовывается в `CfgVehicles`;
* `tools/check_project.py` → `check_root_block_dupes()` ловит дубль **до**
  сборки:

```
ERROR @MusicRadioCore/config.cpp: 'class CfgVehicles' объявлен 2 раза
(строки 101, 436) - сервер скажет '.CfgVehicles: Member already defined'
```

Проверено негативным тестом: добавленный руками второй блок был пойман с
номерами строк, после восстановления файла — `OK - no errors`.

Порядок в файле после исправления (один блок `CfgVehicles`):

```
class CfgVehicles
{
    class Inventory_Base;                 <- общее объявление для всего блока
    MR_MemoryCard_Base / проигрыватели
    MR_CassetteCase_Base / MR_MiniDisc_Base
    @@CARRIER_CLASSES_BEGIN@@ ... END@@   <- маркеры (классы альбомов теперь в DB)
};
class CfgSlots { ... }              <- сюда добавлен Slot_MR_DashSlot
```

Важно: после правки `config.cpp` обязательно `python tools\stage_pbo.py --clean`
(иначе `check_project.py` честно скажет `staging is out of date`), потом
`BUILD_PBO.bat`, `DEPLOY.bat`, перезапуск сервера.

## `.CfgVehicles.CivilianSedan: Member already defined` (line 338)

Дословно:

```
File MusicRadioCore\config.cpp, line 338:
/CfgVehicles.CivilianSedan: Member already defined.
```

Это **не** дубль блока `CfgVehicles` (тот даёт `.CfgVehicles: Member already
defined` и ловится `check_root_block_dupes`). Это дубль записи **внутри** блока:
имя сначала объявлено пустым, а потом определено ещё раз.

```cpp
class CfgVehicles
{
    class CivilianSedan;                      // объявление (строка 339)
    class CivilianSedan : CarScript           // ВТОРОЕ объявление (строка 340)
    {
        attachments[] += { "MR_DashSlot" };
    };
};
```

Парсер видит два члена с одним именем и останавливает загрузку. Так выглядел
раздел `CAR DASH`, который добавлял машинам слот для проигрывателя.

Почему «по-другому» не получится:

* `class CivilianSedan : CarScript { ... }` — второе объявление (ошибка выше);
* `class CivilianSedan { ... }` без `: парент` — это **новое** определение
  класса, а не дополнение существующего: дополнять «на месте» в конфиге умеет
  только `modded class` в скриптах, в `config.cpp` такого нет;
* `attachments[] += { ... }` работает только от **прямого** родителя, чей
  массив объявлен в том же классе, — у ванильных машин массив приходит из
  `CarScript`, который в моде лишь объявлен, так что `+=` молча деградировал бы
  в `=`, обнулив штатные слоты машины.

Поэтому раздел убран целиком (`generate_db_config.py` → `drop_car_dash`):
проигрыватель ставится в инвентарь и оттуда, слот в машине был необязательной
функцией. Заодно из `CfgSlots` уходит `Slot_MR_DashSlot`, которого больше нет ни у одной
машины. Скрипт `MR_MediaItems.DockedInVehicle()` остался — он безопасно
возвращает `null`, если слот не найден.

Если слот в машине всё-таки нужен — это делается **отдельным модом** со своим
`CfgPatches` (иначе конфликт с любым другим модом, который тоже правит машины),
а не правкой этого `config.cpp`.

Проверка до сборки:

```
ERROR @MusicRadioCore/config.cpp:340: 'CivilianSedan' уже объявлен на строке
339, а тут определяется второй раз - сервер скажет
'.CfgVehicles.CivilianSedan: Member already defined'
```

## `mr_devicebase.c,211: Undefined function 'EffectSound.SetStartOffset'` (лог 17:46)

`EffectSound` — обёртка над звуком, и половины нужных методов у неё нет.
`SetStartOffset`, `Skip`, `GetLength`, `SetVolume` принадлежат `AbstractWave`,
который лежит в защищённом поле `m_SoundWaveObject` и снаружи недоступен.

Именно поэтому OMD_CarRadio патчил `EffectSound` отдельным файлом:

```c
modded class EffectSound extends Effect
{
    void OmdRadioSetStartOffset(float offset)
    {
        if (m_SoundWaveObject) m_SoundWaveObject.SetStartOffset(offset);
    }
}
```

У нас то же самое, но с именем `MR_*` и в `MR_SoundMix.c`:

```c
m_MR_Sound.MR_PlayWithOffset(fraction);   // SoundPlay() -> SetStartOffset()
```

Про `m_SoundWaveVolumeMax`: он всё-таки **принадлежит** `EffectSound` (поле класса),
поэтому `MR_SetVolumeMax()` работал бы и напрямую — но лучше через метод, там есть clamp.

**Единицы.** `SetStartOffset` берёт **долю 0..1** от длины волны, а не секунды.
Поэтому возобновление требует известной длины трека: `offset / duration`.
Без длины мы начинаем с нуля и пишем предупреждение, а не перематываем в конец.

Проверка `check_sound_api` в `check_project.py` теперь ищет `SetStartOffset`/`Skip`/
`GetLength` на любом объекте, кроме `m_SoundWave*`, и падает до сборки.

## `Incompatible parameter 'handle'` / `'m_Handle'` — FileHandle это не число

```
mr_library.c,351/371: Incompatible parameter 'handle'
mr_config.c,136:      Incompatible parameter 'm_Handle'
```

Сигнатуры из `ensystem.c` (DayZ 1.30):

```c
typedef int[] FileHandle;                                             // массив!
proto FileHandle OpenFile(string name, FileMode mode);
proto void CloseFile(FileHandle file);

typedef int[] FindFileHandle;
proto FindFileHandle FindFile(string pattern, out string fileName,
                              out FileAttr fileAttributes, FindFileFlags flags);
proto bool FindNextFile(FindFileHandle handle, out string fileName,
                        out FileAttr fileAttributes);                 // ТРИ аргумента
proto native void CloseFindFile(FindFileHandle handle);
```

Три вывода:

| было | стало | почему |
|---|---|---|
| `if (fh == 0)` | `if (!fh.IsOpen())` | `int[]` никогда не равен 0 |
| `m_Handle = 0;` | просто `m_Open = false;` | обнулить массив нельзя, `CloseFile` уже закрыл поток |
| `FindNextFile(h, name)` | `FindNextFile(h, name, attr)` | третий параметр — `out FileAttr` |
| нет | `CloseFindFile(handle)` после цикла | иначе дескриптор обхода течёт на каждом скане |

**Как я ошибался трижды** (и почему это важно):
1. оставил 2 аргумента — «Not enough parameters»;
2. поставил 3, но присваивал результат в `handle` — «Incompatible parameter 'handle'»;
   `FindNextFile` возвращает **bool**, поэтому число аргументов вообще не при чём;
3. заменил проверку на `FileHandle.IsOpen()` — метода не существует:
   `typedef int[] FileHandle` без методов, а `if (!handle)` работает.

Канон обхода (совпадает в трёх независимых источниках: DayZ wiki, ванильный
`init.c`, фикс DZEXP-134):

```c
FindFileHandle handle = FindFile(pattern, fileName, fileAttr, FindFileFlags.ALL);
if (!handle)
    return files.Count();

bool found = true;                  // ПЕРВЫЙ файл уже в fileName
while (found)
{
    if (fileName != string.Empty && IsJson(fileName))
        files.Insert(dir + "/" + fileName);
    found = FindNextFile(handle, fileName, fileAttr);   // bool, не handle
}
CloseFindFile(handle);
```

**Как я ошибался дважды.** В `docs` раньше было записано «правильно — 3 аргумента»,
потом я «исправил» на 2 из-за сообщения про `handle`, и получил ту же ошибку с другой
стороны. Правильный порядок — сначала сигнатура в API (`ensystem.c`), потом лог.
Проверка `check_file_api` теперь сверяет и число аргументов, и сравнения handle с 0.

## `mr_library.c(351): Not enough parameters in function 'FindNextFile'`

Сигнатура в DayZ 1.30 — **три** аргумента, не два:

```cpp
FindFileHandle handle = FindFile(pattern, fileName, attr, FindFileFlags.ALL);
while (handle)
{
    ...
    handle = FindNextFile(handle, fileName, attr);   // handle, fileName, attr
}
```

Я в предыдущей правке убрал третий аргумент (из-за ошибки `Incompatible
parameter 'handle'`, которая на самом деле относилась к `FindFile`, а не к
`FindNextFile`) и получил `Not enough parameters`. Правильно — 3 аргумента.
Проверка `check_string_api` теперь требует ровно 3.

## Почему отчёт писал «наших: 0», хотя мод не собирался

Разборщик логов ловил только заранее перечисленные формулировки
(`Missing ';'`, `Broken expression`, `FIX-ME` …). Строка

```
SCRIPT (E): @".../mr_library.c,351": Not enough parameters in function
```

не подходила ни под один шаблон — и в отчёте было «наших: 0». Теперь есть
шаблон на **любой** `SCRIPT (E)`, плюс `Can.?t compile` (без учёта регистра).
Проверено на реальном логе `07-49-45`: 2 наших ошибки находятся построчно.

Мораль: если сервер пишет `Compile error`, а `CHECK.bat` — «наших: 0»,
значит доверять счётчику нельзя, надо смотреть строки `SCRIPT (E)` в
`script_*.log`.

## «CHECK.bat пишет: наших 0, чужих 2» — что это значит

Так было из-за двух ошибок в разборщике логов, обе исправлены в `20261140`:

1. **`Can't compile` не распознавалось.** В регэкспе стояла обычная кавычка,
   а DayZ пишет по-разному (`Can't` / `Can´t`), плюс сравнение было
   регистрозависимым. Теперь шаблон `Can.?t compile` без учёта регистра.
   `FIX-ME` искался только как `FIX-ME:` — теперь ловится в любом виде.

2. **Стадия определялась неверно.** Бралась последняя найденная в файле
   строка-маркер. После падения компиляции движок продолжает писать
   `[Inputs] Loading inputs default ...` и заново грузит `dta\bin`, поэтому
   в отчёте появлялось «чтение Inputs.xml» — как будто до скриптов дело не
   дошло. Теперь:

   * стадия = самая поздняя **по порядку запуска**, а не по номеру строки;
   * если в логе есть `Can't compile` / `SCRIPT (E)`, стадия пишется честно:
     `компиляция скриптов прервана`.

3. **Раздел 8 печатает сами строки**, а не только счётчик: наши — с текстом
   целиком (до 8 штук), чужие — одной строкой для контекста.

Что означал твой отчёт при `наших: 0`:

```
instance\DayZServer_x64_2026-11-11_07-38-39.RPT  (3847 строк)
  ошибок: 2 (наших: 0, чужих модов: 2)
```

Скорее всего это два чужих `FIX-ME` (CodeLock, Gamazer_World_Menu — они есть
в любом прогоне). Но проверить обязательно: перезапусти CHECK.bat с `20261140`
и посмотри строки. Если появится `MusicRadio` — значит мод всё ещё падает,
и в отчёте будет видно файл и строку.

## Ошибки Game-модуля, которые вылезли пачкой (лог 07:03)

Когда синтаксис наконец сошёлся, компилятор показал всё сразу. Каждая строка
из `script_*.log` и что с ней сделано:

| сообщение | причина | исправлено |
|---|---|---|
| `mr_mediaresolver.c,130: Undefined function 'string.StartsWith'` | у `string` нет `StartsWith` | `MR.StartsWith(s, prefix)` |
| `mr_library.c,336: Undefined function 'string.EndsWith'` | нет и `EndsWith` | `MR.EndsWith(s, suffix)` |
| `mr_library.c,351/371: Incompatible parameter 'handle'` | `FindNextFile` без третьего `out FileAttr` | `FindNextFile(handle, name, attr)` (см. раздел выше) |
| `mr_net.c,84/152/194/219/244: Only NULL assignment is allowed` | `(MR_HostType)x` — приведение enum в присваивании | `MR.HostTypeFromInt(x)` |
| `mr_clientaudio.c,133`, `mr_clientstate.c,89` | то же самое | `MR.HostTypeFromInt` / `MR.MediaKindFromInt` |
| `mr_audiotypes.c,167: Variable 'm_Locator' is not strong ref` | поле-объект без `ref` | `ref MR_Locator m_Locator;` |

Остальные ошибки в том же логе (`Bad type 'JsonFileLoader'`, `Can't find class
Param1`, `Unknown type 'auto'` …) — из Expansion/CF/VPP/KGB, это их проблемы и
на наш мод они не влияют.

Новые проверки в `check_project.py` (`check_string_api`) ловят:
`StartsWith`/`EndsWith`, трёхаргументный `FindNextFile`, приведение enum в
присваивании — всё это ERROR до сборки.

## `mr_locator.c(61): Multiple declaration of function 'ToString'`

```
Can't compile "Game" script module!
MusicRadioCore/Scripts/3_Game/musicradio/mr_locator.c(61):
Multiple declaration of function 'ToString'
```

`MR_Locator` объявлял свой `string ToString()`, а этот метод уже есть у
базового `Class` — перекрывать его движок не даёт.

```cpp
// было
string ToString() { return "LCT|1|" + ... ; }
// стало
string Serialize() { return "LCT|1|" + ... ; }
```

Вызов поправлен в `MR_Net.EncodePlay`: `req.m_Locator.Serialize()`.

Запомнить список имён, которые нельзя объявлять: `ToString`, `GetType`,
`ClassName`, `Type`, `IsInherited`, `GetInstanceType`, `ToStringNative`.
Проверка: `check_project.py` → `check_reserved_methods()`.

## Где искать причину: `.RPT` и `script_*.log`, а не только `*.txt`

`CHECK.bat`, раздел 8, теперь читает из `<сервер>\instance`:

| файл | что в нём |
|---|---|
| `DayZServer_x64_*.RPT` | главный: стадия запуска, стек, все исключения с `Reason:` |
| `DayZServer_x64_*.txt` | общий лог запуска (то, что видно в консоли) |
| `script_*.log`, `error_*.log` | вывод компилятора скриптов: `Syntax error`, `FIX-ME` |

Именно в `.RPT`/`script_*.log` попадают ошибки компиляции — в `*.txt` их нет,
и лог просто обрывается. Полезно и руками:

```
python tools\check_log.py "instance\DayZServer_x64_2026-11-11_07-03-44.RPT"
```

Скрипт печатает последнюю достигнутую стадию, все ошибки с файлом и строкой,
и повторяющиеся `Reason:` падений (например, `Corrupt modstorage header ...`
модstorage CF — это чужие моды, к MusicRadio не относится).

## `mr_audiotypes.c(118): Syntax error` — перенос выражения не работает вообще

Лог `script_2026-11-11_06-48-36.log`:

```
06:48:51.469 SCRIPT (E): @".../mr_audiotypes.c,118": Syntax error
06:48:51.469 SCRIPT (W): @".../mr_audiotypes.c,118": Missing ';' at the end of line
... то же для 119-138 (весь WriteJson)
06:48:51.469 SCRIPT (E): Can't compile "Game" script module!
06:49:03.947 SCRIPT (E): Failed to load game scripts!
```

Строка 118 была такой (`+` в **конце** строки — «эталонная» форма):

```cpp
text = "{\n" +
    "  \"general\": {\n" +
```

**Вывод, который меняет правило: перенос выражения Enforce Script не
принимает ни в какой форме.** Ни `+ ` в начале следующей строки
(`Missing ';'`), ни `+` в конце предыдущей (`Syntax error`). Работает только
**один оператор на одной строке**.

Почему это не всплыло раньше: в тексте мода таких мест больше не было — все
остальные сериализаторы уже были написаны одной строкой. Как только я
«красиво» разбил `WriteJson`, компилятор показал правило напрямую.

Как правильно писать длинные сборки строк:

1. **Несколько присваиваний** (для текста):

```cpp
text = "{\n";
text = text + "  \"general\": {\n";
text = text + "    \"defaultVolume\": " + m_DefaultVolume.ToString() + ",\n";
```

2. **Массив частей + JoinParts** (для провода/RPC, где важна скорость):

```cpp
TStringArray parts = new TStringArray();
parts.Insert("PLAY");
parts.Insert(((int)req.m_HostType).ToString());
...
return MR_Net.JoinParts(parts);
```

Так переписаны `MR_AudioTypes.WriteJson`, `MR_AudioTypes.SerializeConfig`,
`MR_Net.EncodePlay`, `MR_Net.EncodeStop`, `MR_Request.Serialize`,
`MR_DeviceBase.MR_StateLine`, `MR_Library.SerializeManifest`,
`MR_ClientRPC.MR_SetState(...)` (был перенос аргументов).

Проверка в `check_project.py` — `check_expression_wrap()`: любая строка,
кончающаяся на `+ - * / ? && || , (` = ERROR до сборки. Максимальная длина
строки в проекте теперь 170 символов.

## Сервер умирает перед `Module: Game` — а без мода запускается

Диагностический приём, который стоит запомнить: сравните два лога.

```
с модом   : Module: GameLib; ...            <- и конец файла
без мода  : Module: GameLib; Module: Game; Module: World; Module: Mission;
            ... --- Termination successfully completed ---
```

В логе **без** мода сервер не просто дошёл до `Mission` — он прожил ~3 минуты
и завершился штатно (`Termination successfully completed`, 48364 строки).
Значит сам сервер и остальные 40+ модов работают, а падает **компиляция наших
скриптов**: `Module: Game` — это модуль 3_Game всех модов, и он не появляется
ни разу.

Почему не видно причины: при компиляции Game-модуля движок ещё не успевает
писать `SCRIPT (W)` в лог, поэтому файл просто обрывается. Признак, по
которому это ловится: **последняя строка лога — `Module: GameLib`, а
`script_*.log` кончается строкой нашего файла**.

Что было в этой итерации: после починки переносов (см. ниже) я склеил
выражения "в одну строку" — в `MR_AudioTypes.WriteJson` получилось **1200
символов в одной строке**, и компилятор на этом сдавался молча.

Правило: длинную строку разбиваем переносом, где `+` стоит **в конце**
предыдущей строки:

```cpp
text = "{\n" +
    "  \"general\": {\n" +
    "    \"defaultVolume\": " + m_DefaultVolume.ToString() + ",\n" +
```

Так переписаны `MR_AudioTypes.WriteJson`, `MR_AudioTypes.SerializeConfig`,
`MR_Net.EncodePlay/EncodeStop/MR_Request.Serialize`,
`MR_DeviceBase.MR_StateLine`, `MR_Library.SerializeManifest` и длинная
сигнатура `MR_ParseStateLine`. Ограничение закреплено в `check_project.py`
(`check_line_length`, максимум 200 символов).

## `mr_config.c,105: FIX-ME: Only null assignment is allowed on plain data`

Строка есть только в `instance\script_*.log` и `instance\error_*.log`, в
основном `DayZServer_x64_*.txt` её **нет**. Нашлась так:

```
06:21:11.046 SCRIPT (W): @"CodeLock/scripts/3_game/dayzgame.c,13":
                         FIX-ME: Method argument can't be strong reference
06:21:11.062 SCRIPT (W): @"SG_Hideout_Scripts/.../jsonhideoutconfig.c,317":
                         FIX-ME: Method argument can't be strong reference
06:21:11.078 SCRIPT (W): @"MusicRadioCore/Scripts/3_Game/musicradio/mr_config.c,105":
                         FIX-ME: Only null assignment is allowed on plain data
<конец лога, процесс завершился>
```

Было (`MR_AuditLog`):

```cpp
protected static FileHandle m_Handle = 0;
protected static string     m_Path = string.Empty;   // <-- строка 105
```

Статическое поле класса можно инициализировать **только** `0`/`null`. Строка
компилируется, но помечается FIX-ME, и дальше движок работает с полем, которое
объявлено «некорректным». У `FileHandle` те же правила, поэтому проверка
`if (m_Handle == 0)` тоже убрана.

Стало:

```cpp
protected static FileHandle m_Handle;      // только объявление
protected static bool       m_Open;        // признак открытого файла
protected static string     m_Path;

static void Open(string path)
{
    ...
    m_Handle = OpenFile(path, FileMode.WRITE);
    m_Open = (m_Handle != 0);
}
```

Проверка добавлена в `check_project.py` (`check_static_init`): любое статическое
поле с ненулевым инициализатором = ERROR до сборки.

Заметьте: два первых FIX-ME — из чужих модов (`CodeLock`,
`SG_Hideout_Scripts`), они в списке мод-листа и на запуск не влияют. Такие
строки `check_log.py` считает «безобидными» отдельным счётчиком, чтобы наша
ошибка не терялась среди них.

## Сервер пишет `Module: GameLib; ... 32x files` и окно закрывается

Симптом: игра загружается, окно сервера закрывается, в диалоге ничего нет.

Что показывает лог `DayZServer_x64_*.txt`:

```
6:21:10.691 SCRIPT : Module: GameLib; loaded 32x files; 29x classes; ...
6:21:10.698 [Inputs] Loading inputs default "aCarLock/Inputs.xml"
<конец файла>
```

Разбор:

1. Строка `Module: GameLib` — **только начало** компиляции скриптов, причём
   это ванильный модуль (32 файла). Следом должны идти `Module: World` и
   `Module: Mission` — их в логе **нет**, то есть процесс умер посреди сборки
   `4_World`-модуля.
2. Ошибок `SCRIPT (W)` в логе нет вовсе — значит это **не** ошибка
   компиляции скриптов, а падение процесса (или отказ в загрузке модов).
   Ошибку компиляции движок всегда пишет в лог хотя бы одной строкой.

Если лог обрывается так же, а `SCRIPT`-ошибок нет:

1. Смотрите **не** `DayZServer_x64_*.txt`, а:
   * `E:\DayZServer\instance\script_*.log` и `error_*.log` — сюда пишет
     компилятор Enforce Script (FIX-ME, WARNING по нашим .c). В основной лог
     сервера эти строки НЕ попадают — так теряется настоящая причина;
   * `E:\DayZServer\instance\*.RPT` (там стек падения),
   * `E:\DayZServer\instance\*.mdmp` (дамп, если он есть),
   * `E:\DayZServer\instance\battleye\` (отказ BattlEye выглядит именно
     как «сервер поднялся и сразу закрылся», без следа в основном логе).
2. Проверьте подпись: для `verifySign` нужен `@MusicRadio*/keys/KRaTos.bikey`.
   `CHECK.bat`, раздел 7, показывает ключи и подпись `.bisign`.
3. Быстрая проверка «а не мы ли»: запустите сервер с тем же набором модов, но
   **без** `@MusicRadioCore;@MusicRadioDB` и без `MusicRadioServer`:
   * падает так же — виноват другой мод или сам сервер;
   * не падает — присылайте `.RPT`, там будет имя нашего файла.

Автоматически то же самое показывает `CHECK.bat` (раздел 8, «ЛОГИ Сервера»)
и напрямую:

```
python tools\check_log.py "instance\DayZServer_x64_2026-11-11_06-20-55.txt"
```

Скрипт печатает последнюю достигнутую стадию, все ошибки скриптов/конфига и
вердикт одной строкой. Удобно, когда лог на 3700 строк.

## `mr_net.c(128): Broken expression (missing ';'?)`

Второй шаг той же истории, что и `Expected ')'` выше. После починки переносов
игра показала следующий файл и следующую строку:

```
Can't compile "Game" script module!
MusicRadioCore/Scripts/3_Game/musicradio/mr_net.c(128):
Broken expression (missing ';'?)
```

Строка 128 была такой:

```cpp
req.m_Broadcast = (f.Count() > 9) ? MR.ToBool(f.Get(9)) : true;
```

Здесь `?` стоит уже **после** закрытой скобки условия — значит дело не во
вложенности, а в самой конструкции: **тернарный оператор `?:` этот разборщик
не принимает**. Он ждёт `)` или `;` и на `?` сдаётся.

Убраны все пять тернарников в проекте (в `MR_Net.c`, `MR_MediaItems.c` ×2,
`MR_WorldHooks.c`, `MR_RadioMenu.c`, `MR_ServerEngine.c`), каждый — в обычный
`if/else`:

```cpp
// было
return tape ? MR_MediaItems.CarrierLabel(tape) : "(empty)";
// стало
if (tape)
    return MR_MediaItems.CarrierLabel(tape);
return "(empty)";
```

`check_project.py` → `check_ternary()` теперь запрещает `?:` целиком: если
появится снова, сборка скажет

```
ERROR @MusicRadioCore/Scripts/...:128: тернарный '?:' - движок ответит
'Broken expression (missing ';'?)'. Перепишите через if/else
```

## Практика: почему ошибки идут по одной

Обе ошибки (`Expected ')' not a '0'`, `Broken expression`) — это один и тот же
разборщик, который останавливается на первой проблеме. Лечение шло шаг за
шагом: сначала 75 переносов с оператором в начале строки, потом тернарники.
Если после пересборки появится ещё один `SCRIPT (W)` в логе — присылайте лог,
номер строки в диалоге игры может снова указывать на соседний файл, а
настоящая причина будет строкой выше в `DayZServer_x64_*.txt`.

## `Can't compile "Game" script module` + `Expected ')', not a '0'`

Диалог игры:

```
Can't compile "Game" script module!
MusicRadioCore/Scripts/3_Game/musicradio/mr_clientaudio.c(109):
    Expected ')', not a '0'
Failed to load game scripts
```

**Номер файла и строки в диалоге — не там, где ошибка.** Настоящая причина
видна только в логе сервера:

```
SCRIPT (W): @"MusicRadioCore/Scripts/3_Game/musicradio/mr_audiotypes.c,114":
            Missing ';' at the end of line
... то же для строк 115-137 и 236-240
```

В `MR_ClientAudio.c` ошибки не было вовсе. Так выглядел перенос выражения, где
оператор стоит **в начале** следующей строки:

```cpp
// ТАК НЕЛЬЗЯ (строки 114-137 MR_AudioTypes.c):
text = "{\n"
    + "  \"general\": {\n"
    + "    \"defaultVolume\": " + m_DefaultVolume.ToString() + ",\n"
```

Enforce Script читает оператор в начале строки как новое выражение, сообщает
`Missing ';'` и продолжает разбор уже сбитым — поэтому итоговая ошибка
всплывает в другом файле и на другой строке. Правило движка:

* оператор в **конце** строки (`text = "{\n" +` … перенос) — **можно**;
* оператор в **начале** следующей строки — **нельзя**;
* комментарий между частями выражения — тоже нельзя (разрывает выражение).

Исправлено: 75 таких мест в 9 файлах собраны в одну строку (`MR_AudioTypes.c`,
`MR_Net.c`, `MR_Library.c`, `MR_ClientAudio.c`, `MR_Config.c`,
`MR_DeviceBase.c`, `MR_ClientState.c`, `MR_RadioMenu.c`, `MR_ServerEngine.c`).
Проверка добавлена в `check_project.py` (`check_operator_position`), так что
до сборки будет:

```
ERROR @MusicRadioCore/Scripts/3_Game/MusicRadio/MR_Net.c:60: строка начинается
с оператора '+' - движок скажет 'Missing ';' at the end of line' и покажет
ошибку в другом файле. Соберите выражение в одну строку
```

Практический вывод: если игра ругается на `Expected ...` в файле X, сначала
ищите `Missing ';' at the end of line` в `DayZServer_x64_*.txt` — настоящая
ошибка обычно в предыдущем файле журнала.

## Раскладка настоящего PBO (проверено на сборке AddonBuilder 1.0.240639)

```
[00]  "\0sreV"                 5 байт сигнатуры
[05]  блок версий              mimeType 'Vers' + 4 x uint32   -> 17 байт
[22]  "\0"                     конец блока версий
[23]  свойства                 "prefix\0MusicRadioCore\0product\0..." , "\0"
[?]   записи                   name\0 + 5 x uint32
                               (packingMethod, originalSize, RESERVED,
                                timeStamp, dataSize)   ... пустое имя "\0"
[?]   PENTRY + 16 нулей        20 байт между записями и данными
[?]   данные файлов            подряд, в порядке записей
[EOF] "\0" + 20 байт           подпись SHA-1 содержимого (может отсутствовать)
```

Четыре подводных камня, на которых разбор спотыкался (все четыре давали
одинаковый ложный вывод — «PBO не читается» на ЦЕЛОМ файле):

1. **записи начинаются не сразу после `sreV`**, а на 21-м байте: между ними
   блок версий (5 + 17). Прежняя версия пробовала 16/20 нулей и «блок
   заголовков» — но не блок версий, поэтому на настоящем PBO не находила
   записей вообще;
2. **третье поле записи — `reserved`, а не offset** (в настоящих PBO обычно 0);
3. **после последней записи стоит маркер `0x556E6971` и ещё 16 нулей**, то есть
   данные начинаются на 20 байт позже конца записей. Если считать «данные сразу
   после записей», первый файл читается со сдвигом — отсюда прежнее
   «содержимое первого файла не совпало с исходником»;
4. **в конце файла `0x00` + 20 байт SHA-1**, которых нет ни в одной записи.
   Поэтому `длина файла − сумма размеров` промахивается на 21 байт; правильная
   формула — `длина − сумма − 21`.

Как теперь устроен разбор (`tools/inspect_pbo.py`):

* блок записей **ищется сканированием** окна после сигнатуры (`find_entries`):
  первый сдвиг, на котором читаются имена с расширениями и пятиuint32-записи
  до пустого имени. Никакой арифметики «сколько тут нулей» — она и была
  источником всех промахов;
* кандидаты на начало данных проверяются **содержимым** файлов из
  `release\staging` (сравнение с исходником), а не «похожестью»;
* варианты заголовка (с блоком версий, без него, без маркера, без SHA-1,
  `reserved != 0`) закреплены в `selftest_pbo.py` — девять случаев, все должны
  читаться. Тест собирает PBO ровно так, как это делает AddonBuilder.

Проверить руками: `python tools\inspect_pbo.py "release\pbo\MusicRadioCore.pbo" --list`
— должен показать 35 файлов и `config.cpp` с тем же числом строк, что в проекте.

## `! MusicRadioCore: PBO не читается (...)` в конце BUILD_PBO.bat

Это была ошибка **читалки**, а не сборки: AddonBuilder писал `Build Successful`,
а следующий за ним `check_pbo.py` не мог разобрать заголовок и печатал простыню
причин. С revision `20261128` разбор идёт сканированием (см. раздел выше), а
причины печатаются коротко (`_short_problems`): не более четырёх.

Если сообщение всё же появилось — смотрите не на «нулей 16/20», а на строку
`блок записей не найден`: она означает, что файл не PBO (или обрезан при
копировании). Проверьте размер: `MusicRadioDB.pbo` весит ~83 МиБ.

## `! ...: нет .bisign рядом с PBO`

`check_pbo.py` требует подпись рядом с каждым `.pbo`. Если вы собирали PBO не
через `BUILD_PBO.bat`, а вручную (AddonBuilder без `-sign`), подписи нет —
сервер с `verifySign=2` такой мод не пустит. Лечится пересборкой через
`BUILD_PBO.bat` (он передаёт `-sign=<ключ>`), либо отключением проверки
подписей на сервере.

## `. carriers.json: no 'tracks' list, skipped` — больше не ошибка

`carriers.json` — это **выход** генератора (описание альбомов для мода), а не
плейлист. Раньше он попадал в общий обход `@MusicRadioDB\Music\*.json` и
печатал `no 'tracks' list, skipped`, что выглядело как поломка при полностью
рабочем проекте. С revision `20261128` генератор отличает плейлист от
служебного файла (`is_playlist()`: есть `tracks` или `channels`), а причины
пропуска печатает строкой с точкой в начале:

```
  . @MusicRadioDB/Music/carriers.json: не плейлист (нет 'tracks' и 'channels')
  . @MusicRadioDB/Music/index.json: генерируется этим скриптом
```

## `'...python.exe" -c "import' is not recognized`

Это не Python и не проект - это лишняя кавычка в батнике. Было:
`set "PYC="%PY%""` - значение получало вторую пару кавычек, и cmd видел
команду `"C:\...\python.exe"" -c "import ...`, которой не существует.
Симптомы рядом: `Python : ... (версия )` (пусто) и `[ВНИМАНИЕ] набор
скриптов в tools\ не тот` при полностью правильных скриптах.

`check_bat.py` теперь ловит оба случая: `set "X=""путь"""` и вызов
`%VAR% ""путь""`.

## Если CHECK.bat пишет `config.cpp нет внутри PBO`

Так было ровно один раз, и причина была в разборщике, а не в сборке: у
настоящего PBO от AddonBuilder после сигнатуры `\0sreV` идёт 4 байта, а
потом уже записи файлов; плюс бывает блок версий `\0Vers`. Раньше парсер
ждал имя файла сразу после сигнатуры и на настоящем PBO находил `0 файлов`.

Сейчас `inspect_pbo.py` пробует несколько вариантов начала блока, проверяет
последовательность offsets и берёт данные как `len(файл) - сумма размеров`.
Работает на всех четырёх вариантах заголовка (с блоком версий и без, с 0 и 4
байтами после сигнатуры) - это закреплено тестом.

Если всё же видно `0 файл(ов)` - файл не PBO или обрезан.

## Три команды, которые отвечают на все вопросы про сервер

```bat
REM 1. что внутри ВСЕХ копий мода на сервере (главное)
python tools\check_pbo.py --scan

REM 2. что внутри одного файла (путь в кавычках)
python tools\check_pbo.py "E:\DayZServer\@MusicRadioCore\addons\MusicRadioCore.pbo" --line 262

REM 3. что внутри локальной сборки
python tools\check_pbo.py
```

`--scan` сам находит папку сервера и печатает каждую копию `MusicRadio*.pbo`
(включая `!Workshop`, `ServerMOD`, `ServerCARS`) с числом строк `config.cpp`,
состоянием `requiredAddons` и вердиктом «совпадает с проектом / СТАРАЯ сборка».
Ошибка `line 262` относится к **одной** из этих копий — видно к какой.

## `Undefined base class 'Inventory_Base'` — настоящая причина найдена

Лог сервера (2026-11-11, 05:22):

```
File MusicRadioCore\config.cpp, line 262:
/CfgVehicles.MR_CassetteCase_Base: Undefined base class 'Inventory_Base'
```

`Inventory_Base` - ванильный класс, и он **был** объявлен в файле... но в
другом блоке:

```cpp
class CfgVehicles        // строка 101: носители-проигрыватели
{
    class Inventory_Base;
    class MR_MemoryCard_Base : Inventory_Base { ... };
};

class CfgVehicles        // строка 289: кассеты/MiniDisc
{
    // <- здесь объявления НЕ было, а MR_CassetteCase_Base : Inventory_Base
};
```

Видимость в `config.cpp` ограничена блоком: объявление из соседнего
`class CfgVehicles` не помогает. Поэтому в блоке MEDIA CARRIERS теперь есть
собственное `class Inventory_Base;`.

Что сделано:

* генератор вставляет объявление при создании блока MEDIA CARRIERS
  (`_make_section()`);
* `ensure_base_decls()` дописывает его в любой блок, где объявлены
  `MR_CassetteCase_Base` / `MR_MiniDisc_Base`, но самого объявления нет;
* `check_project.py` (`check_base_declarations()`) проверяет каждый такой
  блок и падает с точной формулировкой, если объявления нет - чтобы это не
  всплывало снова только в логе сервера.

## `Undefined base class 'Inventory_Base'` — что смотреть первым

`Inventory_Base` — **ванильный** класс из `DZ_Data`. Если он «не определён»,
значит в загруженном `config.cpp` нет правильного `CfgPatches` и
`requiredAddons[] = { "DZ_Data", ... }`, либо игра загрузила совсем другой файл.

Порядок действий (не гадать, а посмотреть внутрь PBO):

```bat
python tools\check_pbo.py
```

```
MusicRadioCore: 35 файл(ов), 162 KiB
  config.cpp      : 441 строк (config.cpp)
  requiredAddons  : ok (DZ_Data есть)
  MR_CassetteCase_Base: строка 294, порядок верный
  совпадает с проектом: да
  подпись         : MusicRadioCore.pbo.KRaTos.bisign
```

Если `config.cpp в PBO не совпадает с @MusicRadioCore/config.cpp` — на сервер
уехала старая сборка, лечится `BUILD_PBO.bat` + `DEPLOY.bat`.
Если `requiredAddons без DZ_Data` — PBO собран из файла, где `CfgPatches` был
неполным (так бывает, если `.pbo` собрали из чужой папки или из старого
`staging`).

Отдельно: номер строки в ошибке игры ничего не доказывает. Игра пишет
`line 262`, а в актуальном файле на этой строке может быть совсем другое —
именно поэтому `inspect_pbo.py` показывает содержимое, а не номер.

Ещё один симптом того же класса: `Undefined base class 'MR_CassetteCase_Base'`
при верном порядке — значит в файле объявлен не тот базовый класс
(`MR_CassetteCase_Base : Inventory_Base` переименован или потерян). `CHECK.bat`
показывает это строкой `MR_CassetteCase_Base: строка N - порядок верный`.

## Ключи подписи: где их ищут батники

`BUILD_PBO.bat` находит приватный ключ **сам**, в таком порядке:

1. `release\keys\KRaTos.biprivatekey`
2. корень проекта
3. папка, из которой запущен `.bat`
4. `Documents` / `Desktop`

Понимаются имена `KRaTos` (в любом регистре) и старое `musicradio` — так что
переименовывать ничего не нужно. Публичный `KRaTos.bikey` ищется там же и
раскладывается `DEPLOY.bat` в `@MusicRadioCore\keys\` и `@MusicRadioDB\keys\`.
Проверить всё сразу: `CHECK.bat`, раздел **4. КЛЮЧИ ПОДПИСИ**.

Что должно получиться в `release\pbo\`:

```
MusicRadioCore.pbo       MusicRadioCore.pbo.KRaTos.bisign
MusicRadioServer.pbo     MusicRadioServer.pbo.KRaTos.bisign
MusicRadioDB.pbo         MusicRadioDB.pbo.KRaTos.bisign
```

Имена важны: расширение `.bisign` должно совпадать с именем файла `.bikey`
на сервере (`DayZServer\keys\KRaTos.bikey`). Если приватный ключ `KRaTos`,
а публичный лежит как `musicradio.bikey` — это, скорее всего, ключи из разных
пар, и сервер ответит `Signature verification failed`. `CHECK.bat` про такое
предупреждает отдельной строкой.

Пароля у `.biprivatekey` нет: DSSignFile создаёт ключ без пароля, поэтому
`-sign=` в `BUILD_PBO.bat` работает молча, без запросов в консоли.

## Батник падает с трейсбеком: `stage_pbo.py, line 437` в файле, где 360 строк

Значит в `tools\` лежат **старые .py**, а .bat — новые. Это бывает, когда
распаковывают только `.bat` из архива `.bat`-файлов поверх проекта.

Признак: в трейсбеке номера строк больше, чем всего строк в файле, или функции
называются иначе, чем в текущей версии (`make_zip`, `write_config_cpp`, ...).

Проверка одной командой:

```bat
python tools\check_tools.py
```

```
! stage_pbo.py без TOOLS_REVISION - это СТАРАЯ версия скриптов
  Что сделать: распаковать MusicRadio_ARCHIVE.zip ЦЕЛИКОМ с заменой
  (не только .bat): .bat и .py должны быть из одного архива.
```

То же проверяют `REBUILD.bat` (шаг 2b) и `CHECK.bat` (раздел 0) — они
останавливаются и прямо говорят про неполную распаковку. `check_project.py`
пишет в итоге строку `tools: revision 20261111, 9 script(s) present`.

Правило простое: **`.bat` и `.py` берутся из одного архива**.

## Что уже сделано, чтобы это не повторялось

* `BUILD_PBO.bat` перед сборкой спрашивает у `check_fresh.py --stage`, свежий
  ли staging, и при расхождении **сам** запускает `stage_pbo.py --clean`.
  Нет Python — честно предупреждает и спрашивает, продолжать ли.
* `DEPLOY.bat` перед раскладкой проверяет `check_fresh.py --pbo`: если staging
  новее собранных PBO, сначала пересобирает их, а не копирует вчерашние.
* `stage_pbo.py` ставит скопированным файлам свежий mtime — иначе сравнение
  дат «staging vs PBO» врало бы в пределах одной секунды.
* `REBUILD.bat` в конце печатает сверку `check_fresh.py`.
* `check_db_config.py` ловит саму ошибку `Member already defined` заранее:
  считает не имена, а **пути** классов (`CfgSoundSets/MR_..._SoundSet`), поэтому
  легальные одноимённые `MusicRadioDB` внутри `CfgPatches` и `CfgMods` не
  считаются дублем.

## Если `CHECK.bat` говорит «всё свежее», а ошибка в игре та же

Тогда дело не в проекте, а в том, **откуда игра берёт мод**:

1. **Старый `.pbo` на сервере.** `release\deploy\@*` — это заготовка, её нужно
   скопировать в `DayZServer\@*` (и в `keys\` — `KRaTos.bikey`). Скрипты
   этого не делают: они не знают, где у вас сервер.
2. **Мод подключён дважды** — старая копия лежит в `!Workshop` / `@Steam` /
   другой папке и подключается параметром `-mod=`. Проверьте `Start.bat`:
   ```
   -mod=@MusicRadioCore;@MusicRadioDB
   ```
   В списке должны быть ровно те папки, которые вы обновили.
3. **Клиентский и серверный мод разошлись.** `@MusicRadioDB` нужен и серверу,
   и клиенту — с одинаковым содержимым. Обновили только сервер — клиент
   продолжит падать или не услышит музыку.
4. **Лог читается не тот.** Ошибка конфига пишется в
   `DayZServer\profiles\*.RPT` (у вас в папке это файлы `error_*.log`
   и `*.RPT`), а окно-«табличка» показывает лишь последнюю строку из него.
   Смотрите самый свежий файл по времени.

Проверить, что лежит внутри `.pbo`, можно PBO Manager'ом или прямо в игре по
строке `MusicRadioDB` в списке модов лончера — версия должна совпадать с
`meta.cpp` (`version = "1.1.0"`).

## Как это выглядит в логе, когда всё в порядке

```
AddonBuilder: MusicRadioCore  ->  OK
AddonBuilder: MusicRadioServer -> OK
AddonBuilder: MusicRadioDB     -> OK
```
и в RPT нет строк `Member already defined`, `Undefined base class`,
`some input after EndOfFile`. Если такие строки остались при «свежих» файлах —
значит загрузился `.pbo` из пункта 1–3 выше.

## Сервер упал и оставил `.mdmp` — что это значит (раздел 9)

Симптом: сервер закрывается без сообщения, в RPT последняя строка обрывается на
полуслове, а рядом с exe появляется файл вида
`DayZServer_x64_2026-11-11_18-13-33.mdmp` на полмегабайта. Открывать его
блокнотом бесполезно: это бинарный минидамп Windows, и внутри нет текста.

Что делать (ничего не устанавливать не нужно):

```
CHECK.bat --mdmp
```

или напрямую, если дамп лежит не в папке сервера:

```
python tools\read_mdmp.py "E:\DayZServer\DayZServer_x64_2026-11-11_18-13-33.mdmp"
python tools\read_mdmp.py --scan E:\DayZServer     (все дампы по порядку)
```

Скрипт отвечает на три вопроса и ни на один лишний:

* **где упало** — адрес исключения переводится в `модуль+смещение`, например
  `DayZServer_x64.exe+0x2476EF`. Это единственная строка, которую имеет смысл
  искать в changelog DayZ и прикладывать к баг-репорту;
* **что было загружено** — список DLL. Если в нём есть модуль из папки мода
  (`-mod`), падение может быть в нативном коде мода; если нет — мод как
  скриптовый аддон к падению непричастен;
* **можно ли верить стеку** — почти всегда нет. После записи дампа
  (`MinidumpWriteDump`) кадры упавшего потока перезаписаны обработчиком
  (`dbgcore.dll`, `ucrtbase.dll`), символов нет, память усечена до ~0.5 МБ.
  Скрипт так и пишет, вместо того чтобы выдать мусор за стек вызовов.

Пример разбора реального дампа (11.11.2026, наш сервер):

```
исключение: ACCESS_VIOLATION - чтение/запись по плохому адресу
где       : E:\DayZServer\DayZServer_x64.exe+0x2476EF  (адрес 0x7FF63A9A76EF)
нативный код модов: не найден
```

Вывод: `0xC0000005` внутри кода самого движка, сторонних DLL нет, ошибок
скриптов перед падением в RPT нет. Скриптовый мод так не падает — это проблема
движка (сверить версию и смещение с changelog), а не `MusicRadio`.

Формат разобран в самом скрипте: оглавление по `RVA` в заголовке, потоки
`Exception` (6), `ModuleList` (4), `ThreadList` (3), `SystemInfo` (7).
Пригодится, если понадобится добавить поля: смещения записаны в шапке
`tools/read_mdmp.py`.

## `Can't find variable 'TAPE_MARKER'` — «проект чистый, а сборка падает»

Симптом: `CHECK.bat` пишет `OK - no errors, 0 warning(s)`, а игра на стадии
**Game** показывает

```
MusicRadioCore/Scripts/3_Game/MusicRadio/MR_CarrierState.c,172:
Can't find variable 'TAPE_MARKER'
```

Две отдельные причины, и обе были 11.11.2026 сразу:

1. **Скрипты в `.pbo` старые.** `MR_TAPE_MARKER`/`MR_TAPE_SIDE_MAX` — это
   `MR.TAPE_MARKER` и `MR.TAPE_SIDE_MAX` из `MR_Constants.c`. В проекте они
   есть, в собранном `.pbo` — нет. Раньше `CHECK.bat` сверял на сервере только
   `config.cpp`, а скрипты не сверял вообще: `config.cpp` совпадал, «всё
   свежее», а компилировался старый `.c`.
   Теперь `check_fresh.py` сравнивает **все 26 `.c`** внутри `.pbo` с
   проектными и пишет `скрипты: PBO содержит ДРУГИЕ скрипты (1 из 26:
   MR_Constants.c)`. Лечение всегда одно: `BUILD_PBO.bat` → `DEPLOY.bat` →
   перезапуск сервера.

2. **Ссылка на несуществующую константу.** Вторая половина той же ошибки — на
   строке 204, 205, 230, 231. Проверка `check_constants()` в
   `check_project.py` сверяет каждый `MR.ИМЯ` в скриптах с тем, что реально
   объявлено внутри `class MR`, и печатает файл и строку до сборки.

Как проверить руками, что в `.pbo` именно то, что в исходниках:

```
python tools\check_fresh.py            (строка «скрипты: …» по каждому модулю)
python tools\inspect_pbo.py release\pbo\MusicRadioCore.pbo Scripts\3_Game\MusicRadio\MR_Constants.c
```

### Почему `MR.L` и `MR.C` в этой проверке не ругаются

В логе 18:29 после `TAPE_*` были ещё

```
mr_library.c,351: Incompatible parameter 'handle'
mr_config.c,136: Incompatible parameter 'm_Handle'
```

Это не новые ошибки, а **повторные** — те же строки, что чинились в `d71397e`
(`bool found = FindNextFile(...)`, `if (!m_Handle)`). Компилятор Game падает на
первых ошибках и до 4_World не доходит, поэтому в логе остались только 3_Game.
Ещё один повод сверить скрипты в `.pbo`, а не верить `CHECK.bat`.

**Наша доля в том логе — 9 строк из 97.** Остальные 88 — `Gamazer_World_Menu`
и `KGB_LIB_CLIENT`; они компилируются первыми, но к нашим файлам отношения не
имеют.

## `CHECK.bat` пишет «Всё чисто», а лог полон ошибок

Так было до ревизии 20261145: `fresh_real` считался только по уровню
«исходники → staging», а раздел с логами на итог не влиял вообще. Поэтому итог
мог быть «чисто» при семи наших ошибках компиляции наверху.

Что теперь:

* итог учитывает раздел 8 (логи) и найденный `.mdmp`;
* `crosscheck_log()` сверяет ошибки из лога с **текущими** исходниками и пишет
  прямо: «в логе есть ошибки, которых НЕ МОЖЕТ быть в этих исходниках» — то
  есть компилировался другой `.pbo`, а не этот проект;
* `check_fresh.py` печатает по каждому модулю строку `скрипты: …`
  (`совпадают` / `PBO содержит ДРУГИЕ скрипты (1 из 26: MR_CarrierState.c)`);
* `python tools\check_fresh.py` теперь возвращает ошибку и тогда, когда
  исходники со staging согласованы, но `.pbo` на сервере собран из других.

## `CHECK.bat` не видит `.mdmp`

Раздел с дампом искал файлы только в корне папки сервера, а DayZServer пишет
их в **профиль** — `E:\DayZServer\instance\`. Теперь просматриваются корень,
`instance`, `profiles`, `logs` и `instance\Users\*`, а раздел печатается
автоматически, если хоть один дамп найден (ключ `--mdmp` остался для случая
«хочу раздел даже когда дампов нет»).

Проверить, где ищет:

```
python tools\read_mdmp.py --where
```

## «ОК» из старых tools — почему проверки врали

11.11.2026, три прогона подряд: `check_fresh.py` пишет «ОК — исходники, staging
и PBO согласованы», `check_pbo.py` — «ОК — 3 PBO, содержимое и подписи в
порядке», а сервер падает на `mr_library.c:351  Incompatible parameter 'handle'`.

Причина не в скриптах. В `P:\mods\MusicRadio\tools` лежали **старые** `.py`:
ни `check_fresh.py`, ни `check_pbo.py` не содержали проверок, добавленных в
ревизиях 20261144–20261146. Их «ОК» означало «я не умею это проверять», а не
«всё хорошо». Узнаётся по двум признакам в выводе:

| чего нет в выводе | какие tools старые |
|---|---|
| строка `скрипты: …` под каждым модулем | `check_fresh.py` < 20261144 |
| строка `файлов : N (скриптов .c: M)` | `check_pbo.py` < 20261146 |

Проверить точно: `python tools\check_tools.py` — он печатает таблицу
«файл → ревизия по отпечаткам функций» и прямо говорит, какой `.py` не тот.

Лечение: **удалить папку `tools\`** и распаковать архив заново. Просто
«скопировать сверху» недостаточно: рядом остаются `.py`, которых в новой
ревизии уже нет, и проверки продолжают бежать со старыми.

Тот же признак «компилируется другое» — расхождение в цифрах сборки:

```
проект:  @MusicRadioCore\config.cpp      399 строк, staging 43 файла
в PBO:   config.cpp                      391 строка,  35 файлов
```

`AddonBuilder` в этом случае всё равно пишет `Build Successful`. Ловится
строкой `файлов: 35 (скриптов .c: 21), в staging 43` из `check_pbo.py`.

## Почему архив «не обновился»: он распаковался ЭТАЖОМ НИЖЕ

11.11.2026, после «сделал всё как написано»:

```
P:\mods\MusicRadio>python tools\check_tools.py
  ожидаемая ревизия: 20261141
  . скрипты: ревизия 20261141
OK - скрипты на месте и той же ревизии, что батники
```

Причина: внутри архива был верхний каталог `MusicRadio/`. Распаковка в
`P:\mods\MusicRadio` дала `P:\mods\MusicRadio\MusicRadio\tools\…` — новые
скрипты легли этажом ниже, старые остались в `P:\mods\MusicRadio\tools\`,
и `CHECK.bat` продолжал запускать старые.

Что изменено в ревизии 20261148:

* **архив теперь плоский**: внутри нет папки `MusicRadio`, файлы лежат в
  корне архива, распаковка «поверх папки проекта» однозначна;
* после распаковки в корне проекта должны лежать `check_tools.py`,
  `EXPLAIN_STALE.bat` и папка `tools\` — если появилась папка
  `P:\mods\MusicRadio\MusicRadio`, распаковали не туда;
* `check_tools.py` печатает таблицу «файл → ревизия по отпечаткам» — видно,
  какой именно `.py` остался старым;
* `check_tools.py` кладёт `TOOLS_STALE.flag` в корень проекта, а `CHECK.bat`
  при наличии этого файла **сразу останавливается** и отправляет в
  `EXPLAIN_STALE.bat`, вместо того чтобы печатать «ОК» по разделам;
* `EXPLAIN_STALE.bat --fix` переименовывает `tools\` в `tools_old_<время>` —
  чтобы новый архив лёг на освободившееся место, а не рядом.

## 12. `Member already defined` на ванильной машине (слот в автомобиле)

Симптом в логе игры:

```
File MusicRadioCore\config.cpp, line 338:
/CfgVehicles.CivilianSedan: Member already defined.
```

Причина: попытка «дополнить» ванильную машину строкой
`class CivilianSedan : CarScript { attachments[] += { "MR_DashSlot" }; };`.
В Enforce-конфиге такой конструкции нет: `class X : Parent` создаёт **новое**
определение, а не дополнение существующего, поэтому парсер видит два члена с
одним именем внутри одного `CfgVehicles`.

Как сделано сейчас:

* машины в `config.cpp` **не объявляются вовсе**;
* слот живёт только в `CfgSlots → Slot_MR_DashSlot`
  (`name = "MR_DashSlot"`, `selection = "MR_DashSlot"`);
* в слот кладут кассету, мини-диск **или** плеер —
  `MR_MediaItems.IsCarDockItem()` возвращает true для носителя и для плеера,
  `AlbumInVehicle()` берёт альбом у того, что лежит в слоте;
* `tools/selftest_no_keys_in_servermod.py` (`check_car_dash_slot`) падает, если
  кто-то снова начнёт объявлять `CivilianSedan : ...` и т.п.

Проверка негативным тестом: строки возвращены — тест выдал
`FAIL - config.cpp снова определяет CivilianSedan ...`, после восстановления —
`OK`.

**Что осталось:** предмет в слоте не рисуется на торпеде — для этого нужна
селекция `MR_DashSlot` в `.p3d` машины и прокси-модель (см. `docs/07_MODELS.md`,
п. 6). Вставить/вынуть и слушать музыку можно уже сейчас.
