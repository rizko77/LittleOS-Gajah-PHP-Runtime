/*
 * LittleOS Gajah PHP - console.cpp
 * Framebuffer console + grafis primitif + double buffering
 * Tema Tailwind CSS — ditulis dalam C++
 */

#include "hal.hpp"
#include <stdarg.h>

namespace hal {
namespace console {

/* ============================================================
 * FONT BITMAP 8x8 — karakter ASCII 32-126
 * Font klasik CP437 untuk tampilan console
 * ============================================================ */
static const uint8_t font8x8[95][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /*   (32) */
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, /* ! (33) */
    {0x36,0x36,0x14,0x00,0x00,0x00,0x00,0x00}, /* " (34) */
    {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00}, /* # (35) */
    {0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00}, /* $ (36) */
    {0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00}, /* % (37) */
    {0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00}, /* & (38) */
    {0x06,0x06,0x03,0x00,0x00,0x00,0x00,0x00}, /* ' (39) */
    {0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0x00}, /* ( (40) */
    {0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0x00}, /* ) (41) */
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, /* * (42) */
    {0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00}, /* + (43) */
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x06}, /* , (44) */
    {0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00}, /* - (45) */
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00}, /* . (46) */
    {0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00}, /* / (47) */
    {0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00}, /* 0 (48) */
    {0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00}, /* 1 (49) */
    {0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00}, /* 2 (50) */
    {0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00}, /* 3 (51) */
    {0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00}, /* 4 (52) */
    {0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00}, /* 5 (53) */
    {0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00}, /* 6 (54) */
    {0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00}, /* 7 (55) */
    {0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00}, /* 8 (56) */
    {0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00}, /* 9 (57) */
    {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x00}, /* : (58) */
    {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x06}, /* ; (59) */
    {0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0x00}, /* < (60) */
    {0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00}, /* = (61) */
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, /* > (62) */
    {0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00}, /* ? (63) */
    {0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00}, /* @ (64) */
    {0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00}, /* A (65) */
    {0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00}, /* B (66) */
    {0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00}, /* C (67) */
    {0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00}, /* D (68) */
    {0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00}, /* E (69) */
    {0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00}, /* F (70) */
    {0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00}, /* G (71) */
    {0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00}, /* H (72) */
    {0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, /* I (73) */
    {0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00}, /* J (74) */
    {0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0x00}, /* K (75) */
    {0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00}, /* L (76) */
    {0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00}, /* M (77) */
    {0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00}, /* N (78) */
    {0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00}, /* O (79) */
    {0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00}, /* P (80) */
    {0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00}, /* Q (81) */
    {0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00}, /* R (82) */
    {0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0x00}, /* S (83) */
    {0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, /* T (84) */
    {0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00}, /* U (85) */
    {0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00}, /* V (86) */
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, /* W (87) */
    {0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00}, /* X (88) */
    {0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00}, /* Y (89) */
    {0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00}, /* Z (90) */
    {0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00}, /* [ (91) */
    {0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00}, /* \ (92) */
    {0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00}, /* ] (93) */
    {0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00}, /* ^ (94) */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, /* _ (95) */
    {0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00}, /* ` (96) */
    {0x00,0x00,0x1E,0x30,0x3E,0x33,0x6E,0x00}, /* a (97) */
    {0x07,0x06,0x06,0x3E,0x66,0x66,0x3B,0x00}, /* b (98) */
    {0x00,0x00,0x1E,0x33,0x03,0x33,0x1E,0x00}, /* c (99) */
    {0x38,0x30,0x30,0x3E,0x33,0x33,0x6E,0x00}, /* d (100) */
    {0x00,0x00,0x1E,0x33,0x3F,0x03,0x1E,0x00}, /* e (101) */
    {0x1C,0x36,0x06,0x0F,0x06,0x06,0x0F,0x00}, /* f (102) */
    {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x1F}, /* g (103) */
    {0x07,0x06,0x36,0x6E,0x66,0x66,0x67,0x00}, /* h (104) */
    {0x0C,0x00,0x0E,0x0C,0x0C,0x0C,0x1E,0x00}, /* i (105) */
    {0x30,0x00,0x30,0x30,0x30,0x33,0x33,0x1E}, /* j (106) */
    {0x07,0x06,0x66,0x36,0x1E,0x36,0x67,0x00}, /* k (107) */
    {0x0E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00}, /* l (108) */
    {0x00,0x00,0x33,0x7F,0x7F,0x6B,0x63,0x00}, /* m (109) */
    {0x00,0x00,0x1F,0x33,0x33,0x33,0x33,0x00}, /* n (110) */
    {0x00,0x00,0x1E,0x33,0x33,0x33,0x1E,0x00}, /* o (111) */
    {0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F}, /* p (112) */
    {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78}, /* q (113) */
    {0x00,0x00,0x3B,0x6E,0x66,0x06,0x0F,0x00}, /* r (114) */
    {0x00,0x00,0x3E,0x03,0x1E,0x30,0x1F,0x00}, /* s (115) */
    {0x08,0x0C,0x3E,0x0C,0x0C,0x2C,0x18,0x00}, /* t (116) */
    {0x00,0x00,0x33,0x33,0x33,0x33,0x6E,0x00}, /* u (117) */
    {0x00,0x00,0x33,0x33,0x33,0x1E,0x0C,0x00}, /* v (118) */
    {0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00}, /* w (119) */
    {0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00}, /* x (120) */
    {0x00,0x00,0x33,0x33,0x33,0x3E,0x30,0x1F}, /* y (121) */
    {0x00,0x00,0x3F,0x19,0x0C,0x26,0x3F,0x00}, /* z (122) */
    {0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0x00}, /* { (123) */
    {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, /* | (124) */
    {0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0x00}, /* } (125) */
    {0x6E,0x3B,0x00,0x00,0x00,0x00,0x00,0x00}, /* ~ (126) */
};

/* ============================================================
 * STATE CONSOLE
 * ============================================================ */
static struct {
    uint32_t* framebuffer;
    uint32_t* backbuffer;
    uint32_t* render_target; /* Drawing target: framebuffer or backbuffer */
    uint64_t  fb_width;
    uint64_t  fb_height;
    uint64_t  fb_pitch;
    uint32_t  fg_color;
    uint32_t  bg_color;
    uint32_t  cursor_col;
    uint32_t  cursor_row;
    uint32_t  max_cols;
    uint32_t  max_rows;
    uint64_t  fb_size_bytes;
} state;

static const int FONT_W = 8;
static const int FONT_H = 8;
static const int CHAR_SPACING = 1;
static const int LINE_SPACING = 2;
static const int CELL_W = FONT_W + CHAR_SPACING;
static const int CELL_H = FONT_H + LINE_SPACING;

/* ============================================================
 * IMPLEMENTASI GRAFIS DASAR
 * ============================================================ */
void draw_pixel(int32_t x, int32_t y, uint32_t color) {
    if (x < 0 || y < 0 || (uint64_t)x >= state.fb_width || (uint64_t)y >= state.fb_height) return;
    uint32_t* target = state.render_target ? state.render_target : state.framebuffer;
    uint32_t* pixel = (uint32_t*)((uint8_t*)target + y * state.fb_pitch + x * 4);
    *pixel = color;
}

void fill_rect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    uint32_t* target = state.render_target ? state.render_target : state.framebuffer;
    for (int32_t row = y; row < y + h; row++) {
        if (row < 0 || (uint64_t)row >= state.fb_height) continue;
        for (int32_t col = x; col < x + w; col++) {
            if (col < 0 || (uint64_t)col >= state.fb_width) continue;
            uint32_t* pixel = (uint32_t*)((uint8_t*)target + row * state.fb_pitch + col * 4);
            *pixel = color;
        }
    }
}

void draw_char(int32_t x, int32_t y, char c, uint32_t fg, uint32_t bg) {
    int idx = (int)c - 32;
    if (idx < 0 || idx >= 95) idx = 0; /* karakter tidak dikenal -> spasi */
    const uint8_t* glyph = font8x8[idx];

    for (int row = 0; row < FONT_H; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < FONT_W; col++) {
            uint32_t color = (bits & (1 << col)) ? fg : bg;
            draw_pixel(x + col, y + row, color);
        }
    }
}

/* ============================================================
 * IMPLEMENTASI CONSOLE
 * ============================================================ */
void init(void* fb_addr, uint64_t width, uint64_t height, uint64_t pitch) {
    state.framebuffer = (uint32_t*)fb_addr;
    state.fb_width    = width;
    state.fb_height   = height;
    state.fb_pitch    = pitch;
    state.fg_color    = COLOR_FG;
    state.bg_color    = COLOR_BG;
    state.cursor_col  = 0;
    state.cursor_row  = 0;
    state.max_cols    = (uint32_t)(width / CELL_W);
    state.max_rows    = (uint32_t)(height / CELL_H);
    state.fb_size_bytes = height * pitch;
    state.backbuffer    = nullptr;
    state.render_target = nullptr; /* Default: render langsung ke framebuffer */
}

void clear() {
    fill_rect(0, 0, (int32_t)state.fb_width, (int32_t)state.fb_height, state.bg_color);
    state.cursor_col = 0;
    state.cursor_row = 0;
}

void scroll_up() {
    /* Salin semua baris ke atas 1 baris */
    uint64_t line_bytes = CELL_H * state.fb_pitch;
    uint8_t* fb = (uint8_t*)state.framebuffer;

    for (uint32_t row = 1; row < state.max_rows; row++) {
        uint8_t* dst = fb + (row - 1) * line_bytes;
        uint8_t* src = fb + row * line_bytes;
        memcpy(dst, src, line_bytes);
    }

    /* Bersihkan baris paling bawah */
    uint8_t* last_line = fb + (state.max_rows - 1) * line_bytes;
    for (uint64_t i = 0; i < line_bytes / 4; i++) {
        ((uint32_t*)last_line)[i] = state.bg_color;
    }
}

void newline() {
    state.cursor_col = 0;
    state.cursor_row++;
    if (state.cursor_row >= state.max_rows) {
        scroll_up();
        state.cursor_row = state.max_rows - 1;
    }
}

void putchar(char c) {
    if (c == '\n') {
        newline();
        return;
    }
    if (c == '\r') {
        state.cursor_col = 0;
        return;
    }
    if (c == '\t') {
        uint32_t next_tab = (state.cursor_col + 4) & ~3u;
        while (state.cursor_col < next_tab && state.cursor_col < state.max_cols) {
            draw_char(state.cursor_col * CELL_W, state.cursor_row * CELL_H, ' ',
                      state.fg_color, state.bg_color);
            state.cursor_col++;
        }
        if (state.cursor_col >= state.max_cols) newline();
        return;
    }
    if (c == '\b') {
        if (state.cursor_col > 0) {
            state.cursor_col--;
            draw_char(state.cursor_col * CELL_W, state.cursor_row * CELL_H, ' ',
                      state.fg_color, state.bg_color);
        }
        return;
    }

    if (state.cursor_col >= state.max_cols) {
        newline();
    }

    draw_char(state.cursor_col * CELL_W, state.cursor_row * CELL_H, c,
              state.fg_color, state.bg_color);
    state.cursor_col++;
}

void puts(const char* s) {
    if (!s) return;
    while (*s) {
        putchar(*s);
        s++;
    }
}

void puts_colored(const char* s, uint32_t color) {
    uint32_t old_fg = state.fg_color;
    state.fg_color = color;
    puts(s);
    state.fg_color = old_fg;
}

void put_number(int64_t n) {
    char buf[32];
    hal::string::itoa(n, buf);
    puts(buf);
}

void put_hex(uint64_t n) {
    char buf[32];
    hal::string::htoa(n, buf);
    puts("0x");
    puts(buf);
}

void set_fg(uint32_t color) { state.fg_color = color; }
void set_bg(uint32_t color) { state.bg_color = color; }

void set_cursor(uint32_t col, uint32_t row) {
    state.cursor_col = col;
    state.cursor_row = row;
}

uint32_t get_width()     { return state.max_cols; }
uint32_t get_height()    { return state.max_rows; }
uint64_t get_fb_width()  { return state.fb_width; }
uint64_t get_fb_height() { return state.fb_height; }

/* ============================================================
 * PRINTF SEDERHANA
 * ============================================================ */
void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's': {
                    const char* s = va_arg(args, const char*);
                    puts(s ? s : "(null)");
                    break;
                }
                case 'd': {
                    int64_t n = va_arg(args, int64_t);
                    put_number(n);
                    break;
                }
                case 'u': {
                    uint64_t n = va_arg(args, uint64_t);
                    char buf[32];
                    hal::string::utoa(n, buf);
                    puts(buf);
                    break;
                }
                case 'x': {
                    uint64_t n = va_arg(args, uint64_t);
                    put_hex(n);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    putchar(c);
                    break;
                }
                case '%': {
                    putchar('%');
                    break;
                }
                default:
                    putchar('%');
                    putchar(*fmt);
                    break;
            }
        } else {
            putchar(*fmt);
        }
        fmt++;
    }

    va_end(args);
}

