@echo off
REM ============================================================
REM LittleOS - Build Script untuk Windows
REM ============================================================
REM 
REM Script ini membangun kernel LittleOS di Windows.
REM 
REM PRASYARAT:
REM   1. Cross-compiler x86_64-elf-gcc (BUKAN MinGW!)
REM      Download dari: https://github.com/lordmilko/i686-elf-tools/releases
REM      atau: https://wiki.osdev.org/GCC_Cross-Compiler
REM   2. NASM assembler (opsional, belum dipakai)
REM   3. Git (untuk mengunduh Limine)
REM   4. xorriso (untuk membuat ISO)
REM      Bisa install via: scoop install xorriso
REM      atau: choco install xorriso
REM
REM CARA PAKAI:
REM   build.bat          — Build kernel saja
REM   build.bat iso      — Build kernel + buat ISO
REM   build.bat clean    — Hapus file build
REM   build.bat help     — Tampilkan bantuan
REM ============================================================

echo.
echo ==========================================
echo   LittleOS Build System
echo   Version 1.0.0 "Aurora"
echo ==========================================
echo.

REM Cek argumen
if "%1"=="clean" goto clean
if "%1"=="iso" goto build_iso
if "%1"=="help" goto help

:build_kernel
echo [1/3] Mencari cross-compiler...

REM Cek apakah x86_64-elf-g++ ada di PATH
where x86_64-elf-g++ >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set CC=x86_64-elf-g++
    set LD=x86_64-elf-ld
    echo   Ditemukan: x86_64-elf-g++
    goto compile
)

REM Coba cari di lokasi umum
if exist "C:\cross\bin\x86_64-elf-g++.exe" (
    set CC=C:\cross\bin\x86_64-elf-g++
    set LD=C:\cross\bin\x86_64-elf-ld
    echo   Ditemukan: C:\cross\bin\x86_64-elf-g++
    goto compile
)

if exist "%USERPROFILE%\cross\bin\x86_64-elf-g++.exe" (
    set CC=%USERPROFILE%\cross\bin\x86_64-elf-g++
    set LD=%USERPROFILE%\cross\bin\x86_64-elf-ld
    echo   Ditemukan: %USERPROFILE%\cross\bin\x86_64-elf-g++
    goto compile
)

echo.
echo   ERROR: Cross-compiler x86_64-elf-g++ tidak ditemukan!
echo.
echo   Untuk membangun OS, Anda memerlukan cross-compiler khusus.
echo   Ini BUKAN MinGW/MSYS2 - ini adalah compiler yang menghasilkan
echo   binary untuk bare-metal (tanpa OS).
echo.
echo   Cara install:
echo   1. Download dari https://github.com/lordmilko/i686-elf-tools/releases
echo      (cari file x86_64-elf-tools-windows.zip)
echo   2. Ekstrak ke C:\cross\
echo   3. Pastikan C:\cross\bin\ ada di PATH
echo.
echo   Atau gunakan WSL/Linux dengan: sudo apt install gcc-x86-64-elf
echo.
goto end

:compile
echo.
echo [2/3] Mengkompilasi kernel (C++ 17)...

REM Buat direktori output
if not exist "obj\kernel\source" mkdir "obj\kernel\source"
if not exist "bin" mkdir "bin"

REM Flag kompilasi C++17
set CFLAGS=-Wall -Wextra -std=c++17 -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fno-PIC -fno-exceptions -fno-rtti -fno-threadsafe-statics -ffunction-sections -fdata-sections -m64 -march=x86-64 -mabi=sysv -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel -mgeneral-regs-only -g -O2
set CPPFLAGS=-I kernel/include

REM Kompilasi setiap file .cpp
echo   Compiling main.cpp...
%CC% %CFLAGS% %CPPFLAGS% -c kernel/source/main.cpp -o obj/kernel/source/main.o
if %ERRORLEVEL% neq 0 goto error

echo   Compiling console.cpp...
%CC% %CFLAGS% %CPPFLAGS% -c kernel/source/console.cpp -o obj/kernel/source/console.o
if %ERRORLEVEL% neq 0 goto error

echo   Compiling memory.cpp...
%CC% %CFLAGS% %CPPFLAGS% -c kernel/source/memory.cpp -o obj/kernel/source/memory.o
if %ERRORLEVEL% neq 0 goto error

echo   Compiling interrupts.cpp...
%CC% %CFLAGS% %CPPFLAGS% -c kernel/source/interrupts.cpp -o obj/kernel/source/interrupts.o
if %ERRORLEVEL% neq 0 goto error

echo   Compiling php_runtime.cpp...
%CC% %CFLAGS% %CPPFLAGS% -c kernel/source/php_runtime.cpp -o obj/kernel/source/php_runtime.o
if %ERRORLEVEL% neq 0 goto error

echo   Compiling mouse.cpp...
%CC% %CFLAGS% %CPPFLAGS% -c kernel/source/mouse.cpp -o obj/kernel/source/mouse.o
if %ERRORLEVEL% neq 0 goto error

echo   Compiling rtc.cpp...
%CC% %CFLAGS% %CPPFLAGS% -c kernel/source/rtc.cpp -o obj/kernel/source/rtc.o
if %ERRORLEVEL% neq 0 goto error

echo   Compiling desktop.cpp...
%CC% %CFLAGS% %CPPFLAGS% -c kernel/source/desktop.cpp -o obj/kernel/source/desktop.o
if %ERRORLEVEL% neq 0 goto error

