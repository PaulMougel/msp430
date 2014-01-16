/**
 *  \file   main.c
 *  \brief  eZ430-RF2500 tutorial, watchdog
 *  \author Julien Carpentier
 *  \date   2011
 **/

#include <msp430x22x4.h>

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


int timer;

void green_led_cb(void)
{
  if (timer > 0)
  {  
    printf("-- timer : %i \n",timer);
    led_green_switch();
    timer--;
  }
  else
  {
    printf("-- REBOOT !!\n");
    watchdog_restore();
  }
}

int main(void)
{
  
    set_mcu_speed_dco_mclk_8MHz_smclk_4MHz ();
  
    /* leds init */
    leds_init();
    leds_off();

    /* timer init */
    timer=5;
    timerB_init();
    timerB_register_cb(green_led_cb);
    timerB_start_ticks(10000);

  
    /* uart init*/
    uart_init(UART_9600_SMCLK_8MHZ);
    printf("--init OK-- \n");
    
    
    /* enable interrupt */
    eint();
  
    while (1)
    {
      LPM1;
      led_red_switch();
    }
    
    
    return 0;
}

