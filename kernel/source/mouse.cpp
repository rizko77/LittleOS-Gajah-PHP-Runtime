/*
 * LittleOS Gajah PHP - mouse.cpp
 * PS/2 Mouse driver — C++ HAL
 * Digunakan oleh desktop GUI PHP
 */

#include "hal.hpp"

namespace hal {
namespace mouse {

/* State mouse */
static struct {
    int32_t x, y;
    int32_t screen_w, screen_h;
    bool left, right, middle;
    bool prev_left, prev_right;
    /* Packet buffer */
    uint8_t packet[4];
    int     packet_idx;
    bool    has_wheel;
    /* Event queue */
    bool event_pending;
} mstate;

/* Tunggu mouse controller siap untuk ditulis */
static void mouse_wait_write() {
    int timeout = 100000;
    while (timeout--) {
        if (!(hal::ports::inb(0x64) & 0x02)) return;
    }
}

/* Tunggu mouse controller siap untuk dibaca */
static void mouse_wait_read() {
    int timeout = 100000;
    while (timeout--) {
        if (hal::ports::inb(0x64) & 0x01) return;
    }
}

/* Kirim command ke mouse */
static void mouse_write(uint8_t cmd) {
    mouse_wait_write();
    hal::ports::outb(0x64, 0xD4);
    mouse_wait_write();
    hal::ports::outb(0x60, cmd);
}

/* Baca data dari mouse */
static uint8_t mouse_read() {
    mouse_wait_read();
    return hal::ports::inb(0x60);
}

void init() {
    /* Set resolusi layar - default, akan di-update oleh desktop::init */
    mstate.x = 0;
    mstate.y = 0;
    mstate.screen_w = 1024;
    mstate.screen_h = 768;
    mstate.left = mstate.right = mstate.middle = false;
    mstate.prev_left = mstate.prev_right = false;
    mstate.packet_idx = 0;
    mstate.has_wheel = false;
    mstate.event_pending = false;

    /* Enable auxiliary mouse device */
    mouse_wait_write();
    hal::ports::outb(0x64, 0xA8);

    /* Enable interrupts */
    mouse_wait_write();
    hal::ports::outb(0x64, 0x20);
    mouse_wait_read();
    uint8_t status = hal::ports::inb(0x60);
    status |= 0x02;  /* set bit 1: enable IRQ12 */
    status &= ~0x20; /* clear bit 5: enable mouse clock */
    mouse_wait_write();
    hal::ports::outb(0x64, 0x60);
    mouse_wait_write();
    hal::ports::outb(0x60, status);

    /* Set default settings */
    mouse_write(0xF6);
    mouse_read(); /* ACK */

    /* Enable data reporting */
    mouse_write(0xF4);
    mouse_read(); /* ACK */

    /* Set resolusi dari console */
    mstate.screen_w = (int32_t)hal::console::get_fb_width();
    mstate.screen_h = (int32_t)hal::console::get_fb_height();
    mstate.x = mstate.screen_w / 2;
    mstate.y = mstate.screen_h / 2;
}

/* Dipanggil dari ISR (IRQ12) */
extern "C" void mouse_handle_packet(uint8_t data) {
    mstate.packet[mstate.packet_idx] = data;
    mstate.packet_idx++;

    int packet_size = mstate.has_wheel ? 4 : 3;

    if (mstate.packet_idx >= packet_size) {
        mstate.packet_idx = 0;

        /* Validasi: bit 3 dari byte 0 harus selalu set */
        if (!(mstate.packet[0] & 0x08)) return;

        /* Decode packet */
        mstate.prev_left = mstate.left;
        mstate.prev_right = mstate.right;

        mstate.left   = (mstate.packet[0] & 0x01) != 0;
        mstate.right  = (mstate.packet[0] & 0x02) != 0;
        mstate.middle = (mstate.packet[0] & 0x04) != 0;

        int32_t dx = (int32_t)mstate.packet[1];
        int32_t dy = (int32_t)mstate.packet[2];

        /* Sign extend */
        if (mstate.packet[0] & 0x10) dx |= 0xFFFFFF00;
        if (mstate.packet[0] & 0x20) dy |= 0xFFFFFF00;

        /* Overflow check */
        if (mstate.packet[0] & 0x40) dx = 0;
        if (mstate.packet[0] & 0x80) dy = 0;

        /* Update posisi (dy terbalik di PS/2) */
        mstate.x += dx;
        mstate.y -= dy;

        /* Clamp ke layar */
        if (mstate.x < 0) mstate.x = 0;
        if (mstate.y < 0) mstate.y = 0;
        if (mstate.x >= mstate.screen_w) mstate.x = mstate.screen_w - 1;
        if (mstate.y >= mstate.screen_h) mstate.y = mstate.screen_h - 1;

        mstate.event_pending = true;
    }
}

int32_t get_x() { return mstate.x; }
int32_t get_y() { return mstate.y; }
bool is_left_pressed() { return mstate.left; }
bool is_right_pressed() { return mstate.right; }

bool has_event() { return mstate.event_pending; }

MouseEvent poll_event() {
    MouseEvent ev;
    ev.x = mstate.x;
    ev.y = mstate.y;
    ev.dx = 0;
    ev.dy = 0;
    ev.left = mstate.left;
    ev.right = mstate.right;
    ev.middle = mstate.middle;
    ev.clicked = (mstate.left && !mstate.prev_left);
    ev.released = (!mstate.left && mstate.prev_left);
    ev.right_clicked = (mstate.right && !mstate.prev_right);
    mstate.event_pending = false;
    return ev;
}

} // namespace mouse
} // namespace hal
