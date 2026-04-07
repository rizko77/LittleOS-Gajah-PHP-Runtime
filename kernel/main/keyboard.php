<?php
/**
 * LittleOS Gajah PHP 8.2 — keyboard.php
 * Modul keyboard PHP
 * 
 * Menggunakan built-in: readline(), keyboard functions
 * 
 * @package  LittleOS\Keyboard
 */

// ============================================================
// KEYBOARD INPUT PHP
// ============================================================

/**
 * Baca satu baris input dari user (blocking)
 */
function input($prompt) {
    echo $prompt;
    return readline();
}

/**
 * Baca satu karakter (non-blocking, untuk GUI)
 */
function get_key() {
    return keyboard_read();
}

/**
 * Cek apakah ada input keyboard tersedia
 */
function has_key() {
    return keyboard_has_input();
}
