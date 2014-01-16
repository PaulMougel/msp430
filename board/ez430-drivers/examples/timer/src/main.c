/**
 *  \file   main.c
 *  \brief  eZ430-RF2500 tutorial, timerA
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/

#include <msp430.h>
#include <legacymsp430.h>
#include <iomacros.h>
#include <stdio.h>

#include "leds.h"
#include "clock.h"
#include "timer.h"
#include "watchdog.h"

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void green_led_cb(void)
{
  static int b = 0;

  if (b << 16 == 1) {
		led_green_switch();
		b = 0;
	} else {
		b = 1;
	}
}

void red_led_cb(void)
{
  led_red_switch();
}



/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

int main(void)
{
  watchdog_stop();

  /* leds */
  leds_init();
  leds_off();
  
  /* switch to running freq mode            */
  set_mcu_speed_dco_mclk_8MHz_smclk_4MHz ();

  /* timer settings */
  timerB_init();
  timerB_register_cb(green_led_cb);
  /* uncomment to blink red led */
  timerB_set_wakeup(1); 
  /* ACLK ~ 12kHz, 10000 ticks = 0.83s */
  timerB_start_ticks(10000); 

  /*timerA_init();
  timerA_register_cb(red_led_cb);
  timerA_start_ticks(10000 / 2);*/ 

  /* enable interrupt */
  eint();
  
  while (1)
    {
      LPM1;
      led_red_switch();
    }
  return 0;
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************* */
