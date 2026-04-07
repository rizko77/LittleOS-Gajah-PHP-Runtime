<?php
/**
 * LittleOS Gajah PHP 8.2 — string.php
 * Modul string utilities PHP
 * 
 * Fungsi bawaan: strlen, substr, strtoupper, strtolower,
 *   str_repeat, str_contains, strval, intval
 * 
 * @package  LittleOS\String
 */

// ============================================================
// EXTENDED STRING FUNCTIONS
// ============================================================

/**
 * Pad string ke kiri
 */
function str_pad_left($str, $len, $pad) {
    while (strlen($str) < $len) {
        $str = $pad . $str;
    }
    return $str;
}

/**
 * Pad string ke kanan
 */
function str_pad_right($str, $len, $pad) {
    while (strlen($str) < $len) {
        $str = $str . $pad;
    }
    return $str;
}

/**
 * Format angka 2 digit (untuk clock)
 */
function zero_pad($num) {
    if ($num < 10) {
        return "0" . $num;
    }
    return strval($num);
}

/**
 * Trim leading/trailing spaces
 */
function trim_str($str) {
    $start = 0;
    $end = strlen($str) - 1;
    while ($start <= $end && substr($str, $start, 1) == " ") {
        $start = $start + 1;
    }
    while ($end >= $start && substr($str, $end, 1) == " ") {
        $end = $end - 1;
    }
    return substr($str, $start, $end - $start + 1);
}
