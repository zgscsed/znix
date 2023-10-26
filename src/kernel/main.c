#include <znix/debug.h>
#include <znix/interrupt.h>
#define LOGK(fmt, args...) DEBUGK(fmt, ##args)
extern void memory_map_init();
extern void mapping_init();
extern void interrupt_init();
extern void clock_init();
extern void time_init();
extern void rtc_init();
extern void task_init();
extern void syscall_init();
extern void hang();

void intr_test()
{
    bool intr = interrupt_disable();

    set_interrupt_state(intr);
}

// extern void memory_test();
void kernel_init()
{
    memory_map_init();
    mapping_init();
    interrupt_init();
    clock_init();

    task_init();

    syscall_init();
    // time_init();
    // rtc_init();

    // memory_test();
    // bitmap_tests();

    set_interrupt_state(true);
    return;
}