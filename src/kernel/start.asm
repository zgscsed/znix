[bits 32]

extern kernel_init

global _start
_start:
    ; mov byte [0xb8000], 'K'; 表示进入了内核
    call kernel_init
    ; xchg bx, bx
    ; int 0x80;    调用 0x80 中断函数 系统调用
    jmp $; 阻塞