@echo off
setlocal enabledelayedexpansion

echo =========================================================
echo DEPLOY.bat — разложить по серверу
echo =========================================================

:: 1. Проверка свежести PBO
python tools\check_fresh.py --pbo
if errorlevel 1 (
    echo ОШИБКА: PBO устарели или отсутствуют. Сначала запустите BUILD_PBO.bat
    pause
    exit /b 1
)

:: 2. Создание папки развертывания
mkdir release\deploy 2>nul
rmdir /s /q release\deploy\@MusicRadioCore 2>nul
rmdir /s /q release\deploy\@MusicRadioDB 2>nul
rmdir /s /q release\deploy\ServerMOD 2>nul

mkdir release\deploy\@MusicRadioCore\Addons 2>nul
mkdir release\deploy\@MusicRadioDB\Addons 2>nul
mkdir release\deploy\ServerMOD\Addons 2>nul
mkdir release\deploy\keys 2>nul

:: 3. Копирование PBO и ключей
copy release\pbo\MusicRadioCore.pbo release\deploy\@MusicRadioCore\Addons\ 1>nul
copy release\pbo\MusicRadioCore.pbo.KRaTos.bisign release\deploy\@MusicRadioCore\Addons\ 1>nul
copy release\keys\KRaTos.bikey release\deploy\keys\ 1>nul

copy release\pbo\MusicRadioDB.pbo release\deploy\@MusicRadioDB\Addons\ 1>nul
copy release\pbo\MusicRadioDB.pbo.KRaTos.bisign release\deploy\@MusicRadioDB\Addons\ 1>nul

copy release\pbo\MusicRadioServer.pbo release\deploy\ServerMOD\Addons\ 1>nul

:: 4. Создание meta.cpp (если нужно)
echo class CfgPatches { class MusicRadioMeta { units[]={}; weapons[]={}; requiredVersion=0.1; requiredAddons[]={}; }; }; > release\deploy\meta.cpp

echo --- Развертывание на сервер (если найден) ---
if exist "server_path.txt" (
    set /p SERVER_DIR=<server_path.txt
) else (
    echo Укажите путь к серверу (или оставьте пустым, если только развертывание в release\deploy):
    set /p SERVER_DIR=
)

if not "!SERVER_DIR!"=="" (
    if exist "!SERVER_DIR!\" (
        echo Копирование на сервер: !SERVER_DIR!
        
        :: Копирование клиентских модов
        xcopy /E /Y release\deploy\@MusicRadioCore "!SERVER_DIR!\" 1>nul
        xcopy /E /Y release\deploy\@MusicRadioDB "!SERVER_DIR!\" 1>nul
        xcopy /E /Y release\deploy\keys "!SERVER_DIR!\keys\" 1>nul
        
        :: Копирование серверного мода
        xcopy /E /Y release\deploy\ServerMOD "!SERVER_DIR!\ServerMOD\" 1>nul
        
        :: Очистка старых мест
        rmdir /s /q "!SERVER_DIR!\@MusicRadioServer" 2>nul
        rmdir /s /q "!SERVER_DIR!\ServerMOD\keys" 2>nul
        
        echo Развертывание завершено!
    ) else (
        echo ОШИБКА: Директория сервера не найдена: !SERVER_DIR!
        pause
    )
) else (
    echo Развертывание в release\deploy завершено.
)

echo =========================================================
echo DEPLOY завершён
echo =========================================================
pause
exit /b 0
