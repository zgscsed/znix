#include <znix/znix.h>

int magic = ZNIX_MAGIC;

char message[] = "hello znix!!!";       // .data
char buf[1024];                        //  .bss
void kernel_init()
{
    char *video = (char*)0xb8000;  // 文本显示器的内存位置
    for (int i = 0; i < sizeof(message); ++i)
    {
        video[i * 2] = message[i];
    }

    int a = 10;
}