[org 0x1000]; 可用内存中选了一个地址

dw 0x55aa; 魔数， 用于判断错误

; 打印字符串
mov si, loading
call print

; 阻塞
jmp $

print:
    mov ah, 0x0e
.next:
    mov al, [si]
    cmp al, 0
    jz .done
    int 0x10
    inc si
    jmp .next
.done:
    ret

loading:
    db "Loading znix...", 10, 13, 0;    \n\r

error:
    mov si, .msg
    call print
    hlt; 让 CPU 停止
    jmp $
    .msg db "Loading Error!!!", 10, 13, 0
