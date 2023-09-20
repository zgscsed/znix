#include <znix/znix.h>
#include <znix/io.h>
#include <znix/string.h>
#include <znix/console.h>
#include <znix/stdarg.h>
#include <znix/printk.h>

void kernel_init()
{
    console_init();
    printk("cahn%dtest2%3c\n", 67, 's');
    printk("cahn");
    return;
}