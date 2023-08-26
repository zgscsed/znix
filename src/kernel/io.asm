[bits 32]

section .text; 代码段

global inb ; 将inb 导出
inb:
    push ebp;
    mov ebp, esp ; 保存帧

    xor eax, eax ; 将eax清空
    mov edx, [ebp + 8]; port
    in al, dx; 将端口 dx的8bit输入到ax

    jmp $+2 ; 一点延迟
    jmp $+2 ; 一点延迟
    jmp $+2 ; 一点延迟

    leave ; 恢复栈帧
    ret

global outb ; 将inb 导出
outb:
    push ebp;
    mov ebp, esp ; 保存帧

    mov edx, [ebp + 8]; port
    mov eax, [ebp + 12]; value
    out dx, al; 将 al 中的8bit 输出

    jmp $+2 ; 一点延迟
    jmp $+2 ; 一点延迟
    jmp $+2 ; 一点延迟

    leave ; 恢复栈帧
    ret

global inw ; 将inb 导出
inw:
    push ebp;
    mov ebp, esp ; 保存帧

    xor eax, eax ; 将eax清空
    mov edx, [ebp + 8]; port
    in ax, dx; 将端口 dx的8bit输入到ax

    jmp $+2 ; 一点延迟
    jmp $+2 ; 一点延迟
    jmp $+2 ; 一点延迟

    leave ; 恢复栈帧
    ret

global outw ; 将inb 导出
outw:
    push ebp;
    mov ebp, esp ; 保存帧

    mov edx, [ebp + 8]; port
    mov eax, [ebp + 12]; value
    out dx, ax; 将 al 中的8bit 输出

    jmp $+2 ; 一点延迟
    jmp $+2 ; 一点延迟
    jmp $+2 ; 一点延迟

    leave ; 恢复栈帧
    ret

; global inb ; 将inb 导出
; inb:
;     push ebp;
;     mov ebp, esp ; 保存帧

;     leave ; 恢复栈帧
;     ret