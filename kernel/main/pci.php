<?php
/**
 * LittleOS Gajah PHP 8.2 — pci.php
 * Modul PCI bus PHP (informasi dan enumerasi)
 * 
 * @package  LittleOS\PCI
 */

// ============================================================
// PCI CLASS CODES
// ============================================================
$PCI_CLASSES = [
    0x00 => "Unclassified",
    0x01 => "Mass Storage",
    0x02 => "Network",
    0x03 => "Display",
    0x04 => "Multimedia",
    0x05 => "Memory",
    0x06 => "Bridge",
    0x07 => "Communication",
    0x08 => "System Peripheral",
    0x09 => "Input Device",
    0x0A => "Docking Station",
    0x0B => "Processor",
    0x0C => "Serial Bus",
    0x0D => "Wireless",
    0x0E => "Intelligent I/O",
    0x0F => "Satellite",
    0x10 => "Encryption",
    0x11 => "Signal Processing"
];

/**
 * Describe PCI class code
 */
function pci_class_name($class_code) {
    // Simplified — bergantung pada PCI HAL
    return "PCI Device (class " . $class_code . ")";
}
