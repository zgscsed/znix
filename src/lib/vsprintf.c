#include <znix/stdio.h>
#include <znix/string.h>
#include <znix/types.h>
#include <znix/stdarg.h>
#include <znix/assert.h>

// flags
#define ZEROPAD 0x01 // 填充零
#define SIGN 0x02    // unsigned/signed long
#define PLUS 0x04    // 显示加
#define SPACE 0x08   // 如是加，则置空格
#define LEFT 0x10    // 左调整
#define SPECIAL 0x20 // 0x
#define SMALL 0x40   // 使用小写字母
#define DOUBLE 0x80  // 浮点数

// 判断字符是否是数字
bool is_digit(char ch)
{
    return ch >= '0' && ch <= '9';
}

// 转换整数
static int skip_atoi(const char **str)
{
    int num = 0;
    while (is_digit(**str))
    {
        num = num * 10 + *((*str)++) - '0';
    }
    return num;
}

// 将整数转换为指定进制的字符串
// str - 输出字符串指针
// num - 整数
// base - 进制基数
// size - 字符串长度
// precision - 数字长度(精度)
// flags - 选项
static char *number(char *str, u32 *num, int base, int size, int precision, int flags)
{
    char pad = ' ';     // 用于补齐宽度的字符， 默认值设置为空格
    char sign;          // 符号
    char temp[32];      // 数字转成字符， 临时存放
    int i;              // temp索引
    int index;          // 字母集索引

    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    // 1、先解析flags参数， 设置符号，占位符号
    // 如果flags 包含 用小写字母
    if (flags & SMALL)
    {
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    }

    // 如果flags 包含 左对齐, 不需要填零标志
    if (flags & LEFT)
    {
        flags &= ~ZEROPAD;
    }

    // 如果进制基数小于 2 或者大于 36,退出
    if (base < 2 || base > 32)
    {
        return 0;
    }

    // 如果flags 中包含 填零标志, 
    if (flags & ZEROPAD)
    {
        pad = '0';
    }

    // 如果flags 中包含 符号数标记, 并且数值小于0， 那么设置符号sign为负号， 并且设置num为绝对值
    if (flags & DOUBLE && (*(double*)(num)) < 0)
    {
        sign = '-';
        *(double*)(num) = -*(double*)(num);
    }
    else if (flags & SIGN && !(flags & DOUBLE) && ((int)(*num) <0))
    {
        sign = '-';
        (*num) = - (*num);
    }
    else
    {
        // 如果加号标记，则设置sign=加号
        if (flags & PLUS)
        {
            sign = '+';
        }
        // 如果有空格标记，则sign = ' '
        else if (flags & SPACE)
        {
            sign = ' ';
        }
        else
        {
            sign = 0;       // 其它情况设置0
        }
    }

    // 2、计算宽度, 如果有符号，则宽度值-1,
    if (sign)
    {
        size--;
    }

    // 如果flags 中包含特殊转换，16进制 宽度 - 2 (0x) ; 8进制宽度-1 （0）
    if (flags & SPECIAL)
    {
        if (base == 16)
        {
            size -= 2;
        }
        else if (base == 8)
        {
            --size;
        }
    }


    // 3、转换数字
    i = 0;
    // 浮点数，0， 整数三种情况处理
    if (flags & DOUBLE)
    {
        // 整数部分
        u32 ival = (u32)(*(double*)(num));
        // 小数部分
        u32 fval = (u32)((*(double*)(num) - ival)*1000000);
        do {
            index = (fval) % base;
            (fval) /= base;
            temp[i++] = digits[index];
        } while (fval);
        do {
            index = (ival) % base;
            (ival) /= base;
            temp[i++] = digits[index];
        } while (ival);

    }
    else if (*num == '0')
    {
        temp[i++] = '0';
    }
    else
    {
        while ((*num) != 0)
        {
            index = (*num) % base;
            (*num) /= base;
            temp[i++] = digits[index];       // 个位开始保存
        }
    }

    // 4、 精度计算, 如果字符长度大于精度，则精度扩展为数字个数
    if (i > precision)
    {
        precision = i;
    }

    // 宽度值 size 减去用户存放数字字符的个数
    size -= precision;

    // 5、开始转换
    // 如果flags中没有填0和左对齐标志, 则首先填放剩余宽度空格
    if (!(flags & (ZEROPAD + LEFT)))
    {
        while (size-- > 0)
        {
            *str++ = ' ';
        }
    }

    // 符号位
    if (sign)
    {
        *str++ = sign;
    }

    // 如果是特殊转换
    if (flags & SPECIAL)
    {
        // 8进制
        if (base == 8)
        {
            *str++ = '0';
        }
        else if (base == 16)
        {
            *str++ = '0';
            *str++ = digits[33];  // 16进制'0x'
        }
    }

    // 如果没有左对齐， 则剩余宽度 填补齐占位字符pad
    if (!(flags & LEFT))
    {
        while (size-- > 0)
        {
            *str++ = pad;
        }
    }
    // 如果数字个数小于进度，补齐 （精度-1)个'0'
    while (i < precision--)
    {
        *str++ = '0';
    }

    // 将数字字符填入字符串中
    while (i-- > 0)
    {
        *str++ = temp[i];
    }

    // 如果宽度大于0，有左对齐标记，剩余宽度放 空格
    while (size-- > 0)
    {
        *str++ = ' ';
    }

    return str;
}

