#ifndef ZNIX_SYSCALL_H_
#define ZNIX_SYSCALL_H_

#include <znix/types.h>

typedef enum syscall_t
{
    SYS_NR_TEST,
    SYS_NR_YIELD,
} syscall_t;

u32 test();
void yield();

#endif //ZNIX_SYSCALL_H_