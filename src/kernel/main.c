#include <znix/znix.h>
#include <znix/io.h>
#include <znix/string.h>
#include <znix/console.h>

char message[] = "hello znix!!!\n";
char message1[] = "\bhello \bzni\tx!!!\n";
char buf[1024];
void kernel_init()
{
    console_init();

    console_writes(message, sizeof(message));
    console_writes(message1, sizeof(message));

    return;
}