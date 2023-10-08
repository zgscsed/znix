#ifndef ZNIX_H_
#define ZNIX_H_

// 内核魔数，用于校验错误
#define ZNIX_MAGIC 0x20220205

void kernel_init();   // 初始化内核

#endif