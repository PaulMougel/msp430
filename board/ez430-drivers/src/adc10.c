/**
 *  \file   adc10.h
 *  \brief  eZ430-RF2500 tutorial, adc10
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


#include <stdint.h>
#include <stdlib.h>

#include "isr_compat.h"
#include "io_compat.h"
#include "lpm_compat.h"
#include "clock.h"
#include "adc10.h"
#include "flash.h"


static volatile int coeff_1;
static volatile int coeff_2;

/* **************************************************
 * 
 * **************************************************/

void adc10_start(void)
{
		coeff_1 = 4230;
		coeff_2 = 2780;
}

void adc10_calibrate(uint16_t coeff1, uint16_t coeff2) {
	if (coeff1 !=0){
		coeff_1 = coeff1;
	}
	if (coeff2 !=0){
		coeff_2 = coeff2;
	}
}


/* **************************************************
 * 
 * **************************************************/

void adc10_stop(void)
{
	//  ADC10CTL0  = 0;
	ADC10CTL0 &= ~(REFON + ADC10ON);	// turn off A/D to save power
}

/* **************************************************
 * 
 * **************************************************/

ISR(ADC10, adc10irq)
{
	LPM_OFF_ON_EXIT;
}

#define TEMPOFFSET_ 0x10F4
SFRB(TEMPOFFSET, TEMPOFFSET_);

int adc10_sample_temp(void)
{
	volatile long result;
	int degC;

	ADC10CTL1 = INCH_10 + ADC10DIV_4;	// Temp Sensor ADC10CLK/3
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE + ADC10SR;
	loop_4_cycles(60);	// delay to allow reference to settle (240 cycles)
	ADC10CTL0 |= ENC + ADC10SC;	// Sampling and conversion start

	LPM(0);

	result = ADC10MEM;
	ADC10CTL0 &= ~ENC;
	ADC10CTL0 &= ~(REFON + ADC10ON);	// turn off A/D to save power

	// oC = ((A10/1024)*1500mV)-986mV)*1/3.55mV = A10*423/1024 - 278
	// the temperature is transmitted as an integer where 32.1 = 321
	// hence 4230 instead of 423
	// VTEMP=0.00355(TEMPC)+0.986

	degC = ((result * coeff_1) / 1024) - coeff_2;

	/*
	   if( TEMPOFFSET != 0xFFFF )
	   degC += TEMPOFFSET; 
	 */
	return degC;
}

/* **************************************************
 * 
 * **************************************************/

int adc10_sample_avcc(void)
{
	volatile long result;
	unsigned int volt;

	ADC10CTL1 = INCH_11;	// AVcc/2
	ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE + REF2_5V;

	loop_4_cycles(60);	// delay to allow reference to settle (240 cycles)

	ADC10CTL0 |= ENC + ADC10SC;	// Sampling and conversion start

	LPM(0);

	result = ADC10MEM;
	ADC10CTL0 &= ~ENC;
	ADC10CTL0 &= ~(REFON + ADC10ON);	// turn off A/D to save power

	volt = (result * 25) / 512;
	return volt;
}

/* **************************************************
 * 
 * **************************************************/
