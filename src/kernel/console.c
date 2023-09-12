#include <znix/console.h>
#include <znix/io.h>
#include <znix/string.h>


#define CRT_ADDR_REG 0x3D4 // CRT(6845)索引寄存器
#define CRT_DATA_REG 0x3D5 // CRT(6845)数据寄存器

#define CRT_START_ADDR_H 0xC // 显示内存起始位置 - 高位
#define CRT_START_ADDR_L 0xD // 显示内存起始位置 - 低位
#define CRT_CURSOR_H 0xE     // 光标位置 - 高位
#define CRT_CURSOR_L 0xF     // 光标位置 - 低位

#define MEM_BASE 0xB8000              // 显卡内存起始位置
#define MEM_SIZE 0x4000               // 显卡内存大小
#define MEM_END (MEM_BASE + MEM_SIZE) // 显卡内存结束位置
#define WIDTH 80                      // 屏幕文本列数
#define HEIGHT 25                     // 屏幕文本行数
#define ROW_SIZE (WIDTH * 2)          // 每行字节数
#define SCR_SIZE (ROW_SIZE * HEIGHT)  // 屏幕字节数

#define NUL 0x00
#define ENQ 0x05
#define ESC 0x1B // ESC
#define BEL 0x07 // \a
#define BS 0x08  // \b
#define HT 0x09  // \t
#define LF 0x0A  // \n
#define VT 0x0B  // \v
#define FF 0x0C  // \f
#define CR 0x0D  // \r
#define DEL 0x7F

#define ERASE 0x0720   // 空格，小端

#define STYLE 7        // 字符样式

static console_t console;

// 获得当前显示器的内存开始位置
static void get_screen(console_t* con);
// 设置屏幕内存开始位置
static void set_screen(console_t* con);
// 获取当前屏幕开始的光标位置
static void get_cursor(console_t *con);
// 设置当前屏幕开始的光标位置
static void set_cursor(console_t *con);
// 计算光标xy坐标
static void calculate_cursor_xy(console_t *con);

// 清楚屏幕
static void erase_screen(console_t *con, u16 *start, u32 count);
// 向上滚屏
static void scroll_up(console_t *con);

// \b 退格
static void bs(console_t *con);
// \t tab宽度相当于8个空格的位， 不支持设置空格数量
static void ht(console_t *con);
// \n 换行,  如果屏幕没有满，那么不向上滚屏
static void lf(console_t *con);
// \r 将光标移动这行的起始位置
static void cr(console_t *con);
// 打印一个字符
static void write_charator(console_t *con, char ch);

// 获得当前显示器的内存开始位置
static void get_screen(console_t* con)
{
    outb(CRT_ADDR_REG, CRT_START_ADDR_H);
    con->screen = inb(CRT_DATA_REG) << 8;   // 高8位
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);
    con->screen |= inb(CRT_DATA_REG);

    con->screen <<= 1;                       // 需要 * 2， 两个字节控制一位
    con->screen += con->mem_base;
}

// 设置屏幕内存开始位置
static void set_screen(console_t* con)
{
    outb(CRT_ADDR_REG, CRT_START_ADDR_H);
    outb(CRT_DATA_REG, (con->screen - con->mem_base) >> 9 & 0xff);     // 设置高地址
    outb(CRT_ADDR_REG, CRT_START_ADDR_L);
    outb(CRT_DATA_REG, (con->screen - con->mem_base) >> 1 & 0xff);     // 设置低地址
}

// 获取当前屏幕开始的光标位置
static void get_cursor(console_t *con)
{
    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    con->pos = inb(CRT_DATA_REG) << 8;   // 高8位
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    con->pos |= inb(CRT_DATA_REG);

    con->pos <<= 1;                       // 需要 * 2， 两个字节控制一位
    con->pos += con->mem_base;

    // 计算xy坐标
    calculate_cursor_xy(con);
}

// 设置当前屏幕开始的光标位置
static void set_cursor(console_t *con)
{
    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    outb(CRT_DATA_REG, (con->pos - con->mem_base) >> 9 & 0xff);     // 设置高地址
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    outb(CRT_DATA_REG, (con->pos - con->mem_base) >> 1 & 0xff);     // 设置低地址

    // 计算xy坐标
    calculate_cursor_xy(con);
}

// 计算光标xy坐标
static void calculate_cursor_xy(console_t *con)
{
    u32 delta = (con->pos - con->screen) >> 1;      // 光标和屏幕起始的相对值
    con->x = delta % con->width;
    con->y = delta / con->width;
}

// 清除屏幕
static void erase_screen(console_t *con, u16 *start, u32 count)
{
    int num = 0;
    while (num++ < count)
    {
        *start++ = con->erase;   // 设置为空格
    }
}

