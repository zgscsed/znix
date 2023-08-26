#ifndef ZNIX_IO_H_
#define ZNIX_IO_H_

#include <znix/types.h>

extern u8 inb(u16 port);    // 输入一个字符
extern u16 inw(u16 port);   // 输入一个字

extern void outb(u16 port, u8 value);        // 输出一个字节
extern void outw(u16 port, u16 value);       // 输出一个字

#endif //ZNIX_IO_H_