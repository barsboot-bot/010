@echo off
setlocal enabledelayedexpansion

echo =========================================================
echo AUTO_BUILD.bat — вся цепочка одной командой
echo =========================================================

:: Проверка устаревших инструментов
if exist TOOLS_STALE.flag (
    echo ОШИБКА: Инструменты устарели (TOOLS_STALE.flag)
    echo Запустите: EXPLAIN_STALE.bat --fix
    pause
    exit /b 1
)

:: Анализ параметров
set "DEPLOY=0"
set "NO_BUILD=0"

:parse_args
if "%1"=="" goto end_parse
if /i "%1"=="--deploy" (
    set DEPLOY=1
    shift
    goto parse_args
)
if /i "%1"=="--no-build" (
    set NO_BUILD=1
    shift
    goto parse_args
)
shift
goto parse_args
:end_parse

:: Вызов автосборки через Python
echo Запуск автосборки...
python tools\autobuild.py --deploy %DEPLOY% --no-build %NO_BUILD%
if errorlevel 1 (
    echo ОШИБКА при автосборке
    pause
    exit /b 1
)

:: Если --deploy — запуск DEPLOY.bat
if %DEPLOY%==1 (
    echo Запуск развертывания...
    call DEPLOY.bat
)

echo =========================================================
echo AUTO_BUILD завершён
echo =========================================================
pause
exit /b 0
