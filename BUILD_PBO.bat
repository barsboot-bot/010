@echo off
setlocal enabledelayedexpansion

echo =========================================================
echo BUILD_PBO.bat — сборка и подпись PBO
echo =========================================================

:: 1. Поиск DayZ Tools
set "TOOLS_PATH="
:: Проверка стандартных путей Steam
if exist "C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\Bin\AddonBuilder\AddonBuilder.exe" (
    set "TOOLS_PATH=C:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\Bin\AddonBuilder\AddonBuilder.exe"
) else if exist "D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\AddonBuilder\AddonBuilder.exe" (
    set "TOOLS_PATH=D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\AddonBuilder\AddonBuilder.exe"
) else (
    echo Деньги не найдены DayZ Tools. Укажите путь к AddonBuilder.exe:
    set /p TOOLS_PATH="Путь: "
)

if not exist "!TOOLS_PATH!" (
    echo ОШИБКА: AddonBuilder.exe не найден по пути: !TOOLS_PATH!
    pause
    exit /b 1
)

echo Найден AddonBuilder: !TOOLS_PATH!

:: 2. Проверка ключа
set "KEY_PATH=release\keys\KRaTos.biprivatekey"
if not exist "!KEY_PATH!" (
    echo Предупреждение: ключ !KEY_PATH! не найден.
    echo Сборка будет без подписи (или с ошибкой, если AddonBuilder требует ключ).
    echo Создайте ключ через DayZ Tools → DSSignFile.
    pause
)

:: 3. Проверка staging (если устарел — обновить)
python tools\stage_pbo.py --check
if errorlevel 1 (
    echo Staging устарел. Обновляем...
    python tools\stage_pbo.py --clean
    if errorlevel 1 (
        echo ОШИБКА при обновлении staging
        pause
        exit /b 1
    )
)

:: 4. Сборка PBO (AddonBuilder)
echo Сборка MusicRadioCore...
"!TOOLS_PATH!" -source="release\staging\MusicRadioCore" -prefix="MusicRadioCore" -output="release\pbo" -clear -packonly -sign="!KEY_PATH!"
if errorlevel 1 (
    echo ОШИБКА при сборке MusicRadioCore
    pause
    exit /b 1
)

echo Сборка MusicRadioDB...
"!TOOLS_PATH!" -source="release\staging\MusicRadioDB" -prefix="MusicRadioDB" -output="release\pbo" -clear -packonly -sign="!KEY_PATH!"
if errorlevel 1 (
    echo ОШИБКА при сборке MusicRadioDB
    pause
    exit /b 1
)

echo Сборка MusicRadioServer (без подписи)...
"!TOOLS_PATH!" -source="release\staging\MusicRadioServer" -prefix="MusicRadioServer" -output="release\pbo" -clear -packonly
if errorlevel 1 (
    echo ОШИБКА при сборке MusicRadioServer
    pause
    exit /b 1
)

echo =========================================================
echo BUILD_PBO завершён. PBO находятся в release\pbo
echo =========================================================
pause
exit /b 0
