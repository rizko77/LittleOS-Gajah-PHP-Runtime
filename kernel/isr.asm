default rel
section .text

global isr_common_stub
extern isr_handler

isr_common_stub:
    ; save general registers
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rsi
    push rdi
    push rbp
    push rdx
    push rcx
    push rbx
    push rax

    ; arg1 = pointer ke stack (register state)
    mov rdi, rsp

    call isr_handler

    ; restore registers (reverse order)
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rbp
    pop rdi
    pop rsi
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    add rsp, 16     ; skip error_code + interrupt_number

    iretq


; %macro ISR_NOERR 1
; global isr%1
; isr%1:
;     push 0          ; dummy error code
;     push %1         ; interrupt number
;     jmp isr_common_stub
; %endmacro
;
; %macro ISR_ERR 1
; global isr%1
; isr%1:
;     push %1         ; interrupt number
;     jmp isr_common_stub
; %endmacro

; Macro 256
%macro MAKE_ISR 1
global isr%1
isr%1:
%if (%1 = 8) || (%1 = 10) || (%1 = 11) || (%1 = 12) || (%1 = 13) || (%1 = 14) || (%1 = 17) || (%1 = 21) || (%1 = 29) || (%1 = 30)
    push %1
%else
    push 0
    push %1
%endif
    jmp isr_common_stub
%endmacro

%assign i 0
%rep 256
    MAKE_ISR i
%assign i i+1
%endrep

; Export ISR Table
global isr_table
isr_table:
%assign i 0
%rep 256
    dq isr %+ i
%assign i i+1
%endrep
