/**
 *  \file   main.c
 *  \brief  eZ430-RF2500 tutorial, adc10
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
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
#include "flash.h"
#include "uart.h"

#define ID 1
#define ID_LOCATION INFOD_START

int main(void)
{
    watchdog_stop();

    set_mcu_speed_dco_mclk_16MHz_smclk_8MHz();
    uart_init(UART_9600_SMCLK_8MHZ);
    leds_init();

    unsigned char * settings = (unsigned char *) (ID_LOCATION);

    printf("id stored before=%d\r\n", *settings);

    printf("id will be changed\r\n");
    if(flash_write_byte(settings, ID) == 0)
    {
        printf("written byte %d\r\n", ID);
    } else {
        printf("erasing segment...\r\n");
        flash_erase_info_memory();
        if(flash_write_byte(settings, ID) == 0)
        {
            printf("segment erased...\r\n");
            printf("written byte %d\r\n", ID);
        }
        else
        {
            printf("segment erase failed\r\n");
        }
    }

    printf("id stored after=%d\r\n", *settings);

    led_red_switch();

    for(;;);
}
