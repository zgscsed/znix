#include <znix/stdlib.h>

void delay(u32 count)
{
    while (count--)
    {
        ;
    }
}

void hang()
{
    while (true)
    {
        ;
    }
}

// 将bcd码转为整数
u8 bcd_to_bin(u8 value)
{
    return (value &0xf) + (value >> 4) * 10;
}
u8 bin_to_bcd(u8 value)
{
    return (value / 10) * 0x10 + (value %  10);
}

// 计算 num 分成 size 的数量， 向上取整
u32 div_round_up(u32 num, u32 size)
{
    return (num + size - 1) / size;
}