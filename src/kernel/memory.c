#include <znix/memory.h>
#include <znix/types.h>
#include <znix/debug.h>
#include <znix/assert.h>

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)

#define ZONE_VALIE 1     // ards 可以内存区域
#define ZONE_RESERVED 2  // ards 不可用区域

// 获取addr的页索引,  12位 16进制的三位， 一页是0x1000
#define IDX(addr) ((u32)addr >> 12)

typedef struct ards_t
{
    u64 base;    // 内存基地址
    u64 size;    // 内存长度
    u32 type;    // 类型
}_packed ards_t;

u32 memory_base = 0;    // 可用内存基地址， 应该等于1M
u32 memory_size = 0;    // 可用内存大小
u32 total_pages = 0;    // 所有内存页数
u32 free_pages = 0;     // 空闲内存页数

#define used_pages (total_pages - free_pages)      // 已用页数

void memory_init(u32 magic, u32 addr)
{
    u32 count;
    ards_t *ptr;

    // 如果loader 进入的内核
    if (magic == ZNIX_MAGIC)
    {
        count = *(u32*)addr;
        ptr = (ards_t*)(addr + 4);
        for (size_t i = 0; i < count; ++i, ++ptr)
        {
            LOGK("Memory base 0x%p size 0x%p type %d\n",
                 (u32)ptr->base, (u32)ptr->size, (u32)ptr->type);
            // 全局变量，选择一个最大可以内存
            if (ptr->type == ZONE_VALIE && ptr->size > memory_size)
            {
                memory_base = (u32)ptr->base;
                memory_size = (u32)ptr->size;
            }
        }
    }
    else
    {
        panic("Memory init magic unknown 0x%p\n", magic);
    }

    LOGK("ARDS count %d\n", count);
    LOGK("Memory base 0x%p\n", (u32)memory_base);
    LOGK("Memory size 0x%p\n", (u32)memory_size);

    assert(memory_base == MEMORY_BASE); // 内存开始的位置为 1M
    assert((memory_size & 0xfff) == 0); // 要求按页对齐

    total_pages = IDX(memory_size) + IDX(MEMORY_BASE);
    free_pages = IDX(memory_size);

    LOGK("Total pages %d\n", total_pages);
    LOGK("Free pages %d\n", free_pages);
}