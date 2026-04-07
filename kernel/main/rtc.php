<?php
/**
 * LittleOS Gajah PHP 8.2 — rtc.php
 * Modul Real-Time Clock PHP
 * 
 * Menggunakan built-in: rtc_hour(), rtc_minute(), rtc_second(),
 *   rtc_day(), rtc_month(), rtc_year()
 * 
 * @package  LittleOS\RTC
 */

// ============================================================
// RTC FUNCTIONS PHP
// ============================================================

/**
 * Dapatkan waktu sekarang sebagai string HH:MM:SS
 */
function get_time_string() {
    $h = rtc_hour();
    $m = rtc_minute();
    $s = rtc_second();
    return zero_pad($h) . ":" . zero_pad($m) . ":" . zero_pad($s);
}

/**
 * Dapatkan tanggal sekarang sebagai string DD/MM/YYYY
 */
function get_date_string() {
    $d = rtc_day();
    $m = rtc_month();
    $y = rtc_year();
    return zero_pad($d) . "/" . zero_pad($m) . "/" . $y;
}

/**
 * Dapatkan nama hari
 */
function get_day_name($weekday) {
    $days = ["Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"];
    if ($weekday >= 0 && $weekday < 7) {
        return $days[$weekday];
    }
    return "???";
}

/**
 * Dapatkan nama bulan
 */
function get_month_name($month) {
    $months = ["", "Januari", "Februari", "Maret", "April", "Mei", "Juni",
               "Juli", "Agustus", "September", "Oktober", "November", "Desember"];
    if ($month >= 1 && $month <= 12) {
        return $months[$month];
    }
    return "???";
}
