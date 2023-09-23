#include <znix/znix.h>
#include <znix/io.h>
#include <znix/string.h>
#include <znix/console.h>
#include <znix/stdarg.h>
#include <znix/printk.h>
#include <znix/global.h>
#include <znix/task.h>
#include <znix/interrupt.h>

void kernel_init()
{
    console_init();
    gdt_init();

    // printk("test1:%d\n", 48);
    // for (int i = 0; i< 100; ++i)
    // {
    //     printk("test1:%d\n", i);
    // }
    // task_init();

    interrupt_init();
    return;
}