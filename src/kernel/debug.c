#include <znix/debug.h>
#include <znix/stdarg.h>
#include <znix/stdio.h>
#include <znix/printk.h>

static char buf[1024];
void debugk(char *file, int line, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    printk("[%s] [%d] %s", file, line, buf);
}