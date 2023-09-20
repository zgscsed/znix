#include <znix/znix.h>
#include <znix/io.h>
#include <znix/string.h>
#include <znix/console.h>
#include <znix/stdarg.h>
#include <znix/printk.h>
#include <znix/debug.h>

void kernel_init()
{
    console_init();
    BMB;
    DEBUGK("SDFSF");
    return;
}