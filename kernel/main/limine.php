<?php
/**
 * LittleOS Gajah PHP 8.2 — limine.php
 * Modul Limine bootloader protocol PHP
 * 
 * Informasi tentang bootloader yang digunakan.
 * Definisi protocol ada di C++ (limine.h).
 * 
 * @package  LittleOS\Limine
 */

// ============================================================
// LIMINE BOOTLOADER INFO
// ============================================================

$LIMINE_VERSION = "8.x";
$LIMINE_PROTOCOL = "Limine Boot Protocol";

/**
 * Tampilkan info bootloader
 */
function bootloader_info() {
    echo "  Bootloader: Limine v8.x\n";
    echo "  Protocol : Limine Boot Protocol\n";
    echo "  Features : Framebuffer, Memory Map, HHDM\n";
    echo "  PHP Note : Bootloader adalah satu-satunya\n";
    echo "             bagian non-PHP di LittleOS.\n";
    echo "             Setelah boot, kernel PHP mengambil alih.\n";
}
