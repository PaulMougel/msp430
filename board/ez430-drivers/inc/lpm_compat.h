#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
//#include <msp430/common.h>
#define LPM(n) LPM ## n
#define LPM_OFF_ON_EXIT LPM4_EXIT
#elif defined(__IAR_SYSTEMS_ICC__)
/* This is the IAR compiler */
#include <intrinsics.h>
#define LPM(n) __low_power_mode_ ## n ## ()
#define LPM_OFF_ON_EXIT __low_power_mode_off_on_exit()
#endif
