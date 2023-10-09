#include <znix/debug.h>

extern void memory_map_init();
extern void mapping_init();
extern void interrupt_init();
extern void clock_init();
extern void time_init();
extern void rtc_init();
extern void hang();

extern void memory_test();
void kernel_init()
{
    memory_map_init();
    mapping_init();
    interrupt_init();
    // task_init();

    // clock_init();
    // time_init();
    // rtc_init();

    // memory_test();

    BMB;

    char *ptr = (char *)(0x100000 * 20);
    ptr[0] = 'a';

    // asm volatile("sti\n");
    hang();

    return;
}