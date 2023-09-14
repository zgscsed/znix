#include <znix/znix.h>
#include <znix/io.h>
#include <znix/string.h>
#include <znix/console.h>
#include <znix/stdarg.h>


void test_args(int cnt, ...)
{
    va_list args;
    va_start(args, cnt);
    int arg;
    
    while (cnt--)
    {
        arg = va_arg(args, int);
    }
    va_end(args);
}

void kernel_init()
{
    console_init();
    test_args(5, 1, 0xaa, 5, 0x55, 10);
    return;
}