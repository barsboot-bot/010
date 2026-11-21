@echo off
setlocal enabledelayedexpansion

echo =========================================================
echo REBUILD.bat — «пересобрать проект без DayZ Tools»
echo =========================================================

:: 1. Проверка Python
python --version >nul 2>&1
if errorlevel 1 (
    echo ОШИБКА: Python не найден. Установите Python 3.8+
    pause
    exit /b 1
)

:: 2. Проверка библиотек (Pillow, numpy)
python -c "import PIL; import numpy" >nul 2>&1
if errorlevel 1 (
    echo Библиотеки Pillow/numpy не найдены. Установить? (y/n)
    set /p choice=
    if /i "!choice!"=="y" (
        python -m pip install Pillow numpy
    ) else (
        echo ОШИБКА: Библиотеки необходимы для обработки текстур
        pause
        exit /b 1
    )
)

echo --- Шаг 1: Текстуры носителей (кассета, MD), обложки ---
python tools\make_textures.py
if errorlevel 1 (
    echo ОШИБКА на этапе текстур
    pause
    exit /b 1
)

echo --- Шаг 2: База музыки и конфиги ---
python tools\generate_db_config.py
if errorlevel 1 (
    echo ОШИБКА на этапе генерации базы
    pause
    exit /b 1
)

echo --- Шаг 3: Обложки для альбомов ---
python tools\make_textures.py --albums
if errorlevel 1 (
    echo ОШИБКА на этапе обложек альбомов
    pause
    exit /b 1
)

echo --- Шаг 4: Staging и архив ---
python tools\stage_pbo.py --clean --zip
if errorlevel 1 (
    echo ОШИБКА на этапе staging
    pause
    exit /b 1
)

echo --- Проверки ---
python tools\check_project.py
python tools\check_bat.py
python tools\check_fresh.py

echo =========================================================
echo REBUILD завершён. Далее: BUILD_PBO.bat
echo =========================================================
pause
exit /b 0
