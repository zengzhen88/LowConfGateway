#include <stdint.h>

#ifdef USE_ARMV
static __inline int16_t SatW32ToW16(int32_t value32) {
    int32_t out = 0;

    __asm __volatile ("ssat %0, #16, %1" : "=r"(out) : "r"(value32));

    return (int16_t)out;
}
#else 
#define SatW32ToW16(value32) \
    ((value32) > 32767 ? 32767 : (value32) < -32768 ? -32768 : (value32))
#endif



