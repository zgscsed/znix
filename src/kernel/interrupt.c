#include <znix/interrupt.h>
#include <znix/global.h>
#include <znix/debug.h>
#include <znix/printk.h>

#define ENTRY_SIZE 0x20

static gate_t idt[IDT_SIZE];
static pointer_t idt_ptr;

handler_t handler_table[IDT_SIZE];
extern handler_t handler_entry_table[ENTRY_SIZE];

// 中断函数输出打印
static char *messages[] = {
    "#DE Divide Error\0",
    "#DB RESERVED\0",
    "--  NMI Interrupt\0",
    "#BP Breakpoint\0",
    "#OF Overflow\0",
    "#BR BOUND Range Exceeded\0",
    "#UD Invalid Opcode (Undefined Opcode)\0",
    "#NM Device Not Available (No Math Coprocessor)\0",
    "#DF Double Fault\0",
    "    Coprocessor Segment Overrun (reserved)\0",
    "#TS Invalid TSS\0",
    "#NP Segment Not Present\0",
    "#SS Stack-Segment Fault\0",
    "#GP General Protection\0",
    "#PF Page Fault\0",
    "--  (Intel reserved. Do not use.)\0",
    "#MF x87 FPU Floating-Point Error (Math Fault)\0",
    "#AC Alignment Check\0",
    "#MC Machine Check\0",
    "#XF SIMD Floating-Point Exception\0",
    "#VE Virtualization Exception\0",
    "#CP Control Protection Exception\0",
};

// 中断函数
extern void exception_handler(int vector)
{
    char *message = NULL;
    if (vector < 22)     // 22种异常
    {
        message = messages[vector];
    }
    else
    {
        message = messages[15];
    }

    printk("Exception : [0x%02X] %s \n", vector, messages[vector]);
    // 阻塞
    while (true)
    {
        ;
    }
}

void interrupt_init()
{
    for (size_t i = 0; i < ENTRY_SIZE; ++i)
    {
        gate_t *gate = &idt[i];
        handler_t handler = handler_entry_table[i];
        gate->offset0 = (u32)handler & 0xffff;        // 16位
        gate->offset1 = ((u32)handler >> 16) & 0xffff;
        gate->selector = 1 << 3;                                // 代码段
        gate->reserved = 0;                                     // 保留不用
        gate->type = 0b1110;                                    // 中断门
        gate->segment = 0;                                      // 系统段
        gate->DPL = 0;                                          // 内核态
        gate->present = 1;                                      // 有效
    }

    for (size_t i = 0; i < ENTRY_SIZE; ++i)
    {
        handler_table[i] = exception_handler;                   // 中断函数中执行的异常处理函数
    }

    idt_ptr.base = (u32)idt;
    idt_ptr.limit = sizeof(idt) - 1;

    BMB;
    asm volatile("lidt idt_ptr");
}