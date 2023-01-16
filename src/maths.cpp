#include "hamchat.h"
unsigned int ccitt_crc16(const unsigned char *data, size_t size) {
    size_t i, cnt;
    unsigned int work, cs;

    cs = 0xFFFF;
    if (size < 1)
        return ~cs;
    cnt = 0;
    do {
        work = 0x00FF & data[cnt++];
        for (i = 0; i < 8; i++) {
            if ((cs & 0x0001) ^ (work & 0x0001))
                cs = (cs >> 1) ^ 0x8408;
            else
                cs >>= 1;
            work >>= 1;
        }
    } while (cnt < size);
    cs = ~cs;
    work = cs;
    cs = (cs << 8) | (work >> 8 & 0x00FF);
    return cs & 0xFFFF;
}
