#ifndef ZNIX_DEBUG_H_
#define ZNIX_DEBUG_H_

void debugk(char *file, int line, const char *fmt, ...);

#define BMB asm volatile("xchgw %bx, %bx") // bochs magic breakpoint
// 替换的方式是将参数的两个部分以##连接。##表示连接变量代表前面的参数列表。
//使用这种形式可以将宏的参数传递给一个参数。args…是宏的参数，表示可变的参数列表，使用##args将其传给printf函数.
#define DEBUGK(fmt, args...) debugk(__BASE_FILE__, __LINE__, fmt, ##args)
#endif