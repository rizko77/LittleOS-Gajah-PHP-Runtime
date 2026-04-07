@echo off
REM ============================================================
REM LittleOS - Menjalankan di VirtualBox
REM ============================================================

echo.
echo Menjalankan LittleOS di VirtualBox...
echo.

REM Cek apakah ISO sudah ada
if not exist "LittleOS_Gajah_x86_PHP_8.2_amd_64.iso" (
    echo ERROR: LittleOS_Gajah_x86_PHP_8.2_amd_64.iso tidak ditemukan!
    echo Jalankan "build.bat iso" terlebih dahulu.
    goto end
)

REM Cari VBoxManage
set VBOX=
where VBoxManage >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set VBOX=VBoxManage
    goto found_vbox
)

if exist "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" (
    set "VBOX=C:\Program Files\Oracle\VirtualBox\VBoxManage.exe"
    goto found_vbox
)

echo VirtualBox tidak ditemukan.
echo.
echo Anda bisa menjalankan ISO secara manual:
echo 1. Buka VirtualBox
echo 2. Buat VM baru:
echo    - Name: LittleOS
echo    - Type: Other
echo    - Version: Other/Unknown (64-bit)
echo    - RAM: 256 MB
echo    - No hard disk
echo 3. Settings ^> Storage ^> Tambahkan LittleOS_Gajah_x86_PHP_8.2_amd_64.iso
echo 4. Settings ^> Display ^> Video Memory: 128 MB
echo 5. Klik Start
echo.
goto end

:found_vbox
echo Ditemukan VirtualBox: %VBOX%
echo.

REM Cek apakah VM sudah ada
"%VBOX%" showvminfo "LittleOS_Gajah_PHP" >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo VM "LittleOS_Gajah_PHP" sudah ada, menghapus yang lama...
    "%VBOX%" controlvm "LittleOS_Gajah_PHP" poweroff >nul 2>&1
    timeout /t 2 /nobreak >nul
    "%VBOX%" unregistervm "LittleOS_Gajah_PHP" --delete >nul 2>&1
)

echo Membuat VM baru "LittleOS_Gajah_PHP"...

REM Buat VM
"%VBOX%" createvm --name "LittleOS_Gajah_PHP" --ostype "Other_64" --register

REM Konfigurasi VM
"%VBOX%" modifyvm "LittleOS_Gajah_PHP" --memory 256 --vram 128 --cpus 1
"%VBOX%" modifyvm "LittleOS_Gajah_PHP" --graphicscontroller vboxvga
"%VBOX%" modifyvm "LittleOS_Gajah_PHP" --audio none
"%VBOX%" modifyvm "LittleOS_Gajah_PHP" --mouse ps2
"%VBOX%" modifyvm "LittleOS_Gajah_PHP" --keyboard ps2
"%VBOX%" modifyvm "LittleOS_Gajah_PHP" --boot1 dvd --boot2 none --boot3 none --boot4 none

REM Tambahkan storage controller dan ISO
"%VBOX%" storagectl "LittleOS_Gajah_PHP" --name "IDE" --add ide
"%VBOX%" storageattach "LittleOS_Gajah_PHP" --storagectl "IDE" --port 1 --device 0 --type dvddrive --medium "%CD%\LittleOS_Gajah_x86_PHP_8.2_amd_64.iso"

echo.
echo Menjalankan VM...
"%VBOX%" startvm "LittleOS_Gajah_PHP"

echo.
echo ==========================================
echo   LittleOS Gajah PHP Desktop
echo   sedang berjalan di VirtualBox!
echo ==========================================

:end
