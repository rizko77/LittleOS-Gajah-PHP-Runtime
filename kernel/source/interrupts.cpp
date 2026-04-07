/*
 * LittleOS Gajah PHP - interrupts.cpp
 * IDT, PIC, dan penanganan interupsi (C++)
 * Timer IRQ 0, Keyboard IRQ 1, Mouse IRQ 12
 */

#include "hal.hpp"

/* Referensi external untuk mouse packet handler */
extern "C" void mouse_handle_packet(uint8_t data);

namespace hal {

/* ============================================================
 * STRUKTUR IDT x86_64
 * ============================================================ */
struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} PACKED;

struct IDTR {
    uint16_t limit;
    uint64_t base;
} PACKED;

static IDTEntry idt_entries[256];
static IDTR idtr;

/* ============================================================
 * PIC (Programmable Interrupt Controller)
 * ============================================================ */
#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1
#define PIC_EOI     0x20
#define ICW1_INIT   0x11
#define ICW4_8086   0x01

static void pic_remap() {
    /* Simpan mask */
    uint8_t mask1 = ports::inb(PIC1_DATA);
    uint8_t mask2 = ports::inb(PIC2_DATA);

    /* Mulai inisialisasi PIC */
    ports::outb(PIC1_CMD, ICW1_INIT); ports::io_wait();
    ports::outb(PIC2_CMD, ICW1_INIT); ports::io_wait();

    /* Set offset: IRQ 0-7 -> INT 32-39, IRQ 8-15 -> INT 40-47 */
    ports::outb(PIC1_DATA, 0x20); ports::io_wait();
    ports::outb(PIC2_DATA, 0x28); ports::io_wait();

    /* Hubungkan PIC1 dan PIC2 */
    ports::outb(PIC1_DATA, 0x04); ports::io_wait();
    ports::outb(PIC2_DATA, 0x02); ports::io_wait();

    /* Mode 8086 */
    ports::outb(PIC1_DATA, ICW4_8086); ports::io_wait();
    ports::outb(PIC2_DATA, ICW4_8086); ports::io_wait();

    /* Restore mask — aktifkan timer (IRQ0) dan keyboard (IRQ1) */
    UNUSED(mask1);
    UNUSED(mask2);
    ports::outb(PIC1_DATA, 0xF8); /* 11111000 = aktifkan IRQ 0, 1, 2 (cascade) */
    ports::outb(PIC2_DATA, 0xEF); /* 11101111 = aktifkan IRQ 12 (mouse) */
}

static void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        ports::outb(PIC2_CMD, PIC_EOI);
    }
    ports::outb(PIC1_CMD, PIC_EOI);
}

/* ============================================================
 * TIMER STATE
 * ============================================================ */
static volatile uint64_t timer_ticks = 0;

/* ============================================================
 * KEYBOARD STATE
 * ============================================================ */
#define KB_BUFFER_SIZE 256
static volatile char    kb_buffer[KB_BUFFER_SIZE];
static volatile uint32_t kb_head = 0;
static volatile uint32_t kb_tail = 0;
static volatile uint8_t  kb_modifiers = 0;
#define KB_MOD_SHIFT    (1 << 0)
#define KB_MOD_CTRL     (1 << 1)
#define KB_MOD_CAPS     (1 << 2)

/* Scancode ke ASCII (US layout) — lowercase */
static const char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' ', 0, 0,0,0,0,0,0,0,0,0,0, 0,0,
    0,0,0,'-', 0,0,0,'+', 0,0,0,0,0, 0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/* Scancode ke ASCII — uppercase (shift) */
static const char scancode_to_ascii_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0, 'A','S','D','F','G','H','J','K','L',':','"','~',
    0, '|','Z','X','C','V','B','N','M','<','>','?', 0,
    '*', 0, ' ', 0, 0,0,0,0,0,0,0,0,0,0, 0,0,
    0,0,0,'-', 0,0,0,'+', 0,0,0,0,0, 0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static void kb_buffer_push(char c) {
    uint32_t next = (kb_head + 1) % KB_BUFFER_SIZE;
    if (next != kb_tail) {
        kb_buffer[kb_head] = c;
        kb_head = next;
    }
}

