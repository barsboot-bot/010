@echo off
setlocal enabledelayedexpansion

echo =========================================================
echo IMPORT_MUSIC.bat — импорт музыки
echo =========================================================

if "%1"=="" (
    echo Использование: IMPORT_MUSIC.bat "путь_к_трекам" "альбом" "жанр"
    echo Пример: IMPORT_MUSIC.bat "I:\музыка\1" "Kos" "Pop"
    pause
    exit /b 1
)

set "SOURCE=%1"
set "ALBUM=%2"
set "GENRE=%3"

if "%ALBUM%"=="" set "ALBUM=Unknown"
if "%GENRE%"=="" set "GENRE=Unknown"

echo Запуск импорта музыки...
echo Источник: %SOURCE%
echo Альбом: %ALBUM%
echo Жанр: %GENRE%

python tools\import_music.py --source "%SOURCE%" --album "%ALBUM%" --genre "%GENRE%" --stage
if errorlevel 1 (
    echo ОШИБКА при импорте музыки
    pause
    exit /b 1
)

echo =========================================================
echo IMPORT_MUSIC завершён
echo =========================================================
pause
exit /b 0
