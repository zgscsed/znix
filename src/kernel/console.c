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

// 清楚屏幕
static void erase_screen(console_t *con, u16 *start, u32 count);


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
}

// 设置当前屏幕开始的光标位置
static void set_cursor(console_t *con)
{
    outb(CRT_ADDR_REG, CRT_CURSOR_H);
    outb(CRT_DATA_REG, (con->pos - con->mem_base) >> 9 & 0xff);     // 设置高地址
    outb(CRT_ADDR_REG, CRT_CURSOR_L);
    outb(CRT_DATA_REG, (con->pos - con->mem_base) >> 1 & 0xff);     // 设置低地址
}

// 清楚屏幕
static void erase_screen(console_t *con, u16 *start, u32 count)
{
    int num = 0;
    while (num++ < count)
    {
        *start++ = con->erase;   // 设置为空格
    }
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

void console_write(console_t *con, char* buf, u32 count)
{

}