/* ============================================================
 * INTERRUPT FRAME (di-push oleh CPU pada x86_64)
 * ============================================================ */
struct InterruptFrame {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

/* ============================================================
 * ISR HANDLERS
 * ============================================================ */
__attribute__((interrupt))
static void timer_isr(InterruptFrame* frame) {
    UNUSED(frame);
    timer_ticks++;
    pic_send_eoi(0);
}

__attribute__((interrupt))
static void keyboard_isr(InterruptFrame* frame) {
    UNUSED(frame);
    uint8_t scancode = ports::inb(0x60);

    /* Key release (bit 7 set) */
    if (scancode & 0x80) {
        uint8_t release = scancode & 0x7F;
        if (release == 0x2A || release == 0x36) { /* Shift release */
            kb_modifiers &= ~KB_MOD_SHIFT;
        }
        if (release == 0x1D) { /* Ctrl release */
            kb_modifiers &= ~KB_MOD_CTRL;
        }
    } else {
        /* Key press */
        if (scancode == 0x2A || scancode == 0x36) { /* Shift press */
            kb_modifiers |= KB_MOD_SHIFT;
        } else if (scancode == 0x1D) { /* Ctrl press */
            kb_modifiers |= KB_MOD_CTRL;
        } else if (scancode == 0x3A) { /* Caps Lock toggle */
            kb_modifiers ^= KB_MOD_CAPS;
        } else {
            char c = 0;
            bool use_shift = (kb_modifiers & KB_MOD_SHIFT) != 0;
            bool use_caps  = (kb_modifiers & KB_MOD_CAPS) != 0;

            if (use_shift) {
                c = scancode_to_ascii_shift[scancode];
            } else {
                c = scancode_to_ascii[scancode];
            }

            /* Caps Lock hanya mempengaruhi huruf */
            if (use_caps && !use_shift && c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            } else if (use_caps && use_shift && c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a';
            }

            if (c != 0) {
                kb_buffer_push(c);
            }
        }
    }

    pic_send_eoi(1);
}

/* Dummy handler untuk interupsi yang tidak ditangani */
__attribute__((interrupt))
static void default_isr(InterruptFrame* frame) {
    UNUSED(frame);
}

__attribute__((interrupt))
static void default_isr_err(InterruptFrame* frame, uint64_t error_code) {
    UNUSED(frame);
    UNUSED(error_code);
}

/* Mouse ISR (IRQ 12 = vektor 44) */
__attribute__((interrupt))
static void mouse_isr(InterruptFrame* frame) {
    UNUSED(frame);
    uint8_t data = ports::inb(0x60);
    mouse_handle_packet(data);
    pic_send_eoi(12);
}

/* Variabel scancode buffer untuk GUI mode */
static volatile uint8_t scancode_buffer[64];
static volatile uint32_t sc_head = 0;
static volatile uint32_t sc_tail = 0;

/* ============================================================
 * IDT SETUP
 * ============================================================ */
static void idt_set_entry(uint8_t vector, void* handler, uint8_t type_attr) {
    uint64_t addr = (uint64_t)handler;
    idt_entries[vector].offset_low  = (uint16_t)(addr & 0xFFFF);
    idt_entries[vector].selector    = 0x28; /* Limine sets GDT entry 5 (0x28) as 64-bit code segment */
    idt_entries[vector].ist         = 0;
    idt_entries[vector].type_attr   = type_attr;
    idt_entries[vector].offset_mid  = (uint16_t)((addr >> 16) & 0xFFFF);
    idt_entries[vector].offset_high = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
    idt_entries[vector].reserved    = 0;
}

namespace interrupts {

void init() {
    /* Inisialisasi semua entry IDT dengan default handler */
    for (int i = 0; i < 256; i++) {
        /* Exception 8, 10-14, 17, 21, 29, 30 have error codes */
        bool has_error = (i == 8 || (i >= 10 && i <= 14) || i == 17 || i == 21 || i == 29 || i == 30);
        if (has_error) {
            idt_set_entry(i, (void*)default_isr_err, 0x8E);
        } else {
            idt_set_entry(i, (void*)default_isr, 0x8E);
        }
    }

    /* Remap PIC */
    pic_remap();

    /* Set handler timer (IRQ 0 = vektor 32) */
    idt_set_entry(32, (void*)timer_isr, 0x8E);

    /* Set handler keyboard (IRQ 1 = vektor 33) */
    idt_set_entry(33, (void*)keyboard_isr, 0x8E);

    /* Set handler mouse (IRQ 12 = vektor 44) */
    idt_set_entry(44, (void*)mouse_isr, 0x8E);

    /* Load IDT */
    idtr.limit = sizeof(idt_entries) - 1;
    idtr.base  = (uint64_t)&idt_entries;
    asm volatile("lidt %0" : : "m"(idtr));
}

void enable() {
    asm volatile("sti");
}

void disable() {
    asm volatile("cli");
}

} // namespace interrupts

/* ============================================================
 * TIMER IMPLEMENTATION
 * ============================================================ */
#define PIT_FREQ     1193182
#define TARGET_FREQ  1000 /* 1000 Hz = 1ms per tick */
#define PIT_CH0_DATA 0x40
#define PIT_CMD      0x43

namespace timer {

void init() {
    uint16_t divisor = (uint16_t)(PIT_FREQ / TARGET_FREQ);

    ports::outb(PIT_CMD, 0x36); /* Channel 0, lobyte/hibyte, square wave */
    ports::outb(PIT_CH0_DATA, (uint8_t)(divisor & 0xFF));
    ports::outb(PIT_CH0_DATA, (uint8_t)((divisor >> 8) & 0xFF));
}

uint64_t get_ticks()   { return timer_ticks; }
uint64_t get_ms()      { return timer_ticks; } /* 1 tick = 1ms */
uint64_t get_seconds() { return timer_ticks / 1000; }

void wait_ms(uint64_t ms) {
    uint64_t target = timer_ticks + ms;
    while (timer_ticks < target) {
        asm volatile("hlt");
    }
}

} // namespace timer

/* ============================================================
 * KEYBOARD IMPLEMENTATION
 * ============================================================ */
namespace keyboard {

void init() {
    /* Keyboard diinisialisasi melalui PIC IRQ 1 */
    /* Flush buffer keyboard hardware */
    while (ports::inb(0x64) & 0x01) {
        ports::inb(0x60);
    }
}

char read_char() {
    while (kb_head == kb_tail) {
        asm volatile("hlt");
    }
    char c = kb_buffer[kb_tail];
    kb_tail = (kb_tail + 1) % KB_BUFFER_SIZE;
    return c;
}

char read_char_nonblocking() {
    if (kb_head == kb_tail) return 0;
    char c = kb_buffer[kb_tail];
    kb_tail = (kb_tail + 1) % KB_BUFFER_SIZE;
    return c;
}

bool has_input() {
    return kb_head != kb_tail;
}

void read_line(char* buf, size_t max_len) {
    size_t pos = 0;
    while (pos < max_len - 1) {
        char c = read_char();
        if (c == '\n') {
            hal::console::putchar('\n');
            break;
        }
        if (c == '\b') {
            if (pos > 0) {
                pos--;
                hal::console::putchar('\b');
            }
            continue;
        }
        buf[pos++] = c;
        hal::console::putchar(c);
    }
    buf[pos] = '\0';
}

uint8_t read_scancode_nonblocking() {
    if (sc_head == sc_tail) return 0;
    uint8_t sc = scancode_buffer[sc_tail];
    sc_tail = (sc_tail + 1) % 64;
    return sc;
}

bool is_shift_held() { return (kb_modifiers & KB_MOD_SHIFT) != 0; }
bool is_ctrl_held()  { return (kb_modifiers & KB_MOD_CTRL) != 0; }

} // namespace keyboard

} // namespace hal
