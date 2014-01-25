#include <msp430f2274.h>
#if defined(__GNUC__) && defined(__MSP430__)
    #include <msp430.h>
    #include <iomacros.h>
#endif

#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "isr_compat.h"
#include "leds.h"
#include "clock.h"
#include "timer.h"
#include "button.h"
#include "uart.h"
#include "adc10.h"
#include "spi.h"
#include "cc2500.h"
#include "flash.h"
#include "watchdog.h"
#include "pt.h"

#define PKTLEN 7
static char radio_tx_buffer[PKTLEN];
void radio_cb(uint8_t *buffer, int size, int8_t rssi) {
    //led_green_blink(500);
    printf("Packet received.\r\n");
    cc2500_rx_enter();
}

void main () {
    watchdog_stop();
    set_mcu_speed_dco_mclk_16MHz_smclk_8MHz();
    uart_init(UART_9600_SMCLK_8MHZ);
    leds_init();
    led_red_off();
    led_green_off();

    spi_init();
    cc2500_init();
    cc2500_rx_register_buffer(radio_tx_buffer, PKTLEN);
    cc2500_rx_register_cb(radio_cb);
    cc2500_rx_enter();

    __enable_interrupt();

    printf("Hello world.\r\n");

    for (;;);
}