/* ============================================================
 * GRAFIS PRIMITIF TAMBAHAN
 * ============================================================ */
void draw_rect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    fill_rect(x, y, w, 1, color);         /* top */
    fill_rect(x, y + h - 1, w, 1, color); /* bottom */
    fill_rect(x, y, 1, h, color);         /* left */
    fill_rect(x + w - 1, y, 1, h, color); /* right */
}

void draw_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    int32_t sx = x0 < x1 ? 1 : -1;
    int32_t sy = y0 < y1 ? 1 : -1;
    int32_t err = dx - dy;
    while (true) {
        draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int32_t e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

void fill_circle(int32_t cx, int32_t cy, int32_t r, uint32_t color) {
    for (int32_t dy = -r; dy <= r; dy++) {
        for (int32_t dx = -r; dx <= r; dx++) {
            if (dx * dx + dy * dy <= r * r) {
                draw_pixel(cx + dx, cy + dy, color);
            }
        }
    }
}

void fill_rounded_rect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
    if (r <= 0) { fill_rect(x, y, w, h, color); return; }
    if (r > h / 2) r = h / 2;
    if (r > w / 2) r = w / 2;
    /* Isi tengah */
    fill_rect(x + r, y, w - 2 * r, h, color);
    fill_rect(x, y + r, r, h - 2 * r, color);
    fill_rect(x + w - r, y + r, r, h - 2 * r, color);
    /* Sudut bulat */
    for (int32_t dy = -r; dy <= 0; dy++) {
        for (int32_t dx = -r; dx <= 0; dx++) {
            if (dx * dx + dy * dy <= r * r) {
                draw_pixel(x + r + dx, y + r + dy, color);     /* kiri atas */
                draw_pixel(x + w - 1 - r - dx, y + r + dy, color); /* kanan atas */
                draw_pixel(x + r + dx, y + h - 1 - r - dy, color); /* kiri bawah */
                draw_pixel(x + w - 1 - r - dx, y + h - 1 - r - dy, color); /* kanan bawah */
            }
        }
    }
}

