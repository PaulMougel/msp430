/**
 *  \file   main.c
 *  \brief  eZ430-RF2500 tutorial, adc10
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/

#include <msp430x22x4.h>
#include <msp430.h>
#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
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

#define ID 1

int main(void)
{
    watchdog_stop();

    set_mcu_speed_dco_mclk_8MHz_smclk_1MHz();
    leds_init();

    led_red_on();
    for(;;)
    {
        delay_ms(1000);
        led_red_switch();
        led_green_switch();
    }
}
