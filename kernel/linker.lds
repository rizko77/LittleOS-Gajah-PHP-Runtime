/* Tell the linker that we want an x86_64 ELF64 output file */
OUTPUT_FORMAT(elf64-x86-64)

/* Entry point kernel adalah fungsi kmain */
ENTRY(kmain)

/* Definisi program headers */
PHDRS
{
    text PT_LOAD FLAGS(7);  /* RWX - readable, writable, executable */
}

SECTIONS
{
    /* Semua sections di higher half */
    . = 0xffffffff80000000;

    /* Limine requests section */
    .limine_requests : {
        KEEP(*(.limine_requests_start))
        KEEP(*(.limine_requests))
        KEEP(*(.limine_requests_end))
    } :text

    /* Text section */
    . = ALIGN(4096);
    .text : {
        *(.text .text.*)
    } :text

    /* Read-only data */
    . = ALIGN(4096);
    .rodata : {
        *(.rodata .rodata.*)
    } :text

    /* Data */
    . = ALIGN(4096);
    .data : {
        *(.data .data.*)
    } :text

    /* BSS */
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :text

    /* Discard unused sections */
    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }
}
