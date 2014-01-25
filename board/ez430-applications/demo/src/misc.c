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

// Timer A
// -------
// Each TIMER_PERIOD_MS all timers are incremented
// Used to update multi3le logic timers using only one hardware timer
#define NUM_TIMERS 5
static uint16_t timer[NUM_TIMERS];
#define TIMER_PERIOD_MS 10
#define TIMER_LED_GREEN_ON timer[0]
#define TIMER_LED_RED_ON timer[1]
#define TIMER_GET_TEMPERATURE timer[2]
#define TIMER_PRINT_STATUS timer[3]
#define TIMER_SEND_STATUS timer[4]

void timerA_cb() {
    int i;
    for (i = 0; i < NUM_TIMERS; i++) {
        if (timer[i] != UINT_MAX) timer[i]++;
    }
}

int timer_reached(uint16_t timer, uint16_t count) {
    return (timer >= count);
}

// LEDs
// ----
static int led_green_duration;
static int led_green_flag;
void led_green_blink (int duration) {
    // duration = #timer A ticks
    led_green_flag = 1;
    led_green_duration = duration;
}

static PT_THREAD(thread_led_green(struct pt *pt)) {
    PT_BEGIN(pt);
    while (1) {
        PT_WAIT_UNTIL(pt, led_green_flag == 1);
        led_green_on();
        TIMER_LED_GREEN_ON = 0;
        PT_WAIT_UNTIL(pt, timer_reached(TIMER_LED_GREEN_ON, led_green_duration));
        led_green_off();
        led_green_flag = 0;
    }
    PT_END(pt);
}

static int led_red_duration;
static int led_red_flag;
void led_red_blink (int duration) {
    // duration = #timer A ticks
    led_red_flag = 1;
    led_red_duration = duration;
}

static PT_THREAD(thread_led_red(struct pt *pt)) {
    PT_BEGIN(pt);
    while (1) {
        PT_WAIT_UNTIL(pt, led_red_flag == 1);
        led_red_on();
        TIMER_LED_RED_ON = 0;
        PT_WAIT_UNTIL(pt, timer_reached(TIMER_LED_RED_ON, led_red_duration));
        led_red_off();
        led_red_flag = 0;
    }
    PT_END(pt);
}

// Radio
// -----
#define PKTLEN 60
static unsigned char radio_rx_buffer[PKTLEN];
static unsigned char radio_tx_buffer[PKTLEN];
static int radio_rx_flag = 0;

static void radio_cb(uint8_t *buffer, int size, int8_t rssi) {
    printf("Message received, checking CRC...");

    if (size == -ERXBADCRC) {
        led_red_blink(50);
        printf("Bad CRC.\r\n");
        cc2500_rx_enter();
        return;
    }

    led_green_blink(50);
    radio_rx_flag = 1;
    printf("Good CRC\r\n");

    cc2500_rx_enter();
}

static void init_message() {
    unsigned int i;
    for (i = 0; i < PKTLEN; i++) {
        radio_tx_buffer[i] = 0x00;
    }
}

// Init
// ----
static void init () {
    watchdog_stop();

    // Hardware init
    set_mcu_speed_dco_mclk_16MHz_smclk_8MHz();
    uart_init(UART_9600_SMCLK_8MHZ);
    adc10_start();
    leds_init();
    led_red_off();
    led_green_off();
    
    timerA_init();
    timerA_register_cb(&timerA_cb);
    timerA_start_milliseconds(TIMER_PERIOD_MS);

    led_green_blink(100);
    led_red_blink(100);

    __enable_interrupt();
}