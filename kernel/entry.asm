default rel
section .text
global _start
extern __bss_start
extern __bss_end
extern kmain

_start:

  lea rsp, [stack_top]
  and rsp, -16
  sub rsp, 8

  lea rdi, [__bss_start]
  lea rcx, [__bss_end]
  sub rcx, rdi

  test rcx, rcx
  jz .bss_done

  xor rax, rax

  mov rdx, rcx
  shr rcx, 3
  rep stosq

  mov rcx, rdx
  and rcx, 7
  rep stosb

.bss_done:
  
  push rbx
  mov eax, 1
  cpuid
  ; EDX bit 25 check (SSE)
  test edx, (1 << 0)
  jz hang
  test edx, (1 << 25)
  jz hang
  test edx, (1 << 26)
  jz hang
  pop rbx
  ; AVX (CPUID.1:ECX.AVX => bit 28)
  ; AVX OSXSAVE Check
  ; test ecx, (1 << 28) ;AVX
  ; jz hang
  ; test ecx, (1 << 26) ;OSXSAVE
  ; jz hang

  ; Streaming SIMD Extension (SSE)
  ; enable CR0
  mov rax, cr0
  and rax, ~(1 << 2)  ; reset EM2, disable emulation
  or rax, (1 << 1)    ; set Monitor Coprocessor    (bit 1)
  or rax, (1 << 5)    ;NE (Native Exception)
  mov cr0, rax

  ;enable CR4
  mov rax, cr4  
  ; OSFXSR      set (bit 9) ; OSXMMEXCPT  set (bit 10)  ; OSXSAVE
  or rax, (1 << 9) | (1 << 10) ;| (1 << 18)
  mov cr4, rax

  ; TSS
  ; lea rax, [stack_top]
  ; mov qword [tss64 + 4], rax ;rsp0
  ; ; Encode TSS descriptor
  ; lea rax, [tss64]
  ; mov rcx, TSS64_STACK_SIZE ; 103 = 104-1
  ; ;low qword
  ; mov rdx, rcx ;limit
  ;
  ; ; RDX = limit, RAX = base TSS
  ; mov rbx, rax ; copy base addr
  ; and rbx, 0xFFFFFF
  ; shl rbx, 16
  ; ; rdx & 0xFFFF
  ; mov rcx, rdx
  ; and rcx, 0xFFFF
  ; or rbx, rcx
  ; ; access byte: rbx | (0x89 << 40)
  ; mov rcx, 0x89
  ; shl rcx, 40
  ; or rbx, rcx
  ; ; lim hi 4 bit: ((limit >> 16) & 0xF) << 48
  ; mov rcx, rdx
  ; shr rcx, 16
  ; and rcx, 0xF
  ; shl rcx, 48
  ; or rbx, rcx
  ; ;((base TSS >> 24) & 0xFF) << 56 ; base 24..31
  ; mov rcx, rax
  ; shr rcx, 24
  ; and rcx, 0xFF
  ; shl rcx, 56
  ; or rbx, rcx
  ; mov [tss_desc], rbx
  ; ;high qword
  ; mov rax, rax
  ; shr rax, 32
  ; mov [tss_desc+8], rax
  ;
  ; lgdt [gdt_ptr]
  ; mov ax, TSS_SELECTOR
  ; ltr ax

  clts ; clear Task Switch -> enable SSE
  fninit ; Init FPU
  ldmxcsr [mxcsr_default] ; Init MXCSR

  ; XMM + YMM
  ; mov ecx, 0
  ; mov eax, 0x3 ;XMM | YMM
  ; xor edx, edx
  ; xsetbv

  ; mov rdi, 0xb8000  ; VGA text mode base
  ; mov byte [rdi], 'H'
  ; mov byte [rdi+1], 0x0F

  ; push 0x08 ;Code64 selector
  ; lea rax, [rel after_jump]
  ; push rax
  ; iretq

  call kmain

hang:
  hlt
  jmp hang

section .bss
align 16
stack_bottom:
  resb 16384
stack_top:

; align 8
; tss64:
;   resb 104 ;104 bytes for tss64

section .data
align 16
mxcsr_default:
  dd 0x1F80

; GDT, TSS
; align 8
; tss64_rsp0_offset equ 4 ;rsp0 offset ref
;
; gdt64:
;   dq 0
;   dq 0
;
; ;at 0x08
; code64_low:
;   dw 0x0000 ;limit 0-15
;   dw 0x0000 ;base 0-15
;   db 0x00 ;base 16-23
;   db 10011010b ;access: present=1 ring0=0, executable=1,readable=1,accessed=0
;   db 00100000b ;flags=64-bit code, limit hi=0
;   db 0x00 ; base 24-31
;   dq 0 ;high qword, yet unused rn
; ;at 0x10
; data64_low:
;   dw 0x0000 ;lim
;   dw 0x0000 ;base
;   dw 0x00 ;base hi
;   db 10010010b ;access: present=1, ring0=0, writable=1
;   db 00000000b ;flags
;   db 0x00 ;base hi
;   dq 0 ;high qword
;
; tss_desc:
;   dq 0 ;desc low qword
;   dq 0 ;desc hi qword
;
; gdt_ptr:
;   dw gdt64_end - gdt64 - 1 ;lim
;   dq gdt64 ;base
; gdt64_end:
;
; TSS_SELECTOR equ 0x18 ; TSS descriptor (GDT index 3) 3 * 8 = 0x18
; TSS64_STACK_SIZE equ 103
