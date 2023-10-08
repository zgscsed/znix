#ifndef ZNIX_RTC_H_
#define ZNIX_RTC_H_

#include <znix/types.h>

u8 cmos_read(u8 addr);
void cmos_write(u8 addr, u8 value);

#endif //ZNIX_RTC_H_