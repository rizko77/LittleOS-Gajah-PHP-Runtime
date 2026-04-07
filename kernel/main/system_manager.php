<?php
/**
 * LittleOS Gajah PHP 8.2 — system_manager.php
 * Window Manager, Desktop Manager, Task Manager, Start Menu
 * 
 * Ini adalah inti desktop environment — ditulis dalam PHP.
 * GUI dirender melalui built-in C++ HAL functions.
 * Menggunakan Tailwind CSS color palette.
 * 
 * @package  LittleOS\SystemManager
 */

// ============================================================
// WINDOW MANAGER PHP
// ============================================================

/**
 * Buka aplikasi dari start menu
 */
function open_app($app_name) {
    if ($app_name == "Sistem") {
        app_sistem();
    } elseif ($app_name == "Setting") {
        app_setting();
    } elseif ($app_name == "File Manager") {
        app_file_manager();
    } elseif ($app_name == "Browser") {
        app_browser();
    } elseif ($app_name == "Terminal") {
        app_terminal();
    } elseif ($app_name == "Task Manager") {
        app_task_manager();
    } elseif ($app_name == "Clock Date") {
        app_clock();
    }
}

// ============================================================
// APLIKASI: Sistem
// ============================================================
function app_sistem() {
    $id = desktop_create_window("Sistem", 100, 50, 500, 400, "sistem");
    $info = "<?php phpinfo(); ?>\n\n";
    $info = $info . "LittleOS Gajah PHP 8.2\n";
    $info = $info . "========================\n\n";
    $info = $info . "OS       : " . kernel_name() . "\n";
    $info = $info . "Version  : " . kernel_version() . "\n";
    $info = $info . "Arch     : " . kernel_arch() . "\n";
    $info = $info . "Kernel   : PHP 8.2 Runtime\n";
    $info = $info . "Type     : Bare Metal x86_64\n\n";
    $info = $info . "Ini BUKAN UNIX.\n";
    $info = $info . "Ini BUKAN Linux.\n";
    $info = $info . "Ini BUKAN DOS.\n\n";
    $info = $info . "Ini adalah LittleOS Gajah,\n";
    $info = $info . "OS dengan kernel murni PHP 8.\n\n";
    $info = $info . "Memori  : " . format_bytes(memory_total()) . "\n";
    $info = $info . "Free    : " . format_bytes(memory_free()) . "\n";
    $info = $info . "Used    : " . format_bytes(memory_used()) . "\n";
    $info = $info . "Screen  : " . screen_width() . "x" . screen_height() . "\n";
    $info = $info . "Uptime  : " . uptime() . " detik\n";
    $info = $info . "\nDriver:\n";
    $info = $info . "  Framebuffer : aktif\n";
    $info = $info . "  Keyboard    : aktif\n";
    $info = $info . "  Mouse       : aktif\n";
    $info = $info . "  Timer       : aktif (1000Hz)\n";
    $info = $info . "  RTC         : aktif\n";
    desktop_set_text($id, $info);
}

// ============================================================
// APLIKASI: Setting
// ============================================================
function app_setting() {
    $id = desktop_create_window("Setting", 150, 80, 450, 350, "setting");
    $info = "<?php // Settings ?>\n\n";
    $info = $info . "LittleOS Settings\n";
    $info = $info . "==================\n\n";
    $info = $info . "Tema     : Tailwind CSS (Dark)\n";
    $info = $info . "Font     : CP437 8x8 Bitmap\n";
    $info = $info . "Resolusi : " . screen_width() . "x" . screen_height() . "\n";
    $info = $info . "Warna BG : Slate-800 (#1E293B)\n";
    $info = $info . "Warna FG : Slate-100 (#F1F5F9)\n";
    $info = $info . "Accent   : Blue-600 (#2563EB)\n\n";
    $info = $info . "Bahasa       : PHP 8.2\n";
    $info = $info . "Runtime      : Custom Interpreter\n";
    $info = $info . "HAL          : C++ (x86_64)\n\n";
    $info = $info . "Tailwind CSS Colors:\n";
    $info = $info . "  slate-900 : Desktop background\n";
    $info = $info . "  slate-950 : Taskbar\n";
    $info = $info . "  blue-600  : Active window\n";
    $info = $info . "  red-500   : Close button\n";
    $info = $info . "  green-500 : Maximize button\n";
    $info = $info . "  yellow-400: Minimize button\n";
    desktop_set_text($id, $info);
}

