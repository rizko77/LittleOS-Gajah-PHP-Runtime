/*
 * LittleOS Gajah PHP - main.cpp
 * Entry point kernel — Limine boot → HAL init → PHP Runtime
 *
 * Ini BUKAN UNIX. BUKAN Linux. BUKAN DOS.
 * Kernel murni PHP 8 Runtime yang berjalan di bare metal x86_64.
 * C++ hanya digunakan untuk komunikasi hardware (HAL).
 * Semua logika kernel ditulis dalam PHP.
 */

#include "kernel.hpp"
#include "hal.hpp"
#include "php_runtime.hpp"
#include "limine.h"

/* ============================================================
 * C++ ABI STUBS — diperlukan tanpa libstdc++
 * ============================================================ */
extern "C" {
    int  __cxa_atexit(void (*)(void*), void*, void*) { return 0; }
    void __cxa_pure_virtual() { for(;;) asm volatile("hlt"); }
    void* __dso_handle = nullptr;
}

/* ============================================================
 * FUNGSI MEMORI DASAR — diperlukan oleh compiler
 * ============================================================ */
extern "C" void* memcpy(void* dest, const void* src, size_t n) {
    void* ret = dest;
    /* Fast path: bulk copy 8 bytes at a time */
    size_t qwords = n / 8;
    size_t remaining = n % 8;
    if (qwords > 0) {
        asm volatile(
            "rep movsq\n"
            : "+D"(dest), "+S"(src), "+c"(qwords)
            : : "memory"
        );
    }
    if (remaining > 0) {
        asm volatile(
            "rep movsb\n"
            : "+D"(dest), "+S"(src), "+c"(remaining)
            : : "memory"
        );
    }
    return ret;
}

extern "C" void* memset(void* dest, int val, size_t n) {
    void* ret = dest;
    uint8_t byte = (uint8_t)val;
    /* Build 8-byte pattern */
    uint64_t pattern = byte;
    pattern |= pattern << 8;
    pattern |= pattern << 16;
    pattern |= pattern << 32;
    size_t qwords = n / 8;
    size_t remaining = n % 8;
    if (qwords > 0) {
        asm volatile(
            "rep stosq\n"
            : "+D"(dest), "+c"(qwords)
            : "a"(pattern)
            : "memory"
        );
    }
    if (remaining > 0) {
        asm volatile(
            "rep stosb\n"
            : "+D"(dest), "+c"(remaining)
            : "a"(byte)
            : "memory"
        );
    }
    return ret;
}

extern "C" void* memmove(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    if (d < s || d >= s + n) {
        /* No overlap or forward direction is safe — use fast memcpy */
        memcpy(dest, src, n);
    } else {
        /* Backward copy for overlapping regions */
        for (size_t i = n; i > 0; i--) d[i-1] = s[i-1];
    }
    return dest;
}

extern "C" int memcmp(const void* a, const void* b, size_t n) {
    const uint8_t* pa = (const uint8_t*)a;
    const uint8_t* pb = (const uint8_t*)b;
    for (size_t i = 0; i < n; i++) {
        if (pa[i] != pb[i]) return pa[i] - pb[i];
    }
    return 0;
}

/* ============================================================
 * IMPLEMENTASI STRING — HAL string utilities
 * ============================================================ */
namespace hal {
namespace string {

size_t strlen(const char* s) {
    size_t len = 0;
    while (s && s[len]) len++;
    return len;
}

char* strcpy(char* dst, const char* src) {
    char* r = dst;
    while ((*dst++ = *src++));
    return r;
}

char* strncpy(char* dst, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++) dst[i] = src[i];
    for (; i < n; i++) dst[i] = '\0';
    return dst;
}

int strcmp(const char* a, const char* b) {
    while (*a && *a == *b) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char* a, const char* b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) return (unsigned char)a[i] - (unsigned char)b[i];
        if (a[i] == '\0') return 0;
    }
    return 0;
}

char* strcat(char* dst, const char* src) {
    char* r = dst;
    while (*dst) dst++;
    while ((*dst++ = *src++));
    return r;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) return (char*)s;
        s++;
    }
    return (c == 0) ? (char*)s : nullptr;
}

