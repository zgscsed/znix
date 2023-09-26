#include <znix/znix.h>
#include <znix/io.h>
#include <znix/string.h>
#include <znix/console.h>
#include <znix/stdarg.h>
#include <znix/printk.h>
#include <znix/global.h>
#include <znix/task.h>
#include <znix/debug.h>
#include <znix/interrupt.h>

void kernel_init()
{
    console_init();
    gdt_init();
    interrupt_init();
    task_init();

    // asm volatile(
    //     "sti\n"
    //     "movl %eax, %eax\n"
    // );

    // asm volatile("sti\n");

    // u32 counter = 0;
    // while (true)
    // {
    //     DEBUGK("looping in kernel init %d...\n", counter++);
    //     delay(100000000);
    // }
    


    return;
}