// ============================================================
// APLIKASI: File Manager  
// ============================================================
function app_file_manager() {
    $id = desktop_create_window("File Manager", 120, 60, 520, 380, "filemanager");
    $info = "<?php // File System ?>\n\n";
    $info = $info . "LittleOS File Manager\n";
    $info = $info . "=====================\n\n";
    $info = $info . "  /\n";
    $info = $info . "  |-- boot/\n";
    $info = $info . "  |   |-- limine.conf\n";
    $info = $info . "  |   |-- littleos (kernel)\n";
    $info = $info . "  |-- kernel/\n";
    $info = $info . "  |   |-- main/\n";
    $info = $info . "  |   |   |-- kernel.php\n";
    $info = $info . "  |   |   |-- console.php\n";
    $info = $info . "  |   |   |-- memory.php\n";
    $info = $info . "  |   |   |-- graphics.php\n";
    $info = $info . "  |   |   |-- keyboard.php\n";
    $info = $info . "  |   |   |-- mouse.php\n";
    $info = $info . "  |   |   |-- timer.php\n";
    $info = $info . "  |   |   |-- rtc.php\n";
    $info = $info . "  |   |   |-- string.php\n";
    $info = $info . "  |   |   |-- ports.php\n";
    $info = $info . "  |   |   |-- pci.php\n";
    $info = $info . "  |   |   |-- driver.php\n";
    $info = $info . "  |   |   |-- interrupts.php\n";
    $info = $info . "  |   |   |-- limine.php\n";
    $info = $info . "  |   |   |-- system_manager.php\n";
    $info = $info . "  |-- apps/\n";
    $info = $info . "  |   |-- (dibangun dalam PHP)\n\n";
    $info = $info . "Semua file .php adalah modul PHP 8.\n";
    $info = $info . "Ini ciri khas LittleOS Gajah PHP!\n";
    desktop_set_text($id, $info);
}

// ============================================================
// APLIKASI: Browser
// ============================================================
function app_browser() {
    $id = desktop_create_window("Browser", 80, 40, 560, 420, "browser");
    $info = "<?php // LittleOS Browser ?>\n\n";
    $info = $info . "LittleOS PHP Browser v1.0\n";
    $info = $info . "=========================\n\n";
    $info = $info . "URL: php://localhost\n";
    $info = $info . "Status: 200 OK\n\n";
    $info = $info . "+-----------------------------------+\n";
    $info = $info . "|  <!DOCTYPE html>                  |\n";
    $info = $info . "|  <html lang='id'>                 |\n";
    $info = $info . "|  <head>                           |\n";
    $info = $info . "|    <title>LittleOS</title>        |\n";
    $info = $info . "|    <!-- Tailwind CSS -->           |\n";
    $info = $info . "|  </head>                          |\n";
    $info = $info . "|  <body class='bg-slate-900'>      |\n";
    $info = $info . "|    <div class='container mx-auto'>|\n";
    $info = $info . "|      <h1 class='text-sky-500      |\n";
    $info = $info . "|           text-4xl font-bold'>    |\n";
    $info = $info . "|        Selamat Datang di           |\n";
    $info = $info . "|        LittleOS Gajah PHP!        |\n";
    $info = $info . "|      </h1>                        |\n";
    $info = $info . "|      <p class='text-slate-300'>   |\n";
    $info = $info . "|        OS pertama dengan kernel    |\n";
    $info = $info . "|        murni PHP 8.2 Runtime.     |\n";
    $info = $info . "|      </p>                         |\n";
    $info = $info . "|    </div>                         |\n";
    $info = $info . "|  </body>                          |\n";
    $info = $info . "|  </html>                          |\n";
    $info = $info . "+-----------------------------------+\n";
    $info = $info . "\n[Tailwind CSS powered browser]\n";
    desktop_set_text($id, $info);
}