void draw_char_transparent(int32_t x, int32_t y, char c, uint32_t fg) {
    int idx = (int)c - 32;
    if (idx < 0 || idx >= 95) idx = 0;
    const uint8_t* glyph = font8x8[idx];
    for (int row = 0; row < FONT_H; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < FONT_W; col++) {
            if (bits & (1 << col)) {
                draw_pixel(x + col, y + row, fg);
            }
        }
    }
}

void draw_string(int32_t x, int32_t y, const char* s, uint32_t fg) {
    if (!s) return;
    int32_t cx = x;
    while (*s) {
        if (*s == '\n') { y += CELL_H; cx = x; s++; continue; }
        draw_char_transparent(cx, y, *s, fg);
        cx += CELL_W;
        s++;
    }
}

void draw_string_bg(int32_t x, int32_t y, const char* s, uint32_t fg, uint32_t bg) {
    if (!s) return;
    int32_t cx = x;
    while (*s) {
        if (*s == '\n') { y += CELL_H; cx = x; s++; continue; }
        draw_char(cx, y, *s, fg, bg);
        cx += CELL_W;
        s++;
    }
}

int string_pixel_width(const char* s) {
    if (!s) return 0;
    int w = 0;
    while (*s) { w += CELL_W; s++; }
    return w;
}

