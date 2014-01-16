#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
#include <msp430.h>
#include <legacymsp430.h>
#elif defined(__IAR_SYSTEMS_ICC__)
/* This is the IAR compiler */
#include <io430.h>
#endif

static int watchdog_backup;

void watchdog_stop()
{
	/* the upper 8 bits are a password */
	watchdog_backup = WDTCTL & 0x00FF;
	WDTCTL = WDTPW | WDTHOLD;
}

void watchdog_restore()
{
	WDTCTL = WDTPW | watchdog_backup;
}
