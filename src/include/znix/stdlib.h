#ifndef ZNIX_STDLIB_H_
#define ZNIX_STDLIB_H_

#include <znix/types.h>

// 简单实现时延
void delay(u32 count);
// 阻塞,死循环
void hang();

u8 bcd_to_bin(u8 value);
u8 bin_to_bcd(u8 value);

#endif