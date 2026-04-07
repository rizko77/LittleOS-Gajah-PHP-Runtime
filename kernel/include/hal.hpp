/*
 * LittleOS Gajah PHP - hal.hpp
 * Hardware Abstraction Layer (C++ untuk komunikasi hardware)
 * Semua akses hardware dilakukan melalui C++ — bukan C
 *
 * Versi Desktop: mendukung GUI, mouse, window manager
 * Tema warna: Tailwind CSS color palette
 */

#pragma once

#include "kernel.hpp"
#include "limine.h"

namespace hal {

/* ============================================================
 * PORT I/O — akses langsung ke port hardware x86
 * ============================================================ */
namespace ports {

static inline uint8_t inb(uint16_t port) {
    uint8_t r;
    asm volatile("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t r;
    asm volatile("inw %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}

static inline void outw(uint16_t port, uint16_t val) {
    asm volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t ind(uint16_t port) {
    uint32_t r;
    asm volatile("inl %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}

static inline void outd(uint16_t port, uint32_t val) {
    asm volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline void io_wait() {
    asm volatile("outb %%al, $0x80" : : "a"(0));
}

} // namespace ports

/* ============================================================
 * STRING — fungsi manipulasi string
 * ============================================================ */
namespace string {

size_t  strlen(const char* s);
char*   strcpy(char* dst, const char* src);
char*   strncpy(char* dst, const char* src, size_t n);
int     strcmp(const char* a, const char* b);
int     strncmp(const char* a, const char* b, size_t n);
char*   strcat(char* dst, const char* src);
char*   strchr(const char* s, int c);
char*   strstr(const char* hay, const char* needle);
void    itoa(int64_t num, char* buf);
void    utoa(uint64_t num, char* buf);
void    htoa(uint64_t num, char* buf);
int64_t atoi(const char* s);
char    to_upper(char c);
char    to_lower(char c);
bool    is_digit(char c);
bool    is_alpha(char c);
bool    is_space(char c);
bool    is_alnum(char c);

} // namespace string

/* ============================================================
 * CONSOLE — output teks framebuffer
 * ============================================================ */

/* ---- Tailwind CSS Color Palette ---- */
#define TW_SLATE_50    0xFFF8FAFC
#define TW_SLATE_100   0xFFF1F5F9
#define TW_SLATE_200   0xFFE2E8F0
#define TW_SLATE_300   0xFFCBD5E1
#define TW_SLATE_400   0xFF94A3B8
#define TW_SLATE_500   0xFF64748B
#define TW_SLATE_600   0xFF475569
#define TW_SLATE_700   0xFF334155
#define TW_SLATE_800   0xFF1E293B
#define TW_SLATE_900   0xFF0F172A
#define TW_SLATE_950   0xFF020617

#define TW_SKY_400     0xFF38BDF8
#define TW_SKY_500     0xFF0EA5E9
#define TW_SKY_600     0xFF0284C7

#define TW_BLUE_400    0xFF60A5FA
#define TW_BLUE_500    0xFF3B82F6
#define TW_BLUE_600    0xFF2563EB
#define TW_BLUE_700    0xFF1D4ED8

#define TW_INDIGO_500  0xFF6366F1
#define TW_INDIGO_600  0xFF4F46E5

#define TW_PURPLE_500  0xFFA855F7
#define TW_PURPLE_600  0xFF9333EA

#define TW_GREEN_400   0xFF4ADE80
#define TW_GREEN_500   0xFF22C55E
#define TW_GREEN_600   0xFF16A34A

#define TW_RED_400     0xFFF87171
#define TW_RED_500     0xFFEF4444
#define TW_RED_600     0xFFDC2626

#define TW_YELLOW_400  0xFFFACC15
#define TW_AMBER_500   0xFFF59E0B

#define TW_ORANGE_400  0xFFFB923C
#define TW_ORANGE_500  0xFFF97316

#define TW_WHITE       0xFFFFFFFF
#define TW_BLACK       0xFF000000

/* Warna ARGB 32-bit (alias) */
#define COLOR_BLACK         0xFF000000
#define COLOR_WHITE         0xFFFFFFFF
#define COLOR_RED           TW_RED_500
#define COLOR_GREEN         TW_GREEN_500
#define COLOR_BLUE          TW_BLUE_500
#define COLOR_YELLOW        TW_YELLOW_400
#define COLOR_CYAN          0xFF06B6D4
#define COLOR_MAGENTA       TW_PURPLE_500
#define COLOR_GRAY          TW_SLATE_500
#define COLOR_DARK_GRAY     TW_SLATE_700
#define COLOR_LIGHT_GRAY    TW_SLATE_300

/* Tema LittleOS Gajah (Tailwind-based) */
#define COLOR_BG            TW_SLATE_900
#define COLOR_FG            TW_SLATE_100
#define COLOR_ACCENT        TW_SKY_500
#define COLOR_SUCCESS       TW_GREEN_500
#define COLOR_WARNING       TW_AMBER_500
#define COLOR_ERROR         TW_RED_500

/* Desktop theme */
#define DESKTOP_BG          TW_SLATE_800
#define TASKBAR_BG          TW_SLATE_950
#define TASKBAR_HEIGHT      36
#define WINDOW_TITLE_BG     TW_SLATE_700
#define WINDOW_TITLE_ACTIVE TW_BLUE_600
#define WINDOW_BG           TW_SLATE_800
#define WINDOW_BORDER       TW_SLATE_600
#define WINDOW_TEXT         TW_SLATE_100
#define BTN_CLOSE_BG        TW_RED_500
#define BTN_MAX_BG          TW_GREEN_500
#define BTN_MIN_BG          TW_YELLOW_400

namespace console {

void init(void* fb_addr, uint64_t width, uint64_t height, uint64_t pitch);
void putchar(char c);
void puts(const char* s);
void puts_colored(const char* s, uint32_t color);
void put_number(int64_t n);
void put_hex(uint64_t n);
void set_fg(uint32_t color);
void set_bg(uint32_t color);
void clear();
void newline();
void scroll_up();
void set_cursor(uint32_t col, uint32_t row);
uint32_t get_width();
uint32_t get_height();
uint64_t get_fb_width();
uint64_t get_fb_height();
void printf(const char* fmt, ...);

/* Grafis primitif */
void draw_pixel(int32_t x, int32_t y, uint32_t color);
void fill_rect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void draw_rect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void draw_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
void fill_circle(int32_t cx, int32_t cy, int32_t r, uint32_t color);
void fill_rounded_rect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
void draw_char(int32_t x, int32_t y, char c, uint32_t fg, uint32_t bg);
void draw_char_transparent(int32_t x, int32_t y, char c, uint32_t fg);
void draw_string(int32_t x, int32_t y, const char* s, uint32_t fg);
void draw_string_bg(int32_t x, int32_t y, const char* s, uint32_t fg, uint32_t bg);
int  string_pixel_width(const char* s);

/* Double buffering */
void swap_buffers();
void begin_frame();   /* Set render target ke backbuffer */
void end_frame();     /* Copy backbuffer → framebuffer, kembali ke direct render */
uint32_t* get_backbuffer();
void* get_framebuffer();
uint64_t get_pitch();

/* Image blitting */
void blit_raw(int32_t dx, int32_t dy, int32_t sw, int32_t sh, const uint32_t* pixels);

} // namespace console

/* ============================================================
 * MEMORY — manajemen memori fisik dan heap
 * ============================================================ */
namespace memory {

void      init(struct limine_memmap_response* memmap, uint64_t hhdm_offset);
void*     alloc_page();
void      free_page(void* addr);
uint64_t  get_total();
uint64_t  get_free();
uint64_t  get_used();

/* Heap dinamis */
void  heap_init();
void* kmalloc(size_t size);
void  kfree(void* ptr);
void* kcalloc(size_t count, size_t size);
void* krealloc(void* ptr, size_t new_size);

} // namespace memory

/* ============================================================
 * INTERRUPTS — IDT dan penanganan interupsi
 * ============================================================ */
namespace interrupts {

void init();
void enable();
void disable();

} // namespace interrupts

/* ============================================================
 * TIMER — PIT timer untuk pencatat waktu
 * ============================================================ */
namespace timer {

void     init();
uint64_t get_ticks();
uint64_t get_ms();
uint64_t get_seconds();
void     wait_ms(uint64_t ms);

} // namespace timer

/* ============================================================
 * KEYBOARD — driver keyboard PS/2
 * ============================================================ */
namespace keyboard {

void init();
char read_char();
char read_char_nonblocking();
bool has_input();
void read_line(char* buf, size_t max_len);
uint8_t read_scancode_nonblocking();
bool is_shift_held();
bool is_ctrl_held();

} // namespace keyboard

/* ============================================================
 * MOUSE — driver mouse PS/2
 * ============================================================ */
namespace mouse {

void init();
int32_t get_x();
int32_t get_y();
bool is_left_pressed();
bool is_right_pressed();
bool has_event();

struct MouseEvent {
    int32_t x, y;
    int32_t dx, dy;
    bool left, right, middle;
    bool clicked;
    bool released;
    bool right_clicked;
};

MouseEvent poll_event();

} // namespace mouse

/* ============================================================
 * RTC — Real-Time Clock
 * ============================================================ */
namespace rtc {

void init();

struct DateTime {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t weekday;
};

DateTime get_time();

} // namespace rtc

/* ============================================================
 * DESKTOP — Window Manager (C++ backend)
 * ============================================================ */
namespace desktop {

static const int MAX_WINDOWS = 32;
static const int TITLE_BAR_H = 28;
static const int BORDER_W    = 1;

enum class WindowState : uint8_t {
    Normal,
    Minimized,
    Maximized,
    Closed
};

struct Window {
    int32_t  id;
    char     title[128];
    int32_t  x, y, w, h;
    int32_t  saved_x, saved_y, saved_w, saved_h;
    WindowState state;
    bool     active;
    bool     dirty;
    char     text_content[8192];
    int      text_len;
    int      text_scroll;
    uint32_t bg_color;
    uint32_t title_color;
    char     app_type[64];
};

/* Context menu item */
static const int MAX_CTX_ITEMS = 12;
struct ContextMenuItem {
    char     label[64];
    char     action[64];
    bool     separator;  /* true = garis pemisah, bukan item */
};

struct ContextMenu {
    bool     visible;
    int32_t  x, y;
    int      item_count;
    ContextMenuItem items[MAX_CTX_ITEMS];
};

struct DesktopState {
    Window   windows[MAX_WINDOWS];
    int      window_count;
    int      active_window;
    int      z_order[MAX_WINDOWS];
    int      z_count;
    bool     running;
    bool     needs_redraw;
    int32_t  screen_w, screen_h;
    bool     start_menu_open;
    bool     dragging;
    int      drag_window;
    int32_t  drag_offset_x, drag_offset_y;
    int32_t  cursor_x, cursor_y;
    ContextMenu context_menu;
};

void init(int32_t screen_w, int32_t screen_h);
DesktopState* get_state();

int  create_window(const char* title, int32_t x, int32_t y, int32_t w, int32_t h, const char* app_type);
void close_window(int id);
void minimize_window(int id);
void maximize_window(int id);
void restore_window(int id);
void set_active_window(int id);
void set_window_text(int id, const char* text);
void append_window_text(int id, const char* text);
void clear_window_text(int id);
Window* get_window(int id);
void bring_to_front(int id);

void render_desktop();
void render_taskbar();
void render_window(Window* win);
void render_start_menu();
void render_context_menu();
void render_cursor(int32_t x, int32_t y);
void render_all();

void open_context_menu(int32_t x, int32_t y, const char* context);
void close_context_menu();

int  hit_test_window(int32_t x, int32_t y);
int  hit_test_close_btn(int id, int32_t x, int32_t y);
int  hit_test_maximize_btn(int id, int32_t x, int32_t y);
int  hit_test_minimize_btn(int id, int32_t x, int32_t y);

} // namespace desktop

/* ============================================================
 * EMBEDDED ASSETS — gambar yang di-embed ke kernel binary
 * ============================================================ */
namespace assets {

/* Mascot 96x96 ARGB */
static const int MASCOT_W = 96;
static const int MASCOT_H = 96;

/* Wallpaper 640x400 ARGB */
static const int WALLPAPER_W = 640;
static const int WALLPAPER_H = 400;

/* Symbols dari objcopy (extern "C" karena dari binary blob) */
extern "C" {
    extern const uint8_t _binary_kernel_assets_mascot_bin_start[];
    extern const uint8_t _binary_kernel_assets_mascot_bin_end[];
    extern const uint8_t _binary_kernel_assets_wallpaper_bin_start[];
    extern const uint8_t _binary_kernel_assets_wallpaper_bin_end[];
    extern const uint8_t _binary_kernel_assets_menu_icon_bin_start[];
    extern const uint8_t _binary_kernel_assets_menu_icon_bin_end[];
}

inline const uint32_t* mascot_pixels()    { return (const uint32_t*)_binary_kernel_assets_mascot_bin_start; }
inline const uint32_t* wallpaper_pixels() { return (const uint32_t*)_binary_kernel_assets_wallpaper_bin_start; }

static const int MENU_ICON_W = 32;
static const int MENU_ICON_H = 32;
inline const uint32_t* menu_icon_pixels() { return (const uint32_t*)_binary_kernel_assets_menu_icon_bin_start; }

} // namespace assets

} // namespace hal
