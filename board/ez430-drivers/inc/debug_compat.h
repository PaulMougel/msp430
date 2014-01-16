#if defined(DEBUG)
#include <stdio.h>
#define DBG_PRINTF printf
#else
#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
#define DBG_PRINTF(x...) do { } while (0)
#elif defined(__IAR_SYSTEMS_ICC__)
/* This is the IAR compiler */
#define DBG_PRINTF(x) do { } while (0)
#endif
#endif
