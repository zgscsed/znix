#ifndef ZNIX_STDARG_H_
#define ZNIX_STDARG_H_

typedef char *va_list;

#define stack_size(t) (sizeof(t) <= sizeof(char *) ? sizeof(char *) : sizeof(t))
// 获取v地址的下一个地址，v指向的传入参数的数量
#define va_start(ap, v) (ap = (va_list)&v + sizeof(char *))
// args 指针，先+=赋值指向下一个参数， 然后解引用得到上一个参数的值
#define va_arg(ap, t) (*(t *)((ap += stack_size(t)) - stack_size(t)))
#define va_end(ap) (ap = (va_list)0)

#endif