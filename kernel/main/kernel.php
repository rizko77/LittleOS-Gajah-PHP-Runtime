<?php
/**
 * LittleOS Gajah PHP 8.2 — kernel.php
 * Modul inti kernel PHP Runtime
 * 
 * Ini BUKAN UNIX. BUKAN Linux. BUKAN DOS.
 * Kernel murni PHP 8 yang berjalan di bare metal x86_64.
 * 
 * @package  LittleOS\Kernel
 * @version  8.2.0
 */

// ============================================================
// KONSTANTA KERNEL
// ============================================================
$KERNEL_NAME    = "LittleOS Gajah PHP";
$KERNEL_VERSION = "8.2.0";
$KERNEL_ARCH    = "x86_64";
$KERNEL_TYPE    = "PHP Runtime (bare metal)";

// ============================================================
// FUNGSI KERNEL INTI
// ============================================================

function kernel_boot_info() {
    echo "[ PHP ] Kernel PHP 8.2 Runtime aktif\n";
    echo "[ PHP ] " . kernel_name() . " v" . kernel_version() . "\n";
    echo "[ PHP ] Arsitektur: " . kernel_arch() . "\n";
    echo "[ PHP ] Resolusi: " . screen_width() . "x" . screen_height() . "\n";
}

function format_bytes($bytes) {
    if ($bytes >= 1073741824) {
        return ($bytes / 1073741824) . " GB";
    }
    if ($bytes >= 1048576) {
        return ($bytes / 1048576) . " MB";
    }
    if ($bytes >= 1024) {
        return ($bytes / 1024) . " KB";
    }
    return $bytes . " B";
}

function php_sysinfo() {
    echo "\n";
    echo "  <?php phpinfo(); ?>\n";
    echo "  ======================================\n";
    echo "  OS      : " . kernel_name() . "\n";
    echo "  Version : " . kernel_version() . "\n";
    echo "  Arch    : " . kernel_arch() . "\n";
    echo "  Kernel  : PHP 8.2 Runtime\n";
    echo "  Memory  : " . format_bytes(memory_total()) . "\n";
    echo "  Free    : " . format_bytes(memory_free()) . "\n";
    echo "  Uptime  : " . uptime() . " detik\n";
    echo "  Screen  : " . screen_width() . "x" . screen_height() . "\n";
    echo "  ======================================\n";
    echo "\n";
}
