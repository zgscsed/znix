#ifndef ZNIX_STDLIB_H_
#define ZNIX_STDLIB_H_

#include <znix/types.h>

// 简单实现时延
void delay(u32 count);
// 阻塞,死循环
void hang();

#endif