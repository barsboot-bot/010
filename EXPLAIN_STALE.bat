@echo off
setlocal enabledelayedexpansion

echo =========================================================
echo EXPLAIN_STALE.bat — объясняет, почему tools устарели
echo =========================================================

if exist TOOLS_STALE.flag (
    echo ПРИЧИНА: Инструменты (tools/) устарели.
    echo Ревизия инструментов в файле TOOLS_STALE.flag отличается от текущей.
    echo 
    echo РЕШЕНИЕ:
    echo 1. Распакуйте архив с новыми инструментами
    echo 2. Запустите с ключом --fix, чтобы обновить
    
    if /i "%1"=="--fix" (
        echo Запуск обновления...
        python tools\update_tools.py
        if errorlevel 1 (
            echo ОШИБКА при обновлении
            pause
            exit /b 1
        )
        del TOOLS_STALE.flag 2>nul
        echo Инструменты обновлены.
    )
) else (
    echo Инструменты актуальны (TOOLS_STALE.flag не найден)
)

echo =========================================================
echo EXPLAIN_STALE завершён
echo =========================================================
pause
exit /b 0
