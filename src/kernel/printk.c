#include <znix/printk.h>
#include <znix/stdarg.h>
#include <znix/stdio.h>
#include <znix/console.h>


static char buf[1024];

// 内核态，标准输出到屏幕
int printk(const char*fmt,  ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    // 格式化到buf中，并返回字符串的长度
    i = vsprintf(buf, fmt, args);

    va_end(args);
    // 输出到屏幕
    console_writes(buf, i);
}