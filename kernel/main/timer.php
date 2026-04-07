<?php
/**
 * LittleOS Gajah PHP 8.2 — timer.php
 * Modul timer PHP
 * 
 * Menggunakan built-in: uptime(), uptime_ms(), sleep()
 * 
 * @package  LittleOS\Timer
 */

// ============================================================
// TIMER FUNCTIONS PHP
// ============================================================

/**
 * Format uptime ke string yang mudah dibaca
 */
function format_uptime() {
    $sec = uptime();
    $hr  = $sec / 3600;
    $min = ($sec % 3600) / 60;
    $s   = $sec % 60;
    return zero_pad($hr) . ":" . zero_pad($min) . ":" . zero_pad($s);
}

/**
 * Dapatkan timestamp saat ini (ms sejak boot)
 */
function timestamp() {
    return uptime_ms();
}
