/*
 * LittleOS Gajah PHP - hal.hpp
 * Hardware Abstraction Layer (C++ untuk komunikasi hardware)
 * Semua akses hardware dilakukan melalui C++ — bukan C
 *
 * Versi Desktop: mendukung GUI, mouse, window manager
 * Tema warna: Tailwind CSS color palette
 */

#pragma once

#include "limine.h"
#include <stddef.h>
#include <stdint.h>

namespace hal {

/* ============================================================
 * PORT I/O — akses langsung ke port hardware x86
 * ============================================================ */
namespace ports {

static inline __attribute__((always_inline)) uint8_t inb(uint16_t port) {
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

static inline void io_wait() { asm volatile("outb %%al, $0x80" : : "a"(0)); }

} // namespace ports

/* ============================================================
 * STRING — fungsi manipulasi string
 * ============================================================ */
namespace string {

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strcat(char *dst, const char *src);
char *strchr(const char *s, int c);
char *strstr(const char *hay, const char *needle);
void itoa(int64_t num, char *buf);
void utoa(uint64_t num, char *buf);
void htoa(uint64_t num, char *buf);
int64_t atoi(const char *s);
char to_upper(char c);
char to_lower(char c);
bool is_digit(char c);
bool is_alpha(char c);
bool is_space(char c);
bool is_alnum(char c);

} // namespace string

/* ============================================================
 * MEMORY — manajemen memori fisik dan heap
 * ============================================================ */
namespace memory {

void init(struct limine_memmap_response *memmap, uint64_t hhdm_offset);
void *alloc_page();
void free_page(void *addr);
uint64_t get_total();
uint64_t get_free();
uint64_t get_used();

/* Heap dinamis */
void heap_init();
void *kmalloc(size_t size);
void kfree(void *ptr);
void *kcalloc(size_t count, size_t size);
void *krealloc(void *ptr, size_t new_size);

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

inline uint64_t rdtscp();
void init();
uint64_t get_ticks();
uint64_t get_ms();
uint64_t get_seconds();
uint64_t time_ns();
void wait_ms(uint64_t ms);
void calibrate_tsc();

} // namespace timer

/* ============================================================
 * KEYBOARD — driver keyboard PS/2
 * ============================================================ */
namespace keyboard {

void init();
char read_char();
char read_char_nonblocking();
bool has_input();
void read_line(char *buf, size_t max_len);
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
static const int BORDER_W = 1;

enum class WindowState : uint8_t { Normal, Minimized, Maximized, Closed };

struct Window {
  int32_t id;
  char title[128];
  int32_t x, y, w, h;
  int32_t saved_x, saved_y, saved_w, saved_h;
  WindowState state;
  bool active;
  bool dirty;
  char text_content[8192];
  int text_len;
  int text_scroll;
  uint32_t bg_color;
  uint32_t title_color;
  char app_type[64];
};

/* Context menu item */
static const int MAX_CTX_ITEMS = 12;
struct ContextMenuItem {
  char label[64];
  char action[64];
  bool separator; /* true = garis pemisah, bukan item */
};

struct ContextMenu {
  bool visible;
  int32_t x, y;
  int item_count;
  ContextMenuItem items[MAX_CTX_ITEMS];
};

struct DesktopState {
  Window windows[MAX_WINDOWS];
  int window_count;
  int active_window;
  int z_order[MAX_WINDOWS];
  int z_count;
  bool running;
  bool needs_redraw;
  int32_t screen_w, screen_h;
  bool start_menu_open;
  bool dragging;
  int drag_window;
  int32_t drag_offset_x, drag_offset_y;
  int32_t cursor_x, cursor_y;
  ContextMenu context_menu;
};

void init(int32_t screen_w, int32_t screen_h);
DesktopState &get_state();

int create_window(const char *title, int32_t x, int32_t y, int32_t w, int32_t h,
                  const char *app_type);
void close_window(int id);
void minimize_window(int id);
void maximize_window(int id);
void restore_window(int id);
void set_active_window(int id);
void set_window_text(int id, const char *text);
void append_window_text(int id, const char *text);
void clear_window_text(int id);
Window *get_window(int id);
void bring_to_front(int id);

void render_desktop();
void render_taskbar();
void render_window(Window *win);
void render_start_menu();
void render_context_menu();
void render_cursor(int32_t x, int32_t y);
void render_all();

void open_context_menu(int32_t x, int32_t y, const char *context);
void close_context_menu();

int hit_test_window(int32_t x, int32_t y);
int hit_test_close_btn(int id, int32_t x, int32_t y);
int hit_test_maximize_btn(int id, int32_t x, int32_t y);
int hit_test_minimize_btn(int id, int32_t x, int32_t y);

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

extern "C" const uint8_t _binary_kernel_assets_mascot_bin_start[];
extern "C" const uint8_t _binary_kernel_assets_mascot_bin_end[];
extern "C" const uint8_t _binary_kernel_assets_wallpaper_bin_start[];
extern "C" const uint8_t _binary_kernel_assets_wallpaper_bin_end[];
extern "C" const uint8_t _binary_kernel_assets_menu_icon_bin_start[];
extern "C" const uint8_t _binary_kernel_assets_menu_icon_bin_end[];

inline const uint32_t *mascot_pixels() {
  return (const uint32_t *)_binary_kernel_assets_mascot_bin_start;
}
inline const uint32_t *wallpaper_pixels() {
  return (const uint32_t *)_binary_kernel_assets_wallpaper_bin_start;
}

static const int MENU_ICON_W = 32;
static const int MENU_ICON_H = 32;
inline const uint32_t *menu_icon_pixels() {
  return (const uint32_t *)_binary_kernel_assets_menu_icon_bin_start;
}

} // namespace assets

} // namespace hal
