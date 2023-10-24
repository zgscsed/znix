[bits 32]

extern console_init
extern gdt_init
extern memory_init
extern kernel_init

global _start
_start:
    ; mov byte [0xb8000], 'K'; 表示进入了内核
    ; call kernel_init
    ; xchg bx, bx
    ; int 0x80;    调用 0x80 中断函数 系统调用

    push ebx;  ards_count
    push eax;  magic

    call console_init    ; 控制台初始化
    call gdt_init        ; 全局描述符初始化     
    call memory_init     ; 内存初始化
    call kernel_init     ; 内核初始化

    xchg bx, bx

    mov eax, 0; 0 号系统调用
    int 0x80;

    jmp $; 阻塞