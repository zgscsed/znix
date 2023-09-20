#ifndef ZNIX_STDIO_H_
#define ZNIX_STDIO_H_

#include <znix/stdarg.h>

// 格式化字符串，核心实现方法
int vsprintf(char *buf, const char *fmt, va_list args);
// 格式化字符串
int sprintf(char *buf, const char *fmt, ...);

// 用户态格式化输出
int printf(const char *fmt, ...);

#endif 