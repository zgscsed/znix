#include <znix/task.h>
#include <znix/printk.h>
#include <znix/memory.h>
#include <znix/interrupt.h>
#include <znix/string.h>
#include <znix/bitmap.h>
#include <znix/assert.h>
#include <znix/debug.h>

extern bitmap_t kernel_map;

extern void task_switch(task_t *next);

#define NR_TASKS 64
static task_t *task_table[NR_TASKS];

// 从task_table中获取一个空闲的任务
static task_t *get_free_task()
{
    for (size_t i = 0; i < NR_TASKS; ++i)
    {
        if (task_table[i] == NULL)
        {
            task_table[i] = (task_t *)alloc_kpage(1);      // 一个进程分配一页
            return task_table[i];
        }
    }
    panic("No more tasks");
}

// 从任务数组中查找某种状态的任务，自己除外
static task_t *task_search(task_state_t state)
{
    // 切换过程，不能被中断打断，原子操作
    assert(!get_interrupt_state());

    task_t *task = NULL;
    task_t *current = running_task();            // 获取当前任务

    for (size_t i = 0; i < NR_TASKS; ++i)
    {
        task_t *ptr = task_table[i];
        if (ptr == NULL)
        {
            continue;
        }
        if (ptr->state != state)
        {
            continue;
        }
        if (current == ptr)
        {
            continue;
        }

        if (task == NULL || task->ticks < ptr->ticks || ptr->jiffies < task->jiffies)
        {
            task = ptr;
        }
    }
    return task;
}

task_t *running_task()
{
    // 将esp 最后三位抹掉，这个刚好是一页的开始
    asm volatile(
        "movl %esp, %eax\n"
        "andl $0xfffff000, %eax\n"
    );
}

void schedule()
{
    task_t *current = running_task();
    task_t *next = task_search(TASK_READY);

    assert(next != NULL);
    assert(next->magic == ZNIX_MAGIC);

    // 当前进程状态修改
    if (current->state == TASK_RUNNING)
    {
        current->state = TASK_READY;
    }
    
    next->state = TASK_RUNNING;
    if (next == current)
    {
        return;
    }
    task_switch(next);
}

static task_t *task_create(target_t target, const char *name, u32 priority, u32 uid)
{
    task_t *task = get_free_task();
    memset(task, 0, PAGE_SIZE);

    u32 stack = (u32)task + PAGE_SIZE;           // 栈底

    stack -= sizeof(task_frame_t);                 // 栈底腾出一个task_frame_t 结构体的内存大小用来保存frame
    task_frame_t *frame = (task_frame_t*)stack;    // 保存frame
    frame->ebx = 0x11111111;
    frame->esi = 0x22222222;
    frame->edi = 0x33333333;
    frame->ebp = 0x44444444;
    frame->eip = (void*)target;                    // 指向任务执行函数

    strcpy((char*)task->name, name);

    task->stack = (u32*)stack;
    task->priority = priority;
    task->ticks = task->priority;
    task->jiffies = 0;
    task->state = TASK_READY;
    task->uid = uid;
    task->vmap = &kernel_map;
    task->pde = KERNEL_PAGE_DIR;
    task->magic = ZNIX_MAGIC;

    return task;
}

// 起始进程对象，最开始初始化的时候使用，后面不在进入
static void task_setup()
{
    task_t *task = running_task();
    task->magic = ZNIX_MAGIC;
    task->ticks = 1;
}

u32 thread_a()
{
    set_interrupt_state(true);

    while (true)
    {
        printk("A");
    }
}

u32 thread_b()
{
    set_interrupt_state(true);

    while (true)
    {
        printk("B");
    }
}

u32 thread_c()
{
    set_interrupt_state(true);

    while (true)
    {
        printk("C");
    }
}

void task_init()
{
    task_setup();

    task_create(thread_a, "a", 5, KERNEL_USER);
    task_create(thread_b, "b", 5, KERNEL_USER);
    task_create(thread_c, "c", 5, KERNEL_USER);
}