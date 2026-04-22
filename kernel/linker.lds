/* Tell the linker that we want an x86_64 ELF64 output file */
OUTPUT_FORMAT(elf64-x86-64)

/* Entry point kernel adalah fungsi kmain */
ENTRY(_start)

/* Definisi program headers */
PHDRS
{
    text PT_LOAD FLAGS(5);  /* R + X - readable, writable, executable */
    data PT_LOAD FLAGS(6);  /* R + W */
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

    . = ALIGN(4096);
    .text : { *(.text .text.*) } :text

    . = ALIGN(4096);
    .rodata : { *(.rodata .rodata.*) } :text

    . = ALIGN(4096);
    .data : { *(.data .data.*) } :data
    
    . = ALIGN(4096);
    .bss : { 
      __bss_start = .;
      *(.bss .bss.*)
      *(COMMON)
      __bss_end = .;
      } :data

    /* Discard unused sections */
    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }
}