// 退格 \b
static void bs(console_t *con)
{
    // 如果当前光标在一行的起始位置，不能退格
    if (con->x == 0)
    {
        return;
    }

    con->x--;
    con->pos -= 2;
    *(u16*)con->pos = con->erase;      // 不清除当前字符，如果后面还有字符，覆盖
}

// \t tab宽度相当于8个空格的位， 不支持设置空格数量
static void ht(console_t *con)
{
    // 对齐8位
    int offset = 8 - (con->x & 7);       // 8位空格
    con->x += offset;
    con->pos += (offset << 1);

    // 增加空格后，可能需要换行
    if (con->x >= con->width)
    {
        con->x -= con->width;
        con->pos -= con->row_size;
        lf(con);
    }
}
// 向上滚屏
static void scroll_up(console_t *con)
{
    // 判断是否达到显示器的内存上限
    if (con->screen + con->scr_size + con->row_size >= con->mem_end)
    {
        // 将当前屏幕的起始的值拷贝到mem_base处，重新设置屏幕起始位置和光标位置
        memcpy(con->mem_base, con->screen, con->scr_size);
        con->pos -= con->screen - con->mem_base;        //  光标位置减少， xy相对坐标不变
        con->screen = con->mem_base;
    }

    // 屏幕向上滚动一行，新的一行显示到屏幕，需要先清空
    u16 *ptr = (u16*)(con->screen + con->scr_size);
    erase_screen(con, ptr, con->width);
    // 屏幕位置增加一行
    con->screen += con->row_size;
    con->pos += con->row_size;          // xy坐标不变
    set_screen(con);       // 设置屏幕位置
}

// 换行\n,  如果屏幕没有满，那么不向上滚屏
static void lf(console_t *con)
{
    // 没有增加一行，没有超过屏幕的高
    if (con->y + 1 < con->height)
    {
        con->y++;
        con->pos += con->row_size;       // 增加一行， 相对位置加一行
        return;
    }
    scroll_up(con);
}

// '\r' 将光标移动这行的起始位置
static void cr(console_t *con)
{
    con->pos -= (con->x << 1);       // x*2 表示这个行现有的内存大小
    con->x = 0;
}

// del 删除当前字符
static void del(console_t *con)
{
    *(u16*)con->pos = con->erase;
}

// 打印一个字符
static void write_charator(console_t *con, char ch)
{
    // 如果需要换行
    if (con->x >= con->width)
    {
        con->x -= con->width;
        con->pos -= con->row_size;        // 这里需要提前减少一行的位置大小， 因为在换行函数中，pos 会增加
        lf(con);
    }

    // 当前光标位置设置新的值，然后更新坐标
    *con->ptr++ = ch;                // 前一个字节 字符
    *con->ptr++ = con->style;        // 后一个字节 样式
    con->x++;
}

void console_init()
{
    console_t* con = &console;
    con->mem_base = MEM_BASE;
    con->mem_size = (MEM_SIZE / ROW_SIZE) * ROW_SIZE;
    con->mem_end = con->mem_base + con->mem_size;

    con->width = WIDTH;
    con->height = HEIGHT;
    con->row_size = con->width * 2;
    con->scr_size = con->width * con->height * 2;
    con->erase = ERASE;
    con->style = STYLE;
    console_clear(con);
}
void console_clear(console_t *con)
{
    if (con == NULL)
    {
        return;
    }
    con->screen = con->mem_base;
    con->pos = con->mem_base;
    con->x = 0;
    con->y = 0;
    set_screen(con);
    set_cursor(con);
    erase_screen(con, con->mem_base, con->mem_size);
}

/*
#define ENQ 0x05
#define ESC 0x1B // ESC
#define BEL 0x07 // \a
#define BS 0x08  // \b
#define HT 0x09  // \t
#define LF 0x0A  // \n
#define VT 0x0B  // \v
#define FF 0x0C  // \f
#define CR 0x0D  // \r
#define DEL 0x7F
*/
void console_write(console_t *con, char* buf, u32 count)
{
    char ch;
    while (count--)
    {
        ch = *buf++;
        switch (ch)
        {
            case NUL:
                break;
            case BEL:
                // 蜂鸣器
                break;
            case BS:
                bs(con);
                break;
            case HT:
                ht(con);
                break;
            case LF:
                lf(con);
                cr(con);
                break;
            case VT:
                break;
            case FF:
                break;
            case CR:
                cr(con);
                break;
            case DEL:
                del(con);
                break;
            default:
                write_charator(con, ch);
        }
        set_cursor(con);  // 处理一个字符更新光标
    }
}

// 写控制台
void console_writes(char* buf, u32 count)
{
    console_t *con = &console;
    console_write(con, buf, count);
}