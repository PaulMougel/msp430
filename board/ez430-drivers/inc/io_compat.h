#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
#include <iomacros.h>
#define SFRB(varname, address) sfrb(varname, address)
#include <msp430.h>
#elif defined(__IAR_SYSTEMS_ICC__)
/* This is the IAR compiler */
#include <intrinsics.h>
#define SFRB(varname, address) __no_init volatile unsigned char varname @ address
#define BV(x) (1 << (x))
#endif