REM Embed binary assets via objcopy
set OBJCOPY=%LD:ld=objcopy%
echo   Embedding mascot.bin...
%OBJCOPY% --input-target binary --output-target elf64-x86-64 --binary-architecture i386:x86-64 kernel/assets/mascot.bin obj/kernel/source/mascot.o
if %ERRORLEVEL% neq 0 goto error

echo   Embedding wallpaper.bin...
%OBJCOPY% --input-target binary --output-target elf64-x86-64 --binary-architecture i386:x86-64 kernel/assets/wallpaper.bin obj/kernel/source/wallpaper.o
if %ERRORLEVEL% neq 0 goto error

echo   Embedding menu_icon.bin...
%OBJCOPY% --input-target binary --output-target elf64-x86-64 --binary-architecture i386:x86-64 kernel/assets/menu_icon.bin obj/kernel/source/menu_icon.o
if %ERRORLEVEL% neq 0 goto error

echo.
echo [3/3] Linking kernel...

%LD% -m elf_x86_64 -nostdlib -static -z max-page-size=0x1000 --gc-sections -T kernel/linker.lds obj/kernel/source/main.o obj/kernel/source/console.o obj/kernel/source/memory.o obj/kernel/source/interrupts.o obj/kernel/source/php_runtime.o obj/kernel/source/mouse.o obj/kernel/source/rtc.o obj/kernel/source/desktop.o obj/kernel/source/mascot.o obj/kernel/source/wallpaper.o obj/kernel/source/menu_icon.o -o bin/littleos
if %ERRORLEVEL% neq 0 goto error

echo.
echo ==========================================
echo   Kernel LittleOS berhasil dibangun!
echo   Output: bin\littleos
echo ==========================================

if "%1"=="" goto end
goto end

:build_iso
call :build_kernel
if %ERRORLEVEL% neq 0 goto end

echo.
echo [ISO] Membuat bootable ISO...

REM Unduh Limine jika belum ada (update ke v12.x)
if not exist "limine" (
    echo   Mengunduh Limine bootloader v12.x...
    git clone https://codeberg.org/Limine/Limine.git limine --branch=v12.x-binary --depth=1
)

REM Buat struktur ISO
if not exist "iso_root\boot\limine" mkdir "iso_root\boot\limine"
if not exist "iso_root\EFI\BOOT" mkdir "iso_root\EFI\BOOT"

echo   [ISO] Copying files...
echo   - bin\littleos -
if exist "bin\littleos" (
    copy /Y "bin\littleos" "iso_root\boot\littleos"
    echo     OK: bin\littleos copied
) else (
    echo     ERROR: bin\littleos not found!
    goto error
)

echo   - boot\limine.conf -
copy /Y "boot\limine.conf" "iso_root\boot\limine\"

echo   - Limine files -
copy /Y "limine\limine-bios.sys" "iso_root\boot\limine\"
copy /Y "limine\limine-bios-cd.bin" "iso_root\boot\limine\"
copy /Y "limine\limine-uefi-cd.bin" "iso_root\boot\limine\"
copy /Y "limine\BOOTX64.EFI" "iso_root\EFI\BOOT\"
copy /Y "limine\BOOTIA32.EFI" "iso_root\EFI\BOOT\"

echo   - Wallpaper -
if exist "assets\wallpaper\limine-wallpaper.jpeg" (
    copy /Y "assets\wallpaper\limine-wallpaper.jpeg" "iso_root\boot\limine\"
    echo     OK: Wallpaper copied
) else (
    echo     WARNING: Wallpaper not found
)

echo   [ISO] Verifying iso_root structure...
dir /s /b iso_root

REM Buat ISO dengan xorriso
xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label iso_root -o LittleOS_Gajah_x86_PHP_8.2_amd_64.iso

REM Install Limine BIOS boot stages
limine\limine.exe bios-install LittleOS_Gajah_x86_PHP_8.2_amd_64.iso

echo.
echo ==========================================
echo   ISO LittleOS_Gajah_x86_PHP_8.2_amd_64.iso berhasil dibuat!
echo   Ukuran: 
dir LittleOS_Gajah_x86_PHP_8.2_amd_64.iso | findstr "LittleOS_Gajah_x86_PHP_8.2_amd_64.iso"
echo.
echo   Untuk menjalankan di VirtualBox:
echo   1. Buat VM baru (Type: Other, Version: Other/Unknown 64-bit)
echo   2. Atur RAM: 256 MB (minimum)
echo   3. Tidak perlu hard disk
echo   4. Settings ^> Storage ^> Tambahkan ISO
echo   5. Start VM
echo ==========================================
goto end

:clean
echo Membersihkan file build...
if exist "obj" rmdir /s /q obj
if exist "bin" rmdir /s /q bin
if exist "iso_root" rmdir /s /q iso_root
if exist "LittleOS_Gajah_x86_PHP_8.2_amd_64.iso" del LittleOS_Gajah_x86_PHP_8.2_amd_64.iso
echo Selesai!
goto end

:help
echo.
echo Penggunaan: build.bat [perintah]
echo.
echo Perintah:
echo   (kosong)    Build kernel saja
echo   iso         Build kernel + buat bootable ISO
echo   clean       Hapus semua file build
echo   help        Tampilkan pesan ini
echo.
echo Prasyarat:
echo   - x86_64-elf-gcc (cross-compiler)
echo   - xorriso (untuk membuat ISO)
echo   - Git (untuk mengunduh Limine)
echo.
goto end

:error
echo.
echo ==========================================
echo   ERROR: Build gagal!
echo   Periksa pesan error di atas.
echo ==========================================

:end