char* strstr(const char* hay, const char* needle) {
    if (!*needle) return (char*)hay;
    size_t nlen = strlen(needle);
    while (*hay) {
        if (strncmp(hay, needle, nlen) == 0) return (char*)hay;
        hay++;
    }
    return nullptr;
}

void itoa(int64_t num, char* buf) {
    if (num == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    bool neg = num < 0;
    if (neg) num = -num;
    char tmp[32];
    int i = 0;
    while (num > 0 && i < 30) {
        tmp[i++] = '0' + (char)(num % 10);
        num /= 10;
    }
    int j = 0;
    if (neg) buf[j++] = '-';
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

void utoa(uint64_t num, char* buf) {
    if (num == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[32];
    int i = 0;
    while (num > 0 && i < 30) {
        tmp[i++] = '0' + (char)(num % 10);
        num /= 10;
    }
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

void htoa(uint64_t num, char* buf) {
    const char hex[] = "0123456789abcdef";
    if (num == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[32];
    int i = 0;
    while (num > 0 && i < 30) {
        tmp[i++] = hex[num & 0xF];
        num >>= 4;
    }
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

int64_t atoi(const char* s) {
    if (!s) return 0;
    int64_t result = 0;
    bool neg = false;
    while (*s == ' ') s++;
    if (*s == '-') { neg = true; s++; }
    else if (*s == '+') { s++; }
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    return neg ? -result : result;
}

char to_upper(char c) { return (c >= 'a' && c <= 'z') ? c - 32 : c; }
char to_lower(char c) { return (c >= 'A' && c <= 'Z') ? c + 32 : c; }
bool is_digit(char c) { return c >= '0' && c <= '9'; }
bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
bool is_space(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
bool is_alnum(char c) { return is_digit(c) || is_alpha(c); }

} // namespace string
} // namespace hal

/* ============================================================
 * LIMINE BOOT REQUESTS
 * Permintaan ke bootloader Limine untuk framebuffer, memory map, dll
 * ============================================================ */

/* Limine base revision */
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(3);

/* Marker awal requests */
__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start[] = LIMINE_REQUESTS_START_MARKER;

/* Framebuffer request */
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    {LIMINE_COMMON_MAGIC, 0x9d5827dcd881dd75, 0xa3148604f6fab11b},
    0, 0
};

/* Memory map request */
__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    {LIMINE_COMMON_MAGIC, 0x67cf3d9d378a806f, 0xe304acdfc50c3c62},
    0, 0
};

/* HHDM (Higher Half Direct Map) request */
__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    {LIMINE_COMMON_MAGIC, 0x48dcf1cb8ad2b852, 0x63984e959a98244b},
    0, 0
};

/* Marker akhir requests */
__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end[] = LIMINE_REQUESTS_END_MARKER;

/* ============================================================
 * PHP KERNEL SCRIPT — Inti logika kernel dalam PHP 8
 * Script ini dijalankan oleh PHP Runtime setelah HAL siap
 * ============================================================ */
static const char* kernel_php_script = R"PHP(<?php
//
// LittleOS Gajah PHP — Desktop GUI Script
// Kernel murni PHP 8 — bukan UNIX, bukan Linux, bukan DOS
// Tampilan Desktop Tailwind CSS
//

// ============================================================
// HELPER FUNCTIONS
// ============================================================

function zero_pad($n) {
    if ($n < 10) {
        return "0" . $n;
    }
    return "" . $n;
}

function format_bytes($bytes) {
    if ($bytes >= 1073741824) {
        return ($bytes / 1073741824) . " GB";
    }
    if ($bytes >= 1048576) {
        return ($bytes / 1048576) . " MB";
    }
    if ($bytes >= 1024) {
        return ($bytes / 1024) . " KB";
    }
    return $bytes . " B";
}

function get_time_string() {
    return zero_pad(rtc_hour()) . ":" . zero_pad(rtc_minute()) . ":" . zero_pad(rtc_second());
}

function get_date_string() {
    return zero_pad(rtc_day()) . "/" . zero_pad(rtc_month()) . "/" . rtc_year();
}

function memory_bar($width) {
    $total = memory_total();
    $used = memory_used();
    if ($total == 0) { return "[" . str_repeat("?", $width) . "]"; }
    $pct = ($used * $width) / $total;
    $bar = "[";
    $i = 0;
    while ($i < $pct) { $bar = $bar . "#"; $i = $i + 1; }
    while ($i < $width) { $bar = $bar . "."; $i = $i + 1; }
    $bar = $bar . "] " . (($used * 100) / $total) . "%";
    return $bar;
}

// ============================================================
// APPLICATION FUNCTIONS
// ============================================================

function open_app($name) {
    if ($name == "Sistem") { app_sistem(); }
    elseif ($name == "Setting") { app_setting(); }
    elseif ($name == "File Manager") { app_file_manager(); }
    elseif ($name == "Browser") { app_browser(); }
    elseif ($name == "Terminal") { app_terminal(); }
    elseif ($name == "Task Manager") { app_task_manager(); }
    elseif ($name == "Clock Date") { app_clock(); }
}

function app_sistem() {
    $wid = desktop_create_window("Sistem", 120, 60, 520, 420, "sistem");
    $t = "";
    $t = $t . "  ____  _   _ ____     ___  ____\n";
    $t = $t . " |  _ \\| | | |  _ \\   / _ \\/ ___|\n";
    $t = $t . " | |_) | |_| | |_) | | | | \\___ \\\n";
    $t = $t . " |  __/|  _  |  __/  | |_| |___) |\n";
    $t = $t . " |_|   |_| |_|_|      \\___/|____/\n\n";
    $t = $t . " LittleOS Gajah PHP 8.2\n";
    $t = $t . " ==========================\n\n";
    $t = $t . " OS          : " . kernel_name() . "\n";
    $t = $t . " Version     : " . kernel_version() . "\n";
    $t = $t . " Arch        : " . kernel_arch() . "\n";
    $t = $t . " Kernel      : PHP 8.2 Runtime\n";
    $t = $t . " DE          : LittleOS Desktop\n";
    $t = $t . " WM          : LittleOS WM (Tailwind)\n";
    $t = $t . " Bootloader  : Limine 8.x\n";
    $t = $t . " Language    : PHP 8.2\n";
    $t = $t . " UI/UX       : Tailwind CSS Palette\n\n";
    $t = $t . " -- Hardware --\n";
    $t = $t . " CPU         : x86_64 (Long Mode)\n";
    $t = $t . " Memori      : " . format_bytes(memory_total()) . "\n";
    $t = $t . " Free        : " . format_bytes(memory_free()) . "\n";
    $t = $t . " Used        : " . format_bytes(memory_used()) . "\n";
    $t = $t . " " . memory_bar(30) . "\n";
    $t = $t . " Layar       : " . screen_width() . "x" . screen_height() . "\n";
    $t = $t . " Uptime      : " . uptime() . " detik\n\n";
    $t = $t . " -- Driver Status --\n";
    $t = $t . " Framebuffer : Aktif (VESA/LFB)\n";
    $t = $t . " Keyboard    : PS/2 Aktif\n";
    $t = $t . " Mouse       : PS/2 Aktif\n";
    $t = $t . " Timer       : PIT 1000Hz\n";
    $t = $t . " RTC         : CMOS Aktif\n";
    $t = $t . " PCI         : Enumerasi OK\n\n";
    $t = $t . " 100% PHP Kernel. Bukan UNIX.\n";
    desktop_set_text($wid, $t);
}

function app_setting() {
    $wid = desktop_create_window("Pengaturan", 160, 80, 480, 380, "setting");
    $t = " === Pengaturan Sistem ===\n\n";
    $t = $t . " [Display]\n";
    $t = $t . "   Resolusi    : " . screen_width() . "x" . screen_height() . "\n";
    $t = $t . "   Theme       : Tailwind CSS Dark\n";
    $t = $t . "   Font        : CP437 8x8 Bitmap\n";
    $t = $t . "   Backbuffer  : Double Buffered\n\n";
    $t = $t . " [Kernel]\n";
    $t = $t . "   Runtime     : PHP 8.2 Interpreter\n";
    $t = $t . "   Mode        : Desktop GUI\n";
    $t = $t . "   HAL         : C++ x86_64\n\n";
    $t = $t . " [Warna Tema Tailwind]\n";
    $t = $t . "   Desktop BG  : slate-800 (#1E293B)\n";
    $t = $t . "   Taskbar     : slate-950 (gradient)\n";
    $t = $t . "   Window      : slate-800 (#1E293B)\n";
    $t = $t . "   Title Active: blue-600  (#2563EB)\n";
    $t = $t . "   Accent      : sky-500   (#0EA5E9)\n";
    $t = $t . "   Close Btn   : red-500   (#EF4444)\n";
    $t = $t . "   Max Btn     : green-500 (#22C55E)\n";
    $t = $t . "   Min Btn     : yellow-400(#FACC15)\n\n";
    $t = $t . " [Input]\n";
    $t = $t . "   Keyboard    : PS/2 (US Layout)\n";
    $t = $t . "   Mouse       : PS/2 Relative\n";
    $t = $t . "   Layout      : EN-US\n\n";
    $t = $t . " [Jaringan]\n";
    $t = $t . "   Status      : Tidak tersedia\n";
    $t = $t . "   WiFi        : Emulasi (ikon saja)\n";
    desktop_set_text($wid, $t);
}

function app_file_manager() {
    $wid = desktop_create_window("File Manager", 140, 70, 500, 400, "files");
    $t = " === File Manager PHP ===\n\n";
    $t = $t . " Lokasi: /kernel/main/\n\n";
    $t = $t . "  Nama               Tipe     Size\n";
    $t = $t . "  ----               ----     ----\n";
    $t = $t . "  kernel.php         PHP      2.1 KB\n";
    $t = $t . "  console.php        PHP      1.8 KB\n";
    $t = $t . "  memory.php         PHP      1.2 KB\n";
    $t = $t . "  graphics.php       PHP      2.4 KB\n";
    $t = $t . "  interrupts.php     PHP      0.8 KB\n";
    $t = $t . "  keyboard.php       PHP      0.6 KB\n";
    $t = $t . "  mouse.php          PHP      0.7 KB\n";
    $t = $t . "  timer.php          PHP      0.5 KB\n";
    $t = $t . "  rtc.php            PHP      0.9 KB\n";
    $t = $t . "  pci.php            PHP      1.1 KB\n";
    $t = $t . "  driver.php         PHP      1.3 KB\n";
    $t = $t . "  system_manager.php PHP      3.5 KB\n";
    $t = $t . "  string.php         PHP      0.9 KB\n";
    $t = $t . "  ports.php          PHP      0.4 KB\n";
    $t = $t . "  limine.php         PHP      0.6 KB\n\n";
    $t = $t . " 15 file  |  Total: 18.8 KB\n";
    $t = $t . " Semua file PHP 8.2 script.\n\n";
    $t = $t . " [/boot/]\n";
    $t = $t . "  limine.conf        CONF     0.3 KB\n";
    $t = $t . "  littleos           ELF64    ~2 MB\n";
    desktop_set_text($wid, $t);
}

function app_browser() {
    $wid = desktop_create_window("PHP Browser", 100, 50, 560, 420, "browser");
    $t = " === LittleOS PHP Browser ===\n\n";
    $t = $t . " [php://localhost]\n";
    $t = $t . " Status: 200 OK | Render: PHP\n";
    $t = $t . " ____________________________\n\n";
    $t = $t . " <!DOCTYPE html>\n";
    $t = $t . " <html lang='id'>\n";
    $t = $t . " <head>\n";
    $t = $t . "   <meta charset='UTF-8'>\n";
    $t = $t . "   <title>LittleOS</title>\n";
    $t = $t . "   <script src='tailwind.js'>\n";
    $t = $t . " </head>\n";
    $t = $t . " <body class='bg-slate-900\n";
    $t = $t . "   text-white min-h-screen'>\n\n";
    $t = $t . "   <nav class='bg-slate-800 p-4'>\n";
    $t = $t . "     <a class='text-sky-400\n";
    $t = $t . "       font-bold text-xl'>\n";
    $t = $t . "       LittleOS Gajah\n";
    $t = $t . "     </a>\n";
    $t = $t . "   </nav>\n\n";
    $t = $t . "   <main class='container p-8'>\n";
    $t = $t . "     <h1 class='text-4xl mb-4\n";
    $t = $t . "       font-bold text-sky-500'>\n";
    $t = $t . "       Selamat Datang!\n";
    $t = $t . "     </h1>\n";
    $t = $t . "     <p class='text-slate-300'>\n";
    $t = $t . "       OS pertama dengan kernel\n";
    $t = $t . "       PHP 8.2 bare metal.\n";
    $t = $t . "     </p>\n";
    $t = $t . "     <?php echo 'Powered by PHP'; ?>\n";
    $t = $t . "   </main>\n";
    $t = $t . " </body>\n";
    $t = $t . " </html>\n";
    desktop_set_text($wid, $t);
}

function app_terminal() {
    $wid = desktop_create_window("Terminal PHP", 80, 90, 560, 400, "terminal");
    $t = " LittleOS Gajah PHP Terminal v1.0\n";
    $t = $t . " PHP 8.2 Runtime | x86_64 Bare Metal\n";
    $t = $t . " ------------------------------------\n\n";
    $t = $t . " php> echo 'Hello World!';\n";
    $t = $t . " Hello World!\n\n";
    $t = $t . " php> kernel_name();\n";
    $t = $t . " " . kernel_name() . "\n\n";
    $t = $t . " php> memory_total() / 1048576;\n";
    $t = $t . " " . (memory_total() / 1048576) . " MB\n\n";
    $t = $t . " php> phpinfo();\n";
    $t = $t . " PHP 8.2 (LittleOS Runtime)\n";
    $t = $t . " Zend Engine: LittleOS PHP VM\n";
    $t = $t . " Architecture: x86_64\n\n";
    $t = $t . " php> echo date('H:i:s');\n";
    $t = $t . " " . get_time_string() . "\n\n";
    $t = $t . " php> $arr = [1, 2, 3];\n";
    $t = $t . " php> echo count($arr);\n";
    $t = $t . " 3\n\n";
    $t = $t . " php> echo strtoupper('littleos');\n";
    $t = $t . " LITTLEOS\n\n";
    $t = $t . " php> _\n";
    desktop_set_text($wid, $t);
}

function app_task_manager() {
    $wid = desktop_create_window("Task Manager", 180, 50, 500, 400, "tasks");
    $t = " === Task Manager PHP ===\n\n";
    $t = $t . " PID  NAMA               STATUS    MEM\n";
    $t = $t . " ---  ----               ------    ---\n";
    $t = $t . "   1  php_runtime        Running   " . format_bytes(memory_used()) . "\n";
    $t = $t . "   2  desktop_manager    Running   2 MB\n";
    $t = $t . "   3  window_manager     Running   512 KB\n";
    $t = $t . "   4  mouse_driver       Running   64 KB\n";
    $t = $t . "   5  keyboard_driver    Running   32 KB\n";
    $t = $t . "   6  timer_service      Running   16 KB\n";
    $t = $t . "   7  rtc_service        Running   8 KB\n";
    $t = $t . "   8  pci_enumerator     Idle      4 KB\n\n";
    $t = $t . " -----------------------------------\n";
    $t = $t . " MEMORI:\n";
    $t = $t . "   Total     : " . format_bytes(memory_total()) . "\n";
    $t = $t . "   Digunakan : " . format_bytes(memory_used()) . "\n";
    $t = $t . "   Tersedia  : " . format_bytes(memory_free()) . "\n";
    $t = $t . "   " . memory_bar(30) . "\n\n";
    $t = $t . " CPU:\n";
    $t = $t . "   Arsitektur: x86_64 (Long Mode)\n";
    $t = $t . "   Mode      : Protected + Paging\n\n";
    $t = $t . " UPTIME: " . uptime() . " detik\n";
    $t = $t . " KERNEL: PHP 8.2 Runtime\n";
    desktop_set_text($wid, $t);
}

function app_clock() {
    $wid = desktop_create_window("Jam & Tanggal", 250, 100, 380, 320, "clock");
    $t = " === Jam & Tanggal ===\n\n";
    $t = $t . "    ___  ___  ___\n";
    $t = $t . "   |" . zero_pad(rtc_hour()) . " |" . zero_pad(rtc_minute()) . " |" . zero_pad(rtc_second()) . " |\n";
    $t = $t . "   |___|:|___|:|___|\n\n";
    $t = $t . " Waktu    : " . get_time_string() . "\n";
    $t = $t . " Tanggal  : " . get_date_string() . "\n\n";
    $t = $t . " Detail:\n";
    $t = $t . "   Jam     : " . rtc_hour() . "\n";
    $t = $t . "   Menit   : " . rtc_minute() . "\n";
    $t = $t . "   Detik   : " . rtc_second() . "\n";
    $t = $t . "   Hari    : " . rtc_day() . "\n";
    $t = $t . "   Bulan   : " . rtc_month() . "\n";
    $t = $t . "   Tahun   : " . rtc_year() . "\n\n";
    $t = $t . " Timezone  : UTC (CMOS RTC)\n";
    $t = $t . " Uptime    : " . uptime() . " detik\n";
    $t = $t . " Ticks     : " . uptime_ms() . " ms\n\n";
    $t = $t . " [PHP RTC Driver Active]\n";
    desktop_set_text($wid, $t);
}

function app_about() {
    $wid = desktop_create_window("Tentang LittleOS", 200, 90, 420, 320, "sistem");
    $t = "\n";
    $t = $t . "    LittleOS Gajah PHP\n";
    $t = $t . "    ==================\n\n";
    $t = $t . "    Versi " . kernel_version() . "\n\n";
    $t = $t . "    OS pertama di dunia dengan\n";
    $t = $t . "    kernel murni PHP 8.2 yang\n";
    $t = $t . "    berjalan di bare metal x86_64.\n\n";
    $t = $t . "    Ini BUKAN UNIX.\n";
    $t = $t . "    Ini BUKAN Linux.\n";
    $t = $t . "    Ini BUKAN DOS.\n\n";
    $t = $t . "    HAL  : C++ (hardware layer)\n";
    $t = $t . "    GUI  : Tailwind CSS palette\n";
    $t = $t . "    Boot : Limine bootloader\n\n";
    $t = $t . "    (c) 2024-2026 LittleOS\n";
    $t = $t . "    Lisensi: GNU General Public License v3.0\n";
    desktop_set_text($wid, $t);
}

// ============================================================
// CONTEXT MENU ACTION HANDLER
// ============================================================

function handle_context_action($action) {
    if ($action == "open_terminal") { app_terminal(); }
    elseif ($action == "open_files") { app_file_manager(); }
    elseif ($action == "open_browser") { app_browser(); }
    elseif ($action == "open_sistem") { app_sistem(); }
    elseif ($action == "open_tasks") { app_task_manager(); }
    elseif ($action == "open_setting") { app_setting(); }
    elseif ($action == "display_settings") { app_setting(); }
    elseif ($action == "change_wallpaper") { app_setting(); }
    elseif ($action == "about") { app_about(); }
    elseif ($action == "refresh") {
        // Refresh = just redraw
    }
    elseif ($action == "show_desktop") {
        // Minimize semua — belum didukung batch
    }
    elseif ($action == "close_win") {
        // Ditangani di C++ level
    }
}

// ============================================================
// DESKTOP MAIN LOOP
// ============================================================

desktop_init();

while (true) {
    $event = desktop_poll_events();

    // Handle start menu items
    if ($event == "Sistem") { open_app("Sistem"); }
    elseif ($event == "Setting") { open_app("Setting"); }
    elseif ($event == "File Manager") { open_app("File Manager"); }
    elseif ($event == "Browser") { open_app("Browser"); }
    elseif ($event == "Terminal") { open_app("Terminal"); }
    elseif ($event == "Task Manager") { open_app("Task Manager"); }
    elseif ($event == "Clock Date") { open_app("Clock Date"); }

    // Handle context menu actions
    elseif ($event == "open_terminal") { handle_context_action($event); }
    elseif ($event == "open_files") { handle_context_action($event); }
    elseif ($event == "open_browser") { handle_context_action($event); }
    elseif ($event == "open_sistem") { handle_context_action($event); }
    elseif ($event == "open_tasks") { handle_context_action($event); }
    elseif ($event == "open_setting") { handle_context_action($event); }
    elseif ($event == "display_settings") { handle_context_action($event); }
    elseif ($event == "change_wallpaper") { handle_context_action($event); }
    elseif ($event == "about") { handle_context_action($event); }
    elseif ($event == "refresh") { handle_context_action($event); }
    elseif ($event == "show_desktop") { handle_context_action($event); }

    desktop_render();
    sleep(16);
}
)PHP";

/* ============================================================
 * KERNEL ENTRY POINT — kmain()
 * Dipanggil oleh Limine bootloader setelah setup awal
 * ============================================================ */
extern "C" NORETURN void kmain(void) {
    /* ---- Cek framebuffer dari Limine ---- */
    if (!framebuffer_request.response ||
        framebuffer_request.response->framebuffer_count < 1) {
        /* Tidak ada framebuffer — halt */
        for (;;) asm volatile("hlt");
    }

    struct limine_framebuffer* fb = framebuffer_request.response->framebuffers[0];

    /* ---- TAHAP 1: Inisialisasi Console (C++ HAL) ---- */
    hal::console::init(
        (void*)fb->address,
        fb->width,
        fb->height,
        fb->pitch
    );
    hal::console::clear();

    hal::console::puts_colored("[ HAL ] ", COLOR_ACCENT);
    hal::console::puts("Framebuffer console aktif (");
    hal::console::put_number((int64_t)fb->width);
    hal::console::puts("x");
    hal::console::put_number((int64_t)fb->height);
    hal::console::puts(")\n");

    /* ---- TAHAP 2: Inisialisasi Memory (C++ HAL) ---- */
    if (memmap_request.response && hhdm_request.response) {
        hal::memory::init(
            memmap_request.response,
            hhdm_request.response->offset
        );
        hal::console::puts_colored("[ HAL ] ", COLOR_ACCENT);
        hal::console::puts("Physical memory manager aktif (");
        char buf[32];
        hal::string::utoa(hal::memory::get_total() / MEGABYTE, buf);
        hal::console::puts(buf);
        hal::console::puts(" MB total)\n");
    }

    hal::memory::heap_init();
    hal::console::puts_colored("[ HAL ] ", COLOR_ACCENT);
    hal::console::puts("Heap allocator aktif\n");

    /* ---- TAHAP 3: Inisialisasi Interrupts (C++ HAL) ---- */
    hal::interrupts::init();
    hal::console::puts_colored("[ HAL ] ", COLOR_ACCENT);
    hal::console::puts("IDT dan PIC aktif\n");

    /* ---- TAHAP 4: Inisialisasi Timer (C++ HAL) ---- */
    hal::timer::init();
    hal::console::puts_colored("[ HAL ] ", COLOR_ACCENT);
    hal::console::puts("PIT Timer aktif (1000 Hz)\n");

    /* ---- TAHAP 5: Inisialisasi Keyboard (C++ HAL) ---- */
    hal::keyboard::init();
    hal::console::puts_colored("[ HAL ] ", COLOR_ACCENT);
    hal::console::puts("PS/2 Keyboard aktif\n");

    /* ---- TAHAP 6: Inisialisasi Mouse (C++ HAL) ---- */
    hal::mouse::init();
    hal::console::puts_colored("[ HAL ] ", COLOR_ACCENT);
    hal::console::puts("PS/2 Mouse aktif\n");

    /* ---- TAHAP 7: Inisialisasi RTC (C++ HAL) ---- */
    hal::rtc::init();
    hal::console::puts_colored("[ HAL ] ", COLOR_ACCENT);
    hal::console::puts("CMOS RTC aktif\n");

    /* ---- Aktifkan hardware interrupts ---- */
    hal::interrupts::enable();
    hal::console::puts_colored("[ HAL ] ", COLOR_ACCENT);
    hal::console::puts("Interrupts enabled\n");

    hal::console::puts("\n");
    hal::console::puts_colored("[ HAL ] ", COLOR_SUCCESS);
    hal::console::puts("Hardware Abstraction Layer (C++) siap.\n");
    hal::console::puts_colored("[ PHP ] ", COLOR_SUCCESS);
    hal::console::puts("Memulai PHP 8 Desktop Runtime...\n");
    hal::console::puts("\n");

    /* Tunggu sebentar agar user bisa lihat boot log */
    hal::timer::wait_ms(1500);

    /* ---- BOOT SPLASH SCREEN ---- */
    {
        int32_t sw = (int32_t)fb->width;
        int32_t sh = (int32_t)fb->height;

        hal::console::begin_frame();

        /* Background gelap gradient-like */
        hal::console::fill_rect(0, 0, sw, sh, TW_SLATE_950);

        /* Blit mascot di tengah-atas */
        int32_t mascot_x = (sw - hal::assets::MASCOT_W) / 2;
        int32_t mascot_y = sh / 2 - hal::assets::MASCOT_H - 30;
        hal::console::blit_raw(mascot_x, mascot_y,
            hal::assets::MASCOT_W, hal::assets::MASCOT_H,
            hal::assets::mascot_pixels());

        /* Nama OS di bawah mascot */
        const char* os_title = "LittleOS Gajah PHP";
        int title_w = hal::console::string_pixel_width(os_title);
        hal::console::draw_string((sw - title_w) / 2, sh / 2 + 4, os_title, TW_SKY_400);

        /* Subtitle */
        const char* subtitle = "PHP 8 Runtime | x86_64 Bare Metal";
        int sub_w = hal::console::string_pixel_width(subtitle);
        hal::console::draw_string((sw - sub_w) / 2, sh / 2 + 20, subtitle, TW_SLATE_400);

        /* System info */
        const char* info1 = "Kernel: " LITTLEOS_KERNEL "  |  Arch: " LITTLEOS_ARCH "  |  DE: " LITTLEOS_DE;
        int info1_w = hal::console::string_pixel_width(info1);
        hal::console::draw_string((sw - info1_w) / 2, sh / 2 + 50, info1, TW_SLATE_500);

        const char* info2 = "Display: " LITTLEOS_DISPLAY "  |  Bootloader: " LITTLEOS_BOOTLOADER "  |  UI/UX: " LITTLEOS_UIUX;
        int info2_w = hal::console::string_pixel_width(info2);
        hal::console::draw_string((sw - info2_w) / 2, sh / 2 + 64, info2, TW_SLATE_500);

        /* Loading bar background */
        int bar_w = 300;
        int bar_h = 8;
        int bar_x = (sw - bar_w) / 2;
        int bar_y = sh / 2 + 96;
        hal::console::fill_rounded_rect(bar_x, bar_y, bar_w, bar_h, 4, TW_SLATE_800);

        hal::console::end_frame();

        /* Animate loading bar */
        for (int progress = 0; progress <= bar_w - 4; progress += 6) {
            hal::console::begin_frame();

            /* Redraw full screen */
            hal::console::fill_rect(0, 0, sw, sh, TW_SLATE_950);
            hal::console::blit_raw(mascot_x, mascot_y,
                hal::assets::MASCOT_W, hal::assets::MASCOT_H,
                hal::assets::mascot_pixels());
            hal::console::draw_string((sw - title_w) / 2, sh / 2 + 4, os_title, TW_SKY_400);
            hal::console::draw_string((sw - sub_w) / 2, sh / 2 + 20, subtitle, TW_SLATE_400);
            hal::console::draw_string((sw - info1_w) / 2, sh / 2 + 50, info1, TW_SLATE_500);
            hal::console::draw_string((sw - info2_w) / 2, sh / 2 + 64, info2, TW_SLATE_500);

            /* Bar background + filled portion */
            hal::console::fill_rounded_rect(bar_x, bar_y, bar_w, bar_h, 4, TW_SLATE_800);
            if (progress > 0) {
                hal::console::fill_rounded_rect(bar_x + 2, bar_y + 2, progress, bar_h - 4, 2, TW_SKY_500);
            }

            /* Loading text */
            const char* loading = "Memuat Desktop...";
            int lt_w = hal::console::string_pixel_width(loading);
            hal::console::draw_string((sw - lt_w) / 2, bar_y + 16, loading, TW_SLATE_500);

            hal::console::end_frame();
            hal::timer::wait_ms(30);
        }

        /* Hold splash briefly */
        hal::timer::wait_ms(500);
    }

    /* ---- TAHAP 8: Jalankan PHP 8 Desktop ---- */
    static PhpRuntime php_runtime;
    php_runtime.init();
    php_runtime.execute_script(kernel_php_script);

    /* Jika PHP script selesai (seharusnya tidak), halt */
    hal::console::puts_colored("\n[KERNEL] ", COLOR_ERROR);
    hal::console::puts("PHP Runtime berhenti. System halted.\n");
    for (;;) asm volatile("hlt");
}
