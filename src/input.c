#include "nes.h"
u8 input_read(void)
{
    u8 result = 0;
    u8 i;
    JOY = 1;
    JOY = 0;
    for (i = 0; i < 8; ++i) result = (result << 1) | (JOY & 1);
    return result;
}
