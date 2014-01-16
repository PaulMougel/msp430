/**
 *  \file   clock.c
 *  \brief  eZ430-RF2500 tutorial, clock
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
#include <intrinsics.h>
#endif

#include <stdio.h>

#include "clock.h"

/*
 *  After a PUC, MCLK and SMCLK are sourced from DCOCLK at ~1.1 MHz (see
 * the device-specific data sheet for parameters) and ACLK is sourced from
 * LFXT1CLK in LF mode with an internal load capacitance of 6pF.
 *
 * DOC 5.1 page 287
 *
 * a 32 kHz osc is available on P2.1 (TAINCLK) to run the TimerA. Clock is
 * sourced from the m41t93 RTC chip.
 *
 */

static unsigned int mclk_freq_mhz = 0;

/***************************************************************
 * we have to wait OFIFG to be sure the switch is ok
 * slau049e.pdf page 4-12 [pdf page 124]
 ***************************************************************/

#define WAIT_CRISTAL() do { } while (0)

int get_mclk_freq_mhz()
{
	return mclk_freq_mhz;
}

static void set_mcu_speed(unsigned char dco_mhz, unsigned char smclk_divider)
{
	switch (dco_mhz) {
	case 1:
		BCSCTL1 = CALBC1_1MHZ;
		DCOCTL = CALDCO_1MHZ;
		break;
	case 8:
		BCSCTL1 = CALBC1_8MHZ;
		DCOCTL = CALDCO_8MHZ;
		break;
	case 12:
		BCSCTL1 = CALBC1_12MHZ;
		DCOCTL = CALDCO_12MHZ;
		break;
	case 16:
		BCSCTL1 = CALBC1_16MHZ;
		DCOCTL = CALDCO_16MHZ;
		break;
	}
	switch (smclk_divider) {
	case 1:
		/* SELM_0: MCL Source Select 0: DCOCLK */
		/* DIVS_1: SMCLK divider /1 */
		BCSCTL2 = SELM_0 | DIVS_0;
		break;
	case 2:
		/* DIVS_1: SMCLK divider /2 */
		BCSCTL2 = SELM_0 | DIVS_1;
		break;
	case 4:
		/* DIVS_2: SMCLK divider /4 */
		BCSCTL2 = SELM_0 | DIVS_2;
		break;
	case 8:
		/* DIVS_3: SMCLK divider /8 */
		BCSCTL2 = SELM_0 | DIVS_3;
		break;
	}

	WAIT_CRISTAL();

	mclk_freq_mhz = dco_mhz;
}

void set_mcu_speed_dco_mclk_1MHz_smclk_1MHz()
{
	set_mcu_speed(1, 1);
}

void set_mcu_speed_dco_mclk_8MHz_smclk_8MHz()
{
	set_mcu_speed(8, 1);
}

void set_mcu_speed_dco_mclk_8MHz_smclk_4MHz()
{
	set_mcu_speed(8, 2);
}

void set_mcu_speed_dco_mclk_8MHz_smclk_2MHz()
{
	set_mcu_speed(8, 4);
}

void set_mcu_speed_dco_mclk_8MHz_smclk_1MHz()
{
	set_mcu_speed(8, 8);
}

void set_mcu_speed_dco_mclk_12MHz_smclk_12MHz()
{
	set_mcu_speed(12, 1);
}

void set_mcu_speed_dco_mclk_12MHz_smclk_6MHz()
{
	set_mcu_speed(12, 2);
}

void set_mcu_speed_dco_mclk_12MHz_smclk_3MHz()
{
	set_mcu_speed(12, 4);
}

void set_mcu_speed_dco_mclk_12MHz_smclk_1_5MHz()
{
	set_mcu_speed(12, 8);
}

void set_mcu_speed_dco_mclk_16MHz_smclk_16MHz()
{
	set_mcu_speed(16, 1);
}

void set_mcu_speed_dco_mclk_16MHz_smclk_8MHz()
{
	set_mcu_speed(16, 2);
}

void set_mcu_speed_dco_mclk_16MHz_smclk_4MHz()
{
	set_mcu_speed(16, 4);
}

void set_mcu_speed_dco_mclk_16MHz_smclk_2MHz()
{
	set_mcu_speed(16, 8);
}

/* the IAR version of loop_4_cycles is defined in loop_4_cycles.s43 */
#if defined(__GNUC__) && defined(__MSP430__)
/* uint32_t version */
/* will block 4*n cycles + constant (function jump and init) */
void loop_4_cycles(register uint32_t n)
{
	/* C equivalent: do{ n--; } while(n > 0) */
	__asm__ __volatile__("0: \n" "add.w #-1,%A[n] \n"	/* LSB, 1 cycles */
			     "addc.w #-1,%B[n] \n"	/* MSB, 1 cycles */
			     "jc 0b \n"	/* 2 cycles      */
			     :[n] "+r"((long)n));
}
#endif

void inline delay_usec(unsigned int usec)
{
	uint32_t loops;
	switch (mclk_freq_mhz) {
	case 1:		// 4 cycles == 4 us
		loops = usec >> 2;
		break;
	case 8:		// 4 cycles == 0.5 us
		loops = usec << 1;
		break;
	case 12:		// 4 cycles == 0.33 us
		loops = usec * 3;
		break;
	case 16:		// 4 cycles == 0.25 us
		loops = usec << 2;
		break;
	default:
		loops = usec >> 2; /* arbitrary 1MHz */
		break;
	}
	loop_4_cycles(loops);
}

void delay_ms(unsigned int ms)
{
	unsigned int i;
	uint32_t loops;

	switch (mclk_freq_mhz) {
	case 1:
		loops = 1000 >> 2;
		break;
	case 8:
		loops = 1000 << 1;
		break;
	case 12:
		loops = 1000 * 3;
		break;
	case 16:
		loops = 1000 << 2;
		break;
	default: /* arbitrary value to 1MHZ */
		loops = 1000 >> 2;
		break;
	}

	/* calling delay_usec() introduces an high
	 * clock skew for low mclk frequencies
	 */
	for (i = 0; i < ms; i++) {
		loop_4_cycles(loops);
	}
}

/***************************************************************
 *
 ***************************************************************/