// ============================================================
// APLIKASI: Terminal
// ============================================================
function app_terminal() {
    $id = desktop_create_window("Terminal", 140, 90, 500, 350, "terminal");
    $info = "<?php // LittleOS Terminal ?>\n\n";
    $info = $info . "LittleOS Gajah PHP Terminal v1.0\n";
    $info = $info . "================================\n\n";
    $info = $info . "php> echo 'Hello, World!';\n";
    $info = $info . "Hello, World!\n\n";
    $info = $info . "php> echo PHP_VERSION;\n";
    $info = $info . "8.2.0-littleos\n\n";
    $info = $info . "php> $a = 42;\n";
    $info = $info . "php> echo $a * 2;\n";
    $info = $info . "84\n\n";
    $info = $info . "php> echo kernel_name();\n";
    $info = $info . "LittleOS Gajah PHP\n\n";
    $info = $info . "php> phpinfo();\n";
    $info = $info . "[System Information]\n";
    $info = $info . "  OS: PHP 8.2 Runtime\n";
    $info = $info . "  Arch: x86_64\n\n";
    $info = $info . "php> _\n";
    desktop_set_text($id, $info);
}

// ============================================================
// APLIKASI: Task Manager
// ============================================================
function app_task_manager() {
    $id = desktop_create_window("Task Manager", 160, 70, 480, 380, "taskmanager");
    $info = "<?php // Task Manager ?>\n\n";
    $info = $info . "LittleOS Task Manager\n";
    $info = $info . "=====================\n\n";
    $info = $info . "PROSES AKTIF:\n";
    $info = $info . "  PID  Nama              Status\n";
    $info = $info . "  ---  ----              ------\n";
    $info = $info . "  1    php_runtime       Running\n";
    $info = $info . "  2    desktop_manager   Running\n";
    $info = $info . "  3    window_manager    Running\n";
    $info = $info . "  4    timer_service     Running\n";
    $info = $info . "  5    keyboard_driver   Running\n";
    $info = $info . "  6    mouse_driver      Running\n";
    $info = $info . "\nMEMORI:\n";
    $info = $info . "  Total : " . format_bytes(memory_total()) . "\n";
    $info = $info . "  Used  : " . format_bytes(memory_used()) . "\n";
    $info = $info . "  Free  : " . format_bytes(memory_free()) . "\n";
    $info = $info . "  " . memory_bar(30) . "\n";
    $info = $info . "\nUPTIME: " . uptime() . " detik\n";
    $info = $info . "\nKERNEL: PHP 8.2 Runtime\n";
    $info = $info . "Runtime: <?php echo 'Aktif'; ?>\n";
    desktop_set_text($id, $info);
}

// ============================================================
// APLIKASI: Clock Date
// ============================================================
function app_clock() {
    $id = desktop_create_window("Clock Date", 200, 100, 400, 300, "clock");
    $info = "<?php echo date('H:i:s'); ?>\n\n";
    $info = $info . "LittleOS Clock & Date\n";
    $info = $info . "=====================\n\n";
    $info = $info . "Waktu  : " . get_time_string() . "\n";
    $info = $info . "Tanggal: " . get_date_string() . "\n\n";
    $info = $info . "Timezone: UTC (CMOS RTC)\n\n";
    $info = $info . "Uptime : " . format_uptime() . "\n\n";
    $info = $info . "Ticks  : " . uptime_ms() . " ms\n";
    desktop_set_text($id, $info);
}

// ============================================================
// DESKTOP MAIN LOOP (PHP event loop)
// ============================================================

/**
 * Main desktop loop — dipanggil oleh kernel saat desktop mode
 */
function desktop_main() {
    desktop_init();
    
    while (true) {
        // Poll mouse events
        desktop_poll_events();
        
        // Render frame
        desktop_render();
        
        // Small delay to not burn CPU
        sleep(16);
    }
}
