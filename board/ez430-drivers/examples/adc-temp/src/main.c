/**
 *  \file   main.c
 *  \brief  eZ430-RF2500 tutorial, adc + temperature
 *  \author Antoine Fraboulet, Tanguy Risset
 *  \date   2010
 **/

#include <msp430f2274.h>

#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
#include <msp430.h>
#include <iomacros.h>
#include <legacymsp430.h>
#elif defined(__IAR_SYSTEMS_ICC__)
/* This is the IAR compiler */
//#include <io430.h>
#endif

#include <stdio.h>

#include "leds.h"
#include "clock.h"
#include "watchdog.h"
#include "timer.h"
#include "uart.h"
#include "adc10.h"
#include "lpm_compat.h"
#include "flash.h"

#define ID 1
#define COEFF_1_ADDR INFOD_START+1
#define COEFF_2_ADDR INFOD_START+3

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

int main(void)
{
  int temp;

  volatile int adc_coeff_1, adc_coeff_2 = 0;
  watchdog_stop();
  
  set_mcu_speed_dco_mclk_16MHz_smclk_8MHz();
  leds_init();
  led_red_on();

  timerA_init();
  timerA_set_wakeup(1);
  timerA_start_milliseconds(1000);
  
  uart_init(UART_9600_SMCLK_8MHZ);
  printf("adc test application: temperature\n\r");

  adc10_start();

  /* We search for calibration data in flash */
  if (*((char *)COEFF_1_ADDR) !=0xFFFF){
		/* Get calibration data from flash */
		char *foo = (char *) &adc_coeff_1;
		foo[0] = *((char *)COEFF_1_ADDR+1);
		foo[1] = *((char *)COEFF_1_ADDR);

		printf("Using a custom coeff : %d\n", adc_coeff_1);
		adc10_calibrate(adc_coeff_1, 0);
	}
	if (*((char *)COEFF_2_ADDR) !=0xFFFF){
		/* Get calibration data from flash */
		char *foo = (char *) &adc_coeff_2;
		foo[0] = *((char *)COEFF_2_ADDR+1);
		foo[1] = *((char *)COEFF_2_ADDR);
		adc10_calibrate(0, adc_coeff_2);
	}

  __enable_interrupt();

  led_green_on();
  
  for(;;)
    {
      LPM(1);
      led_green_switch();
      temp = adc10_sample_temp();
      printf("temp: %d,%d°C\n\r",temp/10,temp%10);
    }
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
