#include <znix/bitmap.h>
#include <znix/string.h>
#include <znix/assert.h>

// 初始化位图
void bitmap_init(bitmap_t *map, char *bits, u32 length, u32 start)
{
    memset(bits, 0, length);
    bitmap_make(map, bits, length, start);
}

// 构造位图
void bitmap_make(bitmap_t *map, char *bits, u32 length, u32 offset)
{
    map->bits = bits;
    map->length = length;
    map->offset = offset;
}

// 测试位图的某一位是否为1
bool bitmap_test(bitmap_t *map, u32 index)
{
    assert(index >= map->offset);

    // 得到位图的索引
    idx_t idx = index - map->offset;

    // 位图数组中的字节
    u32 bytes = idx / 8;

    // 该字节的哪一位
    u8 bits = idx % 8;

    assert(bytes < map->length);

    // 返回index位置的值是否为1
    return (map->bits[bytes] & (1 << bits));
}

// 设置位图某位的值
void bitmap_set(bitmap_t *map, u32 index, bool value)
{
    // value 必须是二值
    assert(value == 0 || value == 1);

    assert(index >= map->offset);

    // 得到位图的索引
    idx_t idx = index - map->offset;

    // 位图数组中的字节
    u32 bytes = idx / 8;

    // 该字节的哪一位
    u8 bits = idx % 8;

    if (value)
    {
        // 设置为1
        map->bits[bytes] |= (1 << bits);
    }
    else
    {
        // 设置为0;
        map->bits[bytes] &= ~(1 << bits);
    }
}

// 从位图得到连续的count 位
int bitmap_scan(bitmap_t *map, u32 count)
{
    int start = EOF;                    // 标记开始的位置
    u32 bits_left = map->length * 8;    // 剩余位数
    u32 next_bit = 0;                   // 下一位
    u32 counter = 0;                    // 计数

    // 开始找
    while (bits_left-- > 0)
    {
        if (!bitmap_test(map, map->offset + next_bit))
        {
            // 如果下一位没有占用，计数器加一
            counter++;
        }
        else
        {
            // 否则计数器置0
            counter = 0;
        }

        // 下一位
        next_bit++;

        // 找到数量一致， 则设置开始的位置，结束
        if (counter == count)
        {
            start = next_bit - count;
            break;
        }
    }

    // 如果没有找到， 返回EOF
    if (start == EOF)
    {
        return EOF;
    }

    // 否则将找到的位， 全部置为1
    bits_left = count;
    next_bit = start;

    while (bits_left--)
    {
        bitmap_set(map, map->offset + next_bit, true);
        next_bit++;
    }

    // 返回索引
    return start + map->offset;
}
