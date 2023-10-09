#ifndef ZNIX_STDLIB_H_
#define ZNIX_STDLIB_H_

#include <znix/types.h>

#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a < b ? a : b)

// 简单实现时延
void delay(u32 count);
// 阻塞,死循环
void hang();

u8 bcd_to_bin(u8 value);
u8 bin_to_bcd(u8 value);

u32 div_round_up(u32 num, u32 size);

#endif