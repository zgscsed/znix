#include <znix/global.h>
#include <znix/debug.h>
#include <znix/string.h>

descriptor_t gdt[GDT_SIZE];         // 内核全局描述符表
pointer_t gdt_ptr;                  // 内核全局描述符表指针


void gdt_init()
{
    DEBUGK("init gdt!!!\n");

    asm volatile("sgdt gdt_ptr");

    memcpy(&gdt, (void*)gdt_ptr.base, gdt_ptr.limit + 1);

    gdt_ptr.base = (u32)&gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;

    asm volatile("lgdt gdt_ptr\n");
}