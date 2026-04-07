#!/usr/bin/env bash
# ============================================================
# LittleOS - Build Script untuk Linux / WSL
# ============================================================
#
# PRASYARAT:
#   1. Cross-compiler x86_64-elf-gcc
#      Ubuntu/Debian WSL:  sudo apt install gcc-x86-64-linux-gnu binutils-x86-64-linux-gnu
#      atau build sendiri: https://wiki.osdev.org/GCC_Cross-Compiler
#      (gunakan prefix x86_64-elf)
#   2. xorriso
#      sudo apt install xorriso
#   3. git  (untuk mengunduh Limine)
#      sudo apt install git
#
# CARA PAKAI:
#   ./build_kernel.sh           — Build kernel saja
#   ./build_kernel.sh iso       — Build kernel + buat ISO
#   ./build_kernel.sh clean     — Hapus file build
#   ./build_kernel.sh help      — Tampilkan bantuan
# ============================================================

set -e   # keluar otomatis jika ada perintah gagal

echo ""
echo "=========================================="
echo "  LittleOS Build System (Linux/WSL)"
echo "  Version 1.0.0 \"Aurora\""
echo "=========================================="
echo ""

# ---- Deteksi cross-compiler ----
detect_compiler() {
    # Coba beberapa prefix yang umum dipakai
    for PREFIX in x86_64-elf x86_64-linux-gnu; do
        if command -v "${PREFIX}-g++" &>/dev/null; then
            CC="${PREFIX}-g++"
            LD="${PREFIX}-ld"
            OBJCOPY="${PREFIX}-objcopy"
            echo "  Ditemukan: ${PREFIX}-g++"
            return 0
        fi
    done
    echo ""
    echo "  ERROR: Cross-compiler x86_64-elf-g++ tidak ditemukan!"
    echo ""
    echo "  Install di Ubuntu/Debian (WSL):"
    echo "    sudo apt install gcc-x86-64-linux-gnu binutils-x86-64-linux-gnu"
    echo ""
    echo "  Atau build cross-compiler manual:"
    echo "    https://wiki.osdev.org/GCC_Cross-Compiler"
    echo ""
    exit 1
}

build_kernel() {
    echo "[1/3] Mencari cross-compiler..."
    detect_compiler

    echo ""
    echo "[2/3] Mengkompilasi kernel (C++17)..."

    # Buat direktori output
    mkdir -p obj/kernel/source bin

    CFLAGS="-Wall -Wextra -std=c++17 -ffreestanding -fno-stack-protector -fno-stack-check \
-fno-lto -fno-PIC -fno-exceptions -fno-rtti -fno-threadsafe-statics \
-ffunction-sections -fdata-sections -m64 -march=x86-64 -mabi=sysv \
-mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone \
-mcmodel=kernel -mgeneral-regs-only -g -O2"
    CPPFLAGS="-I kernel/include"

    echo "  Compiling main.cpp..."
    $CC $CFLAGS $CPPFLAGS -c kernel/source/main.cpp -o obj/kernel/source/main.o

    echo "  Compiling console.cpp..."
    $CC $CFLAGS $CPPFLAGS -c kernel/source/console.cpp -o obj/kernel/source/console.o

    echo "  Compiling memory.cpp..."
    $CC $CFLAGS $CPPFLAGS -c kernel/source/memory.cpp -o obj/kernel/source/memory.o

    echo "  Compiling interrupts.cpp..."
    $CC $CFLAGS $CPPFLAGS -c kernel/source/interrupts.cpp -o obj/kernel/source/interrupts.o

    echo "  Compiling php_runtime.cpp..."
    $CC $CFLAGS $CPPFLAGS -c kernel/source/php_runtime.cpp -o obj/kernel/source/php_runtime.o

    echo "  Compiling mouse.cpp..."
    $CC $CFLAGS $CPPFLAGS -c kernel/source/mouse.cpp -o obj/kernel/source/mouse.o

    echo "  Compiling rtc.cpp..."
    $CC $CFLAGS $CPPFLAGS -c kernel/source/rtc.cpp -o obj/kernel/source/rtc.o

    echo "  Compiling desktop.cpp..."
    $CC $CFLAGS $CPPFLAGS -c kernel/source/desktop.cpp -o obj/kernel/source/desktop.o

    # Embed binary assets menggunakan objcopy
    echo "  Embedding mascot.bin..."
    $OBJCOPY --input-target binary --output-target elf64-x86-64 \
        --binary-architecture i386:x86-64 \
        kernel/assets/mascot.bin obj/kernel/source/mascot.o

    echo "  Embedding wallpaper.bin..."
    $OBJCOPY --input-target binary --output-target elf64-x86-64 \
        --binary-architecture i386:x86-64 \
        kernel/assets/wallpaper.bin obj/kernel/source/wallpaper.o

    echo "  Embedding menu_icon.bin..."
    $OBJCOPY --input-target binary --output-target elf64-x86-64 \
        --binary-architecture i386:x86-64 \
        kernel/assets/menu_icon.bin obj/kernel/source/menu_icon.o

    echo ""
    echo "[3/3] Linking kernel..."

    $LD -m elf_x86_64 -nostdlib -static -z max-page-size=0x1000 \
        --gc-sections -T kernel/linker.lds \
        obj/kernel/source/main.o \
        obj/kernel/source/console.o \
        obj/kernel/source/memory.o \
        obj/kernel/source/interrupts.o \
        obj/kernel/source/php_runtime.o \
        obj/kernel/source/mouse.o \
        obj/kernel/source/rtc.o \
        obj/kernel/source/desktop.o \
        obj/kernel/source/mascot.o \
        obj/kernel/source/wallpaper.o \
        obj/kernel/source/menu_icon.o \
        -o bin/littleos

    echo ""
    echo "=========================================="
    echo "  Kernel LittleOS berhasil dibangun!"
    echo "  Output: bin/littleos"
    echo "=========================================="
}

