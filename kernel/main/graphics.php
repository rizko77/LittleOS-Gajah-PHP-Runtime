<?php
/**
 * LittleOS Gajah PHP 8.2 — graphics.php
 * Modul grafis PHP — drawing primitives via built-in
 * 
 * Menggunakan built-in: draw_pixel(), draw_rect(), fill_rect(),
 *   draw_line(), fill_circle(), fill_rounded_rect(), draw_string()
 * 
 * Tema: Tailwind CSS color palette
 * 
 * @package  LittleOS\Graphics
 */

// ============================================================
// TAILWIND CSS COLOR PALETTE (PHP constants)
// ============================================================

$TW = [
    "slate-50"  => 0xFFF8FAFC, "slate-100" => 0xFFF1F5F9,
    "slate-200" => 0xFFE2E8F0, "slate-300" => 0xFFCBD5E1,
    "slate-400" => 0xFF94A3B8, "slate-500" => 0xFF64748B,
    "slate-600" => 0xFF475569, "slate-700" => 0xFF334155,
    "slate-800" => 0xFF1E293B, "slate-900" => 0xFF0F172A,
    "sky-400"   => 0xFF38BDF8, "sky-500"   => 0xFF0EA5E9,
    "blue-500"  => 0xFF3B82F6, "blue-600"  => 0xFF2563EB,
    "green-500" => 0xFF22C55E, "red-500"   => 0xFFEF4444,
    "amber-500" => 0xFFF59E0B, "purple-500" => 0xFFA855F7,
    "white"     => 0xFFFFFFFF, "black"     => 0xFF000000
];

// ============================================================
// GRAFIS HELPER PHP
// ============================================================

/**
 * Gambar button dengan tema Tailwind rounded
 */
function draw_button($x, $y, $w, $h, $text, $bg_color, $text_color) {
    fill_rounded_rect($x, $y, $w, $h, 4, $bg_color);
    $tw = strlen($text) * 9;
    $tx = $x + ($w - $tw) / 2;
    $ty = $y + ($h - 8) / 2;
    draw_text($tx, $ty, $text, $text_color);
}

/**
 * Gambar panel / card Tailwind
 */
function draw_panel($x, $y, $w, $h, $bg_color) {
    fill_rounded_rect($x, $y, $w, $h, 8, $bg_color);
}

/**
 * Gambar icon sederhana (karakter besar)
 */
function draw_icon($x, $y, $char, $color) {
    draw_text($x, $y, $char, $color);
}
