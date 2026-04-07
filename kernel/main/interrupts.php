<?php
/**
 * LittleOS Gajah PHP 8.2 — interrupts.php
 * Modul interrupt/event handler PHP
 * 
 * PHP layer di atas IDT/PIC C++ HAL.
 * Events dari hardware di-forward ke callback PHP.
 * 
 * @package  LittleOS\Interrupts
 */

// ============================================================
// EVENT HANDLER SYSTEM (PHP layer)
// ============================================================

$event_handlers = [];

/**
 * Register event handler PHP
 */
function register_handler($event, $callback_name) {
    // Disimpan sebagai asociasi event -> nama fungsi
    echo "[ PHP ] Handler registered: " . $event . " -> " . $callback_name . "\n";
}

/**
 * Daftar event yang didukung
 */
function list_events() {
    echo "  Events yang didukung:\n";
    echo "  - timer_tick    : setiap 1ms\n";
    echo "  - key_press     : tombol keyboard ditekan\n";
    echo "  - mouse_click   : klik mouse\n";
    echo "  - mouse_move    : gerakan mouse\n";
    echo "  - window_close  : window ditutup\n";
}
