<?php
/**
 * LittleOS Gajah PHP 8.2 — ports.php
 * Modul port I/O PHP (wrapper)
 * 
 * Port I/O sebenarnya ditangani oleh C++ HAL.
 * File ini berisi dokumentasi dan helper PHP.
 * 
 * @package  LittleOS\Ports
 */

// ============================================================
// PORT DOCUMENTATION
// ============================================================

/**
 * Daftar port yang digunakan oleh LittleOS:
 * - 0x20-0x21 : PIC1 (master)
 * - 0x40-0x43 : PIT Timer  
 * - 0x60      : PS/2 Keyboard/Mouse data
 * - 0x64      : PS/2 Controller status/command
 * - 0x70-0x71 : CMOS RTC
 * - 0xA0-0xA1 : PIC2 (slave)
 * - 0xCF8-0xCFF : PCI Configuration Space
 * 
 * Semua port diakses melalui C++ HAL,
 * PHP hanya mendefinisikan konstanta.
 */

$PORT_PIC1_CMD  = 0x20;
$PORT_PIC1_DATA = 0x21;
$PORT_PIT_CH0   = 0x40;
$PORT_PIT_CMD   = 0x43;
$PORT_PS2_DATA  = 0x60;
$PORT_PS2_CMD   = 0x64;
$PORT_CMOS_ADDR = 0x70;
$PORT_CMOS_DATA = 0x71;
$PORT_PIC2_CMD  = 0xA0;
$PORT_PIC2_DATA = 0xA1;