// 格式化字符串，核心实现方法
int vsprintf(char *buf, const char *fmt, va_list args)
{
    int result_len;   // 返回值
    int len;

    // 指向处理后的字符串
    char *str;
    char *s;      // 临时用来执行字符串参数
    int *trans_num;     // 保存转换的字符数

    int flags;           // 标志参数
    int field_width;     // 输出字段宽度
    int precision;       // min 整书数字个数; max 字符串中字符个数
    int qualifier;       // 'h', 'l', 'L', 整型数转换后的输出类型形式

    u32 num;             // 传入的参数

    // 使用中间指针执行buf， 解析fmt，遍历字符，直到找到%， 进行格式转换
    for (str = buf; *fmt; ++fmt)
    {
        // 格式转换指示字符串以 % 开始
        // 当遍历fmt字符串找到%时， 开始处理格式转换字符串
        // 普通的字符串直接存入buf中
        if (*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }

        // 1、获取格式指示字符串中的标志域, 存入 flags
        /*
            - `-`：左对齐，默认为右对齐
            - `+`：输出 + 号
            - ` `：如果带符号的转换不以符号开头则添加空格，如果存在 `+` 则忽略
            - `#`：特殊转换：
                - 八进制，转换后字符串首位必须是 `0`
                - 十六进制，转换后必须以 `0x` 或 `0X` 开头
            - `0`：使用 `0` 代替空格
        */
        ++fmt;    // % 符号跳过
        // 可能有多个标志， 使用循环来遍历找到所有的标志
        flags = 0;
        while (*fmt)
        {
            if (*fmt == '-')
            {
                flags |= LEFT;    // 左对齐
                ++fmt;
                continue;
            }
            else if (*fmt == '+')
            {
                flags |= PLUS;    // 放加号
                ++fmt;
                continue;
            }
            else if (*fmt == ' ')
            {
                flags |= SPACE;   // 放空格
                ++fmt;
                continue;
            }
            else if (*fmt == '#')
            {
                flags |= SPECIAL;   // 特殊转换
                ++fmt;
                continue;
            }
            else if (*fmt == '0')
            {
                flags |= ZEROPAD;    // 填零，否则空格
                ++fmt;
                continue;
            }
            else
            {
                break;
            }
        }

        // 2、获取当前参数字段宽度, 存入 field_width; 宽度参数标志字符 *
        field_width = -1;

        // 如果是整数，即为指定宽度
        if (is_digit(*fmt))
        {
            // 将字符转为整数
            field_width = skip_atoi(&fmt);
        }
        // 如果是 '*' 特殊字符，表示下一个参数是指定的宽度
        else if (*fmt == '*')
        {
            ++fmt;
            field_width = va_arg(args, int);      // 返回整数类型的参数，宽度
        }

        // 3、获取格式指示字符串中精度， 存入 precision; 精度参数标志字符 '.'
        precision = -1;
        if (*fmt == '.')
        {
            // 如果是整数，即为指定精度
            if (is_digit(*fmt))
            {
                // 将字符转为整数
                precision = skip_atoi(&fmt);
            }
            // 如果是 '*' 特殊字符，表示下一个参数是指定的精度
            else if (*fmt == '*')
            {
                ++fmt;
                precision = va_arg(args, int);      // 返回整数类型的参数，精度
            }
            
            if (precision < 0)
            {
                precision = 0;
            }
        }

        // 4、获取长度修饰符, 存入 qualifier; 长度修饰符 'h', 'l', 'L'
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
        {
            qualifier = *fmt;
            ++fmt;
        }

        // 5、处理格式转换符
        switch (*fmt)
        {
            // 'c' : 参数是字符
            case 'c':
                {
                    // 如果不是左对齐，那么需要补齐 宽度-1 个空格
                    if (!(flags & LEFT))
                    {
                        while (--field_width > 0)
                        {
                            *str++ = ' ';
                        }
                    }
                    *str++ = (unsigned char)(va_arg(args, int));      // 传入参数
                    // 如果是左对齐，那么右侧补齐空格.如果是不是左对齐，那么下面的循环不会执行
                    while (--field_width > 0)
                    {
                        *str++ = ' ';
                    }
                    break;
                }
            // 's'， 参数是字符串
            case 's':
                {
                    s = va_arg(args, char*);
                    int len = strlen(s);
                    // 精度, 如果字符串长度超过精度，则设置长度为精度
                    if (precision < 0)
                    {
                        precision = len;
                    }
                    else if (len > precision)
                    {
                        len = precision;
                    }
                    // 后面处理类似字符参数
                    if (!(flags & LEFT))
                    {
                        // 那么需要补齐 宽度-len 个空格
                        while (len < field_width--)
                        {
                            *str++ = ' ';
                        }
                    }
                    for (int i = 0; i < len; ++i)
                    {
                        *str++ = *s++;
                    }
                    // 右对齐 那么需要补齐 宽度-len 个空格
                    while (len < field_width--)
                    {
                        *str++ = ' ';
                    }
                    break;
                }
            // 'o', 参数转为无符号八进制
            case 'o':
                {
                    num = va_arg(args, unsigned long);        // 无符号参数
                    str = number(str, &num, 8, field_width, precision, flags);
                    break;
                }
            // 'p', 以指针形式输出十六进制数
            case 'p':
                {
                    // 如果没有设置宽度，则默认设置为8， 并且填零标记
                    if (field_width == -1)
                    {
                        field_width = 8;
                        flags |= ZEROPAD;
                    }

                    num = va_arg(args, unsigned long);
                    str = number(str, &num, 16, field_width, precision, flags);
                    break;
                }
            // 'x' 无符号十六进制, 小写字母
            case 'x':
                {
                    flags |= SMALL;
                }
            // 'X' 无符号十六进制, 大写字母
            case 'X':
                {
                    num = va_arg(args, unsigned long);
                    str = number(str, &num, 16, field_width, precision, flags);
                    break;
                }
            // 被转换为带符号整数, 被转换的值数字个数较少，就会在其左边添零
            case 'd':
            case 'i':
                // 有符号
                flags |= SIGN;
            // 无符号
            case 'u':
                {
                    num = va_arg(args, unsigned long);
                    str = number(str, &num, 10, field_width, precision, flags);
                    break;
                }
            // 转换输出的字符个数保存到由对应输入指针指定的位置
            case 'n':
                {
                    trans_num = va_arg(args, int*);
                    *trans_num = (str - buf);
                    break;
                }
            case 'f':
                {
                    // 处理浮点数
                    flags |= SIGN;
                    flags |= DOUBLE;
                    double dnum = va_arg(args, double);
                    str = number(str, (u32*)&dnum, 10, field_width, precision, flags);
                    break;
                }
            case 'b':
                {
                    // binary
                    num = va_arg(args, unsigned long);
                    str = number(str, &num, 2, field_width, precision, flags);
                    break;
                }
            case 'm':
                {
                    // mac address
                    flags |= SMALL | ZEROPAD;
                    u8 *ptr = va_arg(args, char*);
                    for (size_t t = 0; t < 6; ++t, ++ptr)
                    {
                        int num = *ptr;
                        str = number(str, &num, 16, 2, precision, flags);
                        *str++ = ':';
                    }
                    str--;
                    break;
                }
            case 'r':
                {
                    // ip address
                    flags |= SMALL;
                    u8* ptr = va_arg(args, char*);
                    for (size_t t = 0; t < 4; ++t, ++ptr)
                    {
                        int num = *ptr;
                        str = number(str, &num, 10, field_width, precision, flags);
                        *str++ = '.';
                    }
                    str--;
                    break;
                }
            default:
            {
                // 如果格式转换符不是%,表示格式字符串有错
                if (*fmt != '%')
                {
                    // 直接写入一个 %
                    *str++ = '%';
                }
                // 如果格式转换符还有字符，直接写入
                if (*fmt)
                {
                    *str++ = *fmt;
                }
                else
                {
                    // 退出循环,处理到字符串结尾
                    --fmt;
                }
                break;
            }
        }
    }

    // 最后转换好的字符串结尾添加结束标志
    *str = '\0';
    // 返回转换后的字符串长度
    result_len = str - buf;

    assert(result_len < 1024);     // 不能超过1024
    return result_len;
}
// 格式化字符串
int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);

    return i;
}