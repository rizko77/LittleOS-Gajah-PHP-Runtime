<?php
/**
 * LittleOS Gajah PHP 8.2 — mouse.php
 * Modul mouse driver PHP
 * 
 * Menggunakan built-in: mouse_x(), mouse_y(), mouse_clicked(),
 *   mouse_event()
 * 
 * @package  LittleOS\Mouse
 */

// ============================================================
// MOUSE HELPER FUNCTIONS
// ============================================================

/**
 * Cek apakah posisi mouse di dalam rect
 */
function mouse_in_rect($mx, $my, $x, $y, $w, $h) {
    return ($mx >= $x && $mx < $x + $w && $my >= $y && $my < $y + $h);
}

/**
 * Cek apakah mouse diklik di dalam area
 */
function clicked_in($x, $y, $w, $h) {
    $mx = mouse_x();
    $my = mouse_y();
    if (mouse_clicked()) {
        return mouse_in_rect($mx, $my, $x, $y, $w, $h);
    }
    return false;
}
