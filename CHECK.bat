@echo off
setlocal enabledelayedexpansion

echo =========================================================
echo CHECK.bat — диагностика
echo =========================================================

echo --- Проверка свежести цепочки ---
python tools\check_fresh.py

echo --- Проверка проекта ---
python tools\check_project.py

echo --- Проверка батников ---
python tools\check_bat.py

echo --- Чтение логов сервера ---
if exist "DayZServer_x64_*.RPT" (
    for %%f in ("DayZServer_x64_*.RPT") do (
        echo Чтение лога: %%f
        type "%%f" | findstr /i "MusicRadio"
    )
) else (
    echo Логи сервера не найдены (DayZServer_x64_*.RPT)
)

if exist "instance\*.log" (
    echo Чтение логов скриптов в instance\
    type "instance\*.log" | findstr /i "MusicRadio"
)

:: Проверка дампов (если указан ключ)
if /i "%1"=="--mdmp" (
    echo Поиск дампов...
    if exist "*.mdmp" (
        for %%f in ("*.mdmp") do (
            echo Анализ дампа: %%f
            python tools\check_mdmp.py "%%f"
        )
    ) else (
        echo Дампы не найдены
    )
)

echo =========================================================
echo CHECK завершён
echo =========================================================
pause
exit /b 0
