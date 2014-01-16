/**
 *  \file   timer.c
 *  \brief  eZ430-RF2500 tutorial, timers
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/

#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
#include <msp430.h>
#include <legacymsp430.h>
#elif defined(__IAR_SYSTEMS_ICC__)
/* This is the IAR compiler */
#include <io430.h>
#endif

#include <stdio.h>

#include "isr_compat.h"
#include "lpm_compat.h"
#include "timer.h"

/* ************************************************** */
/* TimerA on ACLK                                     */
/* ************************************************** */

static volatile timer_cb timerA_cb;
static volatile int timerA_wakeup;

ISR(TIMERA0, Timer_A)
{
	if (timerA_cb != NULL)
		timerA_cb();

	if (timerA_wakeup == 1)
		LPM_OFF_ON_EXIT;
}

void timerA_init(void)
{
	timerA_cb = NULL;
	timerA_wakeup = 0;
	timerA_stop();
}

void timerA_register_cb(timer_cb cb)
{
	timerA_cb = cb;
}

void timerA_set_wakeup(int w)
{
	timerA_wakeup = w;
}

void timerA_start_ticks(unsigned ticks)
{
	BCSCTL3 |= LFXT1S_2;	// LFXT1 = VLO
	TACCTL0 = CCIE;		// TCCR0 interrupt enabled
	TAR = 0;
	TACCR0 = ticks;
	TACTL = TASSEL_1 + MC_1;	// ACLK, upmode
}

#define VLO_FREQ 12000
#define TICKS_IN_MS (VLO_FREQ/1000)

void timerA_start_milliseconds(unsigned ms)
{
	timerA_start_ticks(ms * TICKS_IN_MS);
}

void timerA_stop(void)
{
	TACTL = 0;
}

/* ************************************************** */
/* TimerB on VLO 12kHz                                */
/* ************************************************** */

static volatile timer_cb timerB_cb;
static volatile int timerB_wakeup;

ISR(TIMERB0, Timer_B)
{
	if (timerB_cb != NULL)
		timerB_cb();

	if (timerB_wakeup == 1)
		LPM_OFF_ON_EXIT;
}

void timerB_init(void)
{
	timerB_cb = NULL;
	timerB_wakeup = 0;
	timerB_stop();
}

void timerB_register_cb(timer_cb cb)
{
	timerB_cb = cb;
}

void timerB_set_wakeup(int w)
{
	timerB_wakeup = w;
}

void timerB_start_ticks(unsigned ticks)
{
	BCSCTL3 |= LFXT1S_2;	// LFXT1 = VLO
	TBCCTL0 = CCIE;		// TCCR0 interrupt enabled
	TBR = 0;
	TBCCR0 = ticks;
	TBCTL = TBSSEL_1 + MC_1;	// ACLK, upmode
}

void timerB_start_milliseconds(unsigned ms)
{
	timerB_start_ticks(ms * TICKS_IN_MS);
}

void timerB_stop(void)
{
	TBCTL = 0;
}

/* ************************************************** */
/*                                                    */
/* ************************************************** */
