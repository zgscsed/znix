#include <znix/memory.h>
#include <znix/types.h>
#include <znix/stdlib.h>
#include <znix/string.h>
#include <znix/debug.h>
#include <znix/assert.h>

#define LOGK(fmt, args...) DEBUGK(fmt, ##args)

#define ZONE_VALIE 1     // ards 可以内存区域
#define ZONE_RESERVED 2  // ards 不可用区域

// 获取addr的页索引,  12位 16进制的三位， 一页是0x1000
#define IDX(addr) ((u32)addr >> 12)
// 获取页索引 idx 对应的页的开始位置
#define PAGE(idx) ((u32)idx << 12)
// 检测地址是否页起始位置
#define ASSERT_PAGE(addr) assert((addr & 0xfff) == 0)

typedef struct ards_t
{
    u64 base;    // 内存基地址
    u64 size;    // 内存长度
    u32 type;    // 类型
}_packed ards_t;

static u32 memory_base = 0;    // 可用内存基地址， 应该等于1M
static u32 memory_size = 0;    // 可用内存大小
static u32 total_pages = 0;    // 所有内存页数
static u32 free_pages = 0;     // 空闲内存页数

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

//****** 管理物理内存

static u32 start_page = 0;            // 可分配物理内存起始位置
static u8 *memory_map;                // 物理内存数组
static u32 memory_map_pages;          // 物理内存数组占用的页数

void memory_map_init()
{
    // 初始化物理内存
    memory_map = (u8*)memory_base;

    // 计算物理内存数组占用的页数
    memory_map_pages = div_round_up(total_pages, PAGE_SIZE);
    LOGK("Memory map page count %d\n", memory_map_pages);

    // 空闲页数减少
    free_pages -= memory_map_pages;

    //清空物理内存数组
    memset((void*)memory_map, 0, memory_map_pages * PAGE_SIZE);

    // 前1M 的内存位置以及物理内存数组已占用的页, 后面的内存可以分配
    start_page = IDX(MEMORY_BASE) + memory_map_pages;

    for (size_t i = 0; i < start_page; ++i)
    {
        memory_map[i] = 1;     // 标记被占用
    }

    LOGK("Total pages %d free pages %d\n", total_pages, free_pages);
}

// 分配一页物理内存
static u32 get_page()
{
    for (size_t i = start_page; i < total_pages; ++i)
    {
        // 如果物理内存没有占用
        if (!memory_map[i])
        {
            memory_map[i] = 1;
            free_pages -= 1;
            assert(free_pages >= 0);   // 空闲内存页不能为负数
            u32 page = ((u32)i) << 12;    // 根据内存索引，得到页起始地址
            LOGK("Get page 0x%p\n", page);
            return page;
        }
    }
    panic("Out of Memory!!!!");
}

// 释放一页物理内存
static void put_page(u32 addr)
{
    ASSERT_PAGE(addr);

    u32 idx = IDX(addr);

    // idx 大于 1M， 并且小于总页数
    assert(idx >= start_page && idx < total_pages);
    // 保证至少有一个引用
    assert(memory_map[idx] >= 1);

    // 物理引用减一
    memory_map[idx]--;

    // 如果为0， 则空闲页加一
    if (!memory_map[idx])
    {
        free_pages++;
    }

    assert(free_pages > 0 && free_pages < total_pages);
    LOGK("Put page 0x%p\n", addr);
}

void memory_test()
{
    u32 pages[10];
    for (size_t i = 0; i < 10; ++i)
    {
        pages[i] = get_page();
    }

    for (size_t i = 0; i < 10; ++i)
    {
        put_page(pages[i]);
    }
}

//end

// ********内存映射

// 得到 cr3寄存器
u32 inline get_cr3()
{
    // 直接将 mov eax, cr3, 返回值在eax中
    asm volatile("movl %cr3, %eax\n");
}

// 设置cr3 寄存器, 参数是页目录的地址
void inline set_cr3(u32 pde)
{
    ASSERT_PAGE(pde);
    asm volatile("movl %%eax, %%cr3\n" ::"a"(pde));
}

// 将cr0 寄存器最高位 PE 置为1, 启用分页
static inline void enable_page()
{
    // 0b1000_0000_0000_0000_0000_0000_0000_0000
    // 0x80000000
    asm volatile(
        "movl %cr0, %eax\n"
        "orl $0x80000000, %eax\n"
        "movl %eax, %cr0\n");
}

// 初始化页表项
static void entry_init(page_entry_t *entry, u32 index)
{
    *(u32*)entry = 0;
    entry->present = 1;
    entry->write = 1;
    entry->user = 1;
    entry->index = index;
}

// 内核页目录
#define KERNEL_PAGE_DIR 0x200000

// 内核页表
#define KERNEL_PAGE_ENTRY 0x201000

// 初始化内存映射
void mapping_init()
{
    // 初始化页目录
    page_entry_t *pde = (page_entry_t*)KERNEL_PAGE_DIR;
    memset(pde, 0, PAGE_SIZE);

    entry_init(&pde[0], IDX(KERNEL_PAGE_ENTRY));

    // 初始化页表
    page_entry_t *pte = (page_entry_t*)KERNEL_PAGE_ENTRY;
    memset(pte, 0, PAGE_SIZE);    // 先初始化一页,可以映射 4M 物理内存
    
    page_entry_t *entry;
    for (size_t tidx = 0; tidx < 1024; ++tidx)
    {
        entry = &pte[tidx];
        entry_init(entry, tidx);
        memory_map[tidx] = 1;    // 设置物理内存数组，该页被占用
    }

    BMB;

    // 设置cr3寄存器, 设置页目录
    set_cr3((u32)pde);

    BMB;

    // 分页有效
    enable_page();
}
// end