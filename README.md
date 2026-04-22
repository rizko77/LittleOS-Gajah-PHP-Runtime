# LittleOS Gajah PHP

<!--toc:start-->
- [LittleOS Gajah PHP](#littleos-gajah-php)
  - [System Information Overview](#system-information-overview)
  - [System Feature Now](#system-feature-now)
    - [Desktop & GUI](#desktop-gui)
    - [Built-in Applications (Not fully functional yet.)](#built-in-applications-not-fully-functional-yet)
    - [PHP 8.2 Runtime (53 Built-in Functions)](#php-82-runtime-53-built-in-functions)
  - [Project Structure](#project-structure)
  - [Prerequisite](#prerequisite)
    - [Windows](#windows)
    - [Linux / WSL](#linux-wsl)
  - [How to Build](#how-to-build)
    - [Windows (CMD/PowerShell)](#windows-cmdpowershell)
    - [Linux/WSL](#linuxwsl)
    - [QEMU](#qemu)
  - [Architecture Overview](#architecture-overview)
  - [License](#license)
  - [❤️ Wallpaper Credits](#️-wallpaper-credits)
<!--toc:end-->

> **Experimental Opensource Operating System — PHP 8.2 Bare Metal Runtime**

![License](https://img.shields.io/badge/license-GPL--3.0-blue)
![Language](https://img.shields.io/badge/kernel-PHP%208.2-777BB4)
![Arch](https://img.shields.io/badge/arch-x86__64-lightgrey)
![Bootloader](https://img.shields.io/badge/bootloader-Limine-orange)
![Status](https://img.shields.io/badge/status-experimental-yellow)

LittleOS Gajah is a lightweight, standalone operating system built on a PHP 8.2 runtime that runs directly on **bare metal x86_64** hardware — no Linux, no UNIX, no Windows/DOS underneath.
The entire desktop, window manager, applications, and kernel logic are written in **pure PHP 8**.
The hardware abstraction layer (HAL) is written in C++ and compiled with a cross-compiler targeting the freestanding x86_64 environment.

Created by Indonesian developers for the global open source and educational community.

---

## System Information Overview

| Info | Detail |
|---|---|
| **OS Name** | LittleOS Gajah PHP |
| **Version** | 1.0.0 "Gajah" |
| **Architecture** | x86_64 (Long Mode, 64-bit) |
| **Kernel Language** | PHP 8.2 |
| **HAL Language** | C++17 (freestanding) |
| **Bootloader** | Limine 8.x (BIOS + UEFI) |
| **Display Server** | Custom Framebuffer (VESA/LFB) |
| **Desktop Environment** | LittleOS Desktop (Tailwind CSS palette) |
| **Window Manager** | LittleOS WM |
| **UI/UX Theme** | Tailwind CSS color system |
| **Font** | CP437 8×8 Bitmap |
| **Heap Size** | 8 MB (first-fit allocator) |
| **Min RAM** | 256 MB recommended |
| **Input** | PS/2 Keyboard + PS/2 Mouse |
| **Timer** | PIT 8253 @ 1000 Hz |
| **RTC** | CMOS Real-Time Clock |
| **Partition** | MBR + GPT hybrid ISO |

---

## System Feature Now

### Desktop & GUI

- Full graphical desktop with animated boot splash screen
- Double-buffered rendering (no screen tearing)
- Desktop Wallpaper with artwork rendered via pixel form Pixiv <https://i.pximg.net/img-master/img/2022/09/28/00/00/25/101532845_p0_master1200.jpg>
- Draggable windows with title bar (minimize / maximize / close buttons)
- Start menu with application launcher
- Right-click context menu on desktop
- Taskbar with pinned apps, open window buttons, system tray
- System tray showing clock (HH:MM), date, Wi-Fi/battery/volume icons
- Live clock updated every frame via CMOS RTC

### Built-in Applications (Not fully functional yet.)

| App | Description |
|---|---|
| **Terminal PHP** | Interactive PHP REPL (read-eval-print) |
| **Sistem** | System information viewer (CPU, RAM, drivers) |
| **File Manager** | Virtual filesystem browser (kernel PHP files) |
| **PHP Browser** | HTML/PHP source viewer |
| **Pengaturan** | Display, kernel, and theme settings |
| **Task Manager** | Process list and memory/CPU monitor |
| **Jam & Tanggal** | Live digital clock and date |

### PHP 8.2 Runtime (53 Built-in Functions)

**String**

```
strlen()  substr()  strtoupper()  strtolower()
str_repeat()  str_contains()  intval()  strval()
```

**Array & Core**

```
count()  isset()  print()  var_dump()
```

**Kernel / System**

```
kernel_version()  kernel_name()  kernel_arch()
memory_total()  memory_free()  memory_used()
uptime()  uptime_ms()
```

**Console**

```
console_clear()  console_set_color()  console_cols()  console_rows()
readline()  sleep()
```

**Graphics (Framebuffer)**

```
screen_width()  screen_height()
draw_pixel()  fill_rect()  draw_rect()  fill_rounded_rect()
draw_text()  draw_line()  fill_circle()
```

**Input**

```
mouse_x()  mouse_y()  mouse_clicked()  mouse_event()
keyboard_read()  keyboard_has_input()
```

**RTC (Real-Time Clock)**

```
rtc_hour()  rtc_minute()  rtc_second()
rtc_day()  rtc_month()  rtc_year()
```

**Desktop / Window Manager**

```
desktop_init()  desktop_create_window()  desktop_close_window()
desktop_set_text()  desktop_render()  desktop_poll_events()
```

---

## Project Structure

```
LittleOS - PHP Runtime/
├── kernel/
│   ├── include/
│   │   ├── kernel.hpp          # OS constants, version, types
│   │   ├── hal.hpp             # Hardware abstraction layer API
│   │   ├── php_runtime.hpp     # PHP interpreter class definition
│   │   └── limine.h            # Limine bootloader protocol
│   ├── source/
│   │   ├── main.cpp            # Kernel entry, HAL init, boot splash, PHP runner
│   │   ├── php_runtime.cpp     # Complete PHP 8.2 lexer/parser/interpreter
│   │   ├── desktop.cpp         # Window manager, desktop renderer
│   │   ├── console.cpp         # Framebuffer console, font rendering
│   │   ├── memory.cpp          # Physical memory manager, heap allocator
│   │   ├── interrupts.cpp      # IDT, ISR, IRQ handlers
│   │   ├── mouse.cpp           # PS/2 mouse driver
│   │   ├── rtc.cpp             # CMOS RTC driver
│   │   ├── mascot.cpp          # Mascot artwork pixel data
│   │   └── wallpaper.cpp       # Wallpaper pixel data
│   ├── assets/                 # Embedded kernel assets (pixel art)
│   ├── main/                   # PHP source modules (documentation/reference)
│   └── linker.lds              # Linker script (higher-half kernel)
├── boot/
│   └── limine.conf             # Bootloader configuration
├── GNUmakefile                 # Linux/macOS build system
├── build.bat                   # Windows (CMD) build script
├── build_kernel.sh             # Linux / WSL (Bash) build script
├── run_virtualbox.bat          # VirtualBox launcher script
└── toolreqruiement.txt         # Tool requirements list
```

---

## Prerequisite

| Tool | Purpose |
| --- | --- |
| `x86_64-elf-g++` / clang (Linux) | C++17 freestanding cross-compiler |
| `x86_64-elf-ld` / lld (Linux) | Cross-linker |
| `xorriso` | ISO 9660 image creator |
| `limine` | Bootloader (download separately) |
| VirtualBox / QEMU | Testing the ISO |
| git | Clone repo |
| Make & CMake | Build system |
| nasm | Assembly compiler |
| Ninja | Build generator for CMake |

### Windows

1. Download the `x86_64-elf` cross-compiler from [OSDev GCC Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) or [lordmilko/i686-elf-tools](https://github.com/lordmilko/i686-elf-tools)
2. Install xorriso via [MSYS2](https://www.msys2.org/): `pacman -S xorriso`

### Linux / WSL

```bash
# Arch
sudo pacman -Sy
sudo pacman -S clang lld nasm cmake virtualbox xorriso git ninja make

# Ubuntu/Debian
sudo apt update
sudo apt install clang lld nasm cmake virtualbox xorriso git ninja make
# Build or download x86_64-elf cross-compiler, see OSDev wiki
```

Keduanya kemudian download Limine dengan:

- `git clone -b v11.x-binary --single-branch https://github.com/Limine-Bootloader/Limine.git limine`

See [Limine Bootloader Binary](https://github.com/Limine-Bootloader/Limine/tree/v11.x-binary#)

---

## How to Build

### Windows (CMD/PowerShell)

Gunakan `build.bat` yang sudah disediakan:

```bat
REM Build kernel saja
build.bat

REM Build kernel + buat ISO bootable
build.bat iso

REM Hapus semua file build
build.bat clean
```

Prerequisite yang harus tersedia:

- **Cross-compiler** `x86_64-elf-g++` di `C:\cross\bin\` (otomatis dideteksi oleh script)
  Download: <https://github.com/lordmilko/i686-elf-tools/releases>
- **xorriso** — install via `scoop install xorriso` atau `choco install xorriso`
- **git** — diperlukan untuk mengunduh Limine saat pertama kali build ISO

> **Catatan:** Script menggunakan `x86_64-elf-objcopy` untuk meng-embed asset biner
> (mascot, wallpaper, menu icon) ke dalam kernel secara otomatis.

### Linux/WSL

Gunakan `build_kernel.sh` yang sudah disediakan:

```bash
# Beri izin eksekusi (sekali saja)
chmod +x compile.sh

# Build kernel saja
./build_kernel.sh

# Hapus semua file build
./build_kernel.sh clean
```

Jika memakai CMake, jalankan perintah berikut:

```
CXX=clang++ CC=clang cmake -B build -S . -G "Ninja"

cmake --build build
```

Atau cukup menggunakan `./compile.sh`.

---

## How to Run

### VirtualBox
```

1. Create a new VM:
   - Type: Other  /  Version: Other (64-bit)
   - RAM: 256 MB minimum
   - No hard disk needed
2. Settings > Storage > Add ISO: LittleOS_Gajah_x86_PHP_8.2_amd_64.iso
3. Settings > Display > Video Memory: 128 MB
4. Start VM

```

- Alternatif Windows: Gunakan `run_virtualbox.bat`.
- Alternatif Linux: Gunakan `./compile.sh`.

### QEMU

```bash
qemu-system-x86_64 -cdrom LittleOS_Gajah_x86_PHP_8.2_amd_64.iso \
  -m 256M -vga qxl -serial stdio
```

Jika Anda menggunakan compositor wayland dan mengalami masalah dengan kursor, tambahkan argumen `-display sdl`.

Alternatif Linux: Gunakan `compile.sh` untuk otomatis konfigurasi cmake, build, dan run.
> `chmod +x ./compile.sh` Jangan lupa.

---

## Architecture Overview

```
┌─────────────────────────────────────────┐
│          PHP 8.2 Kernel Script          │  ← Pure PHP (desktop, apps, logic)
│     desktop.php  apps  window manager   │
├─────────────────────────────────────────┤
│         PHP 8.2 Interpreter             │  ← php_runtime.cpp (C++17)
│   Lexer → Parser → AST → Evaluator      │
│   53 built-in functions  Value pool     │
├─────────────────────────────────────────┤
│    Hardware Abstraction Layer (HAL)     │  ← hal.hpp / C++ freestanding
│  Console  Memory  Graphics  Input  RTC  │
├─────────────────────────────────────────┤
│          x86_64 Bare Metal              │
│  Framebuffer  PS/2  PIT  CMOS  PCI      │
└─────────────────────────────────────────┘
        Booted by Limine (BIOS/UEFI)
```

---

## License

GNU General Public License — see [LICENSE](LICENSE)

---

## ❤️ Wallpaper Credits

Beautiful artwork used as the desktop wallpaper:

- Pixiv: <https://www.pixiv.net/en/artworks/101532845>
- Image: <https://i.pximg.net/img-master/img/2022/09/28/00/00/25/101532845_p0_master1200.jpg>

All rights belong to the original artist.
