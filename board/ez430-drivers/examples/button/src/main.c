/**
 *  \file   main.c
 *  \brief  eZ430-RF2500 tutorial, button
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
#include "button.h"
#include "uart.h"

#define ID 1



void button_pressed_cb()
{
  printf("-- button pressed\n"); 
  led_green_switch();
}


int main(void)
{
    watchdog_stop();
    
    /* button init */
    button_init();
    button_register_cb(button_pressed_cb);

    set_mcu_speed_dco_mclk_8MHz_smclk_1MHz();
    uart_init(UART_9600_SMCLK_8MHZ);
    leds_init();

    led_red_off();
    led_green_off();
    
    button_enable_interrupt();
     __enable_interrupt();
     
    for(;;)
    {
        delay_ms(1000);
        led_red_switch();
    }
}
