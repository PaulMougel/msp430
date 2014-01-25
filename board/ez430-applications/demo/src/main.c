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

// Protothreads
// ------------
#define NUM_PT 4
static struct pt pt[NUM_PT];

// Timer A
// -------
// Each TIMER_PERIOD_MS all timers are incremented
// Used to update multi3le logic timers using only one hardware timer
#define NUM_TIMERS 4
static uint16_t timer[NUM_TIMERS];
#define TIMER_PERIOD_MS 10
#define TIMER_LED_GREEN_ON timer[0]
#define TIMER_LED_RED_ON timer[1]
#define TIMER_GET_TEMPERATURE timer[2]
#define TIMER_PRINT_STATUS timer[3]

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
#define PKTLEN 7
static char radio_tx_buffer[PKTLEN];
void radio_cb(uint8_t *buffer, int size, int8_t rssi) {
    led_green_blink(100);
    // printf("Packet received.\r\n");
    cc2500_rx_enter();
}

// Application logic
// ------------------
static int last_temperature = 0;
static PT_THREAD(thread_get_temperature(struct pt *pt)) {
    // sets global variable last_temperature
    PT_BEGIN(pt);

    while (1) {
        TIMER_GET_TEMPERATURE = 0;
        PT_WAIT_UNTIL(pt, timer_reached(TIMER_GET_TEMPERATURE, 100)); // 1s
        last_temperature = adc10_sample_temp();
    }
    PT_END(pt);
}

static PT_THREAD(thread_print_status(struct pt *pt)) {
    PT_BEGIN(pt);
    while (1) {
        TIMER_PRINT_STATUS = 0;
        PT_WAIT_UNTIL(pt, timer_reached(TIMER_PRINT_STATUS, 100)); // 1s
        printf("%d,%d,%d,%d\r\n", 0, last_temperature, 0, 0);
    }
    PT_END(pt);
}

int main () {
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

    // Radio init
    spi_init();
    cc2500_init();
    cc2500_rx_register_buffer(radio_tx_buffer, PKTLEN);
    cc2500_rx_register_cb(radio_cb);
    cc2500_rx_enter();


    led_green_blink(100);
    led_red_blink(100);

    __enable_interrupt();

    // Protothreads init
    int i;
    for (i = 0 ; i < NUM_PT; i++) {
        PT_INIT(&pt[i]);
    }

    while (1) {
        thread_led_green(&pt[0]);
        thread_led_red(&pt[1]);
        thread_get_temperature(&pt[2]);
        thread_print_status(&pt[3]);
    }
}