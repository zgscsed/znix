#include <znix/znix.h>
#include <znix/io.h>
#include <znix/string.h>
#include <znix/console.h>

char message[] = "hello znix!!!\n";
char buf[1024];
void kernel_init()
{
    console_init();
    while (1)
    {
        console_writes(message, sizeof(message));
    }

    return;
}