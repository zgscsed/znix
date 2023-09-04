#include <znix/string.h>

// 字符串拷贝， 将src 字符串拷贝给dest字符串
char *strcpy(char *dest, const char *src)
{
    char* ptr = dest;
    while (*src != EOS)
    {
        *ptr++ = *src++;
    }
    *ptr = EOS;
    return dest;
}
// 字符串拷贝， 将src 字符串最多拷贝 count 个字符 到dest字符串
char *strncpy(char *dest, const char *src, size_t count)
{
    char *ptr = dest;
    size_t i = 0;
    for (; i < count; ++i)
    {
        *ptr++ = *src;
        if (*src++ == EOS)
        {
            return dest;
        }
    }

    dest[count - 1] = EOS;
    return dest;
}
// 拼接字符串，将第二个字符串参数拼接到第一个字符串参数结尾。
char *strcat(char *dest, const char *src)
{
    char *ptr = dest;
    while (*ptr != EOS)
    {
        *ptr++;
    }

    while (*src != EOS)
    {
        *ptr++ = *src++;
    }
    *ptr = EOS;
    return dest;
}

// 返回字符串长度， 遇到 '\0'返回
size_t strlen(const char *str)
{
    char *ptr = (char*)str;
    while (*ptr != EOS)
    {
        *ptr++;
    }

    return ptr - str;     // 指针相减， 返回长度
}

// 用于比较两个字符串并根据比较结果返回整数。lhs = rhs 返回0， lhs < rhs 返回 -1， lhs > rhs 返回正数
int strcmp(const char *lhs, const char *rhs)
{
    while ((*lhs == *rhs) && *lhs != EOS && *rhs != EOS)
    {
        ++lhs;
        ++rhs;
    }

    return *lhs < *rhs ? -1 : *lhs > *rhs;     // lhs < rhs return -1；  lhs > rhs return 1;  lhs == rhs return false 0
}

// 在一个str字符串中查找给定字符的第一个匹配的位置。
char *strchr(const char *str, int ch)
{
    char *ptr = (char*)str;
    while (*ptr != EOS)
    {
        if (*ptr == ch)
        {
            return ptr;
        }

        ++ptr;
    }

    return NULL;
}
// 查找字符在指定字符串中从右面开始的第一次出现的位置，如果成功，返回该字符以及其后面的字符，如果失败，则返回 NULL
char *strrchr(const char *str, int ch)
{
    char *lastPtr = NULL;
    char *ptr = (char*)str;
    while (*ptr != EOS)
    {
        if (*ptr == ch)
        {
            lastPtr = ptr;
        }

        ++ptr;
    }

    return lastPtr;

}

// 比较count字节， 返回值类似strcmp
int memcmp(const void *lhs, const void *rhs, size_t count)
{
    char *plhs = (char*)lhs;
    char *prhs = (char*)rhs;

    while ((count > 0) && *plhs != EOS && *prhs != EOS)
    {
        ++plhs;
        ++prhs;
        --count;
    }

    if (count == 0)
    {
        return 0;
    }

    return *plhs < *prhs ? -1 : *plhs > *prhs;
}
// 某一块内存中的内容全部设置为指定的值
void *memset(void *dest, int ch, size_t count)
{
    char *ptr = dest;
    while (count--)
    {
        *ptr++ = ch;         // 没有保护，可能越界
    }
    return dest;
}

// 从源内存地址的起始位置开始拷贝若干个字节到目标内存地址中
void *memcpy(void *dest, const void *src, size_t count)
{
    char *ptr = dest;

    while (count--)
    {
        *ptr++ = *((char*)src++);  // 没有保护，可能越界
    }

    return dest;
}
// ptr所指内存区域的前count个字节查找字符ch。如果成功，返回指向字符ch的指针；否则返回NULL。
void *memchr(const void *buf, int ch, size_t count)
{
    char *ptr = (char*)buf;
    while (count--)
    {
        if (*ptr == EOS)
        {
            return NULL;
        }
        if (*ptr == ch)
        {
            return ptr;
        }
        ++ptr;
    }

    return NULL;
}

#define SEPARATOR1 '/'                                       // 目录分隔符 1
#define SEPARATOR2 '\\'                                      // 目录分隔符 2
#define IS_SEPARATOR(c) (c == SEPARATOR1 || c == SEPARATOR2) // 字符是否位目录分隔符
// 获取第一个分隔符
char *strsep(const char *str)
{
    char *ptr = (char*)str;

    while (*ptr != EOS)
    {
        if (IS_SEPARATOR(*ptr))
        {
            return ptr;
        }
        ++ptr;
    }

    return NULL;
}

// 获取最后一个分隔符
char *strrsep(const char *str)
{
    char *lastPtr = NULL;
    char *ptr = (char*)str;

    while (*ptr != EOS)
    {
        if (IS_SEPARATOR(*ptr))
        {
            lastPtr = ptr;
        }
        ++ptr;
    }

    return lastPtr;
}