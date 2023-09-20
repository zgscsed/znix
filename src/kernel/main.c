#include <znix/znix.h>
#include <znix/io.h>
#include <znix/string.h>
#include <znix/console.h>
#include <znix/stdarg.h>
#include <znix/printk.h>
#include <znix/assert.h>

void kernel_init()
{
    console_init();
    assert(3 > 6);
    printk("cahn%dtest2%3c\n", 67, 's');
    printk("cahn");
    return;
}