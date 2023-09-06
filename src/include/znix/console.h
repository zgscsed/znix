#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <znix/types.h>

typedef struct console_t {
    u32 mem_base;      // 内存基地址位置
    u32 mem_size;      // 内存大小
    u32 mem_end;       // 内存结束位置

    u32 screen;        // 记录当前显示器开始的内存位置
    u32 scr_size;      // 屏幕内存大小

    u32 pos;           // 记录当前光标的内存位置

    u32 x;             // 光标坐标 x
    u32 y;             // 光标坐标 y

    u32 width;         // 显示器宽度
    u32 height;        // 显示器高度
    u32 row_size;      // 行内存大小

    u16 erase;         // 清屏字符
    u8 style;          // 字符样式
}console_t;

void console_init();
void console_clear(console_t *con);
void console_write(console_t *con, char* buf, u32 count);

#endif // CONSOLE_H_