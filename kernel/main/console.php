<?php
/**
 * LittleOS Gajah PHP 8.2 — console.php
 * Modul console PHP — output teks, warna, dan format
 * 
 * Menggunakan built-in functions dari C++ HAL:
 *   echo, console_clear(), console_set_color(), 
 *   console_cols(), console_rows()
 * 
 * @package  LittleOS\Console
 */

// ============================================================
// WARNA TAILWIND CSS (untuk digunakan dari PHP)
// ============================================================
$TW_SLATE_100 = 0xFFF1F5F9;
$TW_SLATE_400 = 0xFF94A3B8;
$TW_SLATE_700 = 0xFF334155;
$TW_SKY_500   = 0xFF0EA5E9;
$TW_BLUE_500  = 0xFF3B82F6;
$TW_GREEN_500 = 0xFF22C55E;
$TW_RED_500   = 0xFFEF4444;
$TW_AMBER_500 = 0xFFF59E0B;
$TW_WHITE     = 0xFFFFFFFF;

// ============================================================
// FUNGSI CONSOLE PHP
// ============================================================

/**
 * Print berwarna — khas PHP Runtime OS
 */
function cprint($text, $color) {
    console_set_color($color);
    echo $text;
    console_set_color(0xFFF1F5F9);
}

/**
 * Print baris baru dengan warna
 */
function cprintln($text, $color) {
    cprint($text . "\n", $color);
}

/**
 * Print dengan format tag PHP sebagai badge
 */
function php_badge($label, $value) {
    cprint("  <?= ", 0xFF94A3B8);
    cprint($label, 0xFF0EA5E9);
    cprint(" ?>  ", 0xFF94A3B8);
    echo $value . "\n";
}

/**
 * Print header box dengan tema PHP
 */
function php_header($title) {
    $len = strlen($title) + 16;
    $line = str_repeat("=", $len);
    echo "\n";
    cprint("  <?php // " . $line . " ?>\n", 0xFF94A3B8);
    cprint("  <?php echo '", 0xFF94A3B8);
    cprint($title, 0xFF0EA5E9);
    cprint("'; ?>\n", 0xFF94A3B8);
    cprint("  <?php // " . $line . " ?>\n", 0xFF94A3B8);
    echo "\n";
}

/**
 * Print separator
 */
function php_separator() {
    cprint("  // " . str_repeat("-", 40) . "\n", 0xFF334155);
}