build_iso() {
    build_kernel

    echo ""
    echo "[ISO] Membuat bootable ISO..."

    # Unduh Limine jika belum ada
    if [ ! -d "limine" ]; then
        echo "  Mengunduh Limine bootloader v12.x..."
        git clone https://codeberg.org/Limine/Limine.git limine \
            --branch=v12.x-binary --depth=1
    fi

    # Build limine tool (untuk bios-install)
    if [ ! -f "limine/limine" ]; then
        echo "  Membangun limine tool..."
        make -C limine
    fi

    # Buat struktur ISO
    mkdir -p iso_root/boot/limine iso_root/EFI/BOOT

    echo "  Menyalin file..."
    cp -v bin/littleos          iso_root/boot/littleos
    cp -v boot/limine.conf      iso_root/boot/limine/
    cp -v limine/limine-bios.sys        iso_root/boot/limine/
    cp -v limine/limine-bios-cd.bin     iso_root/boot/limine/
    cp -v limine/limine-uefi-cd.bin     iso_root/boot/limine/
    cp -v limine/BOOTX64.EFI            iso_root/EFI/BOOT/
    cp -v limine/BOOTIA32.EFI           iso_root/EFI/BOOT/

    if [ -f "assets/wallpaper/limine-wallpaper.jpeg" ]; then
        cp -v assets/wallpaper/limine-wallpaper.jpeg iso_root/boot/limine/
    fi

    ISO_NAME="LittleOS_Gajah_x86_PHP_8.2_amd_64.iso"

    xorriso -as mkisofs -R -r -J \
        -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        -hfsplus -apm-block-size 2048 \
        --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image \
        --protective-msdos-label \
        iso_root -o "$ISO_NAME"

    # Install Limine BIOS boot stages
    ./limine/limine bios-install "$ISO_NAME"

    echo ""
    echo "=========================================="
    echo "  ISO $ISO_NAME berhasil dibuat!"
    ls -lh "$ISO_NAME"
    echo ""
    echo "  Untuk menjalankan di VirtualBox:"
    echo "    vboxmanage startvm \"LittleOS_Gajah_PHP\""
    echo "  Atau:"
    echo "    qemu-system-x86_64 -cdrom $ISO_NAME -m 256M"
    echo "=========================================="
}

clean() {
    echo "Membersihkan file build..."
    rm -rf obj bin iso_root LittleOS_Gajah_x86_PHP_8.2_amd_64.iso
    echo "Selesai!"
}

show_help() {
    echo "Penggunaan: ./build_kernel.sh [perintah]"
    echo ""
    echo "Perintah:"
    echo "  (kosong)    Build kernel saja"
    echo "  iso         Build kernel + buat bootable ISO"
    echo "  clean       Hapus semua file build"
    echo "  help        Tampilkan pesan ini"
    echo ""
    echo "Prasyarat:"
    echo "  - x86_64-elf-g++ atau x86_64-linux-gnu-g++ (cross-compiler)"
    echo "  - xorriso (untuk membuat ISO)"
    echo "  - git (untuk mengunduh Limine)"
    echo ""
}

case "${1:-}" in
    iso)   build_iso   ;;
    clean) clean       ;;
    help)  show_help   ;;
    *)     build_kernel ;;
esac