/* Double buffering — render ke backbuffer, lalu flip ke framebuffer */
void swap_buffers() {
    if (!state.backbuffer) {
        state.backbuffer = (uint32_t*)hal::memory::kmalloc(state.fb_size_bytes);
        if (!state.backbuffer) return;
    }
    /* Copy backbuffer ke layar */
    memcpy(state.framebuffer, state.backbuffer, state.fb_size_bytes);
}

void begin_frame() {
    if (!state.backbuffer) {
        state.backbuffer = (uint32_t*)hal::memory::kmalloc(state.fb_size_bytes);
    }
    if (state.backbuffer) {
        state.render_target = state.backbuffer;
    }
}

void end_frame() {
    if (state.backbuffer) {
        memcpy(state.framebuffer, state.backbuffer, state.fb_size_bytes);
    }
    state.render_target = nullptr; /* Kembali ke framebuffer langsung */
}

uint32_t* get_backbuffer() {
    if (!state.backbuffer) {
        state.backbuffer = (uint32_t*)hal::memory::kmalloc(state.fb_size_bytes);
    }
    return state.backbuffer;
}

void* get_framebuffer() { return state.framebuffer; }
uint64_t get_pitch() { return state.fb_pitch; }

/* Blit raw ARGB pixel data ke layar */
void blit_raw(int32_t dx, int32_t dy, int32_t sw, int32_t sh, const uint32_t* pixels) {
    if (!pixels) return;
    uint32_t* target = state.render_target ? state.render_target : state.framebuffer;
    for (int32_t row = 0; row < sh; row++) {
        int32_t sy = dy + row;
        if (sy < 0 || (uint64_t)sy >= state.fb_height) continue;
        for (int32_t col = 0; col < sw; col++) {
            int32_t sx = dx + col;
            if (sx < 0 || (uint64_t)sx >= state.fb_width) continue;
            uint32_t c = pixels[row * sw + col];
            /* Skip fully transparent pixels */
            if ((c >> 24) == 0 && (c & 0x00FFFFFF) == 0) continue;
            uint32_t* pixel = (uint32_t*)((uint8_t*)target + sy * state.fb_pitch + sx * 4);
            *pixel = c;
        }
    }
}

} // namespace console
} // namespace hal
