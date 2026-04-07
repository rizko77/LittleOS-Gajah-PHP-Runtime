<?php
/**
 * LittleOS Gajah PHP 8.2 — memory.php
 * Modul manajemen memori PHP Runtime
 * 
 * Menggunakan built-in: memory_total(), memory_free(), memory_used()
 * 
 * @package  LittleOS\Memory
 */

// ============================================================
// FUNGSI MEMORI PHP
// ============================================================

/**
 * Tampilkan status memori sistem
 */
function show_memory_status() {
    $total = memory_total();
    $free  = memory_free();
    $used  = memory_used();
    
    echo "  <?php echo memory_get_usage(); ?>\n";
    echo "  ==============================\n";
    echo "  Total : " . format_bytes($total) . "\n";
    echo "  Free  : " . format_bytes($free) . "\n";
    echo "  Used  : " . format_bytes($used) . "\n";
    
    if ($total > 0) {
        $pct = ($used * 100) / $total;
        echo "  Usage : " . $pct . "%\n";
    }
    echo "  ==============================\n";
}

/**
 * Format memori untuk Task Manager
 */
function memory_bar($width) {
    $total = memory_total();
    $used  = memory_used();
    if ($total <= 0) return "[???]";
    
    $pct = ($used * $width) / $total;
    $bar = "[";
    for ($i = 0; $i < $width; $i++) {
        if ($i < $pct) {
            $bar = $bar . "#";
        } else {
            $bar = $bar . "-";
        }
    }
    $bar = $bar . "]";
    return $bar;
}
