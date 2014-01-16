#include <msp430f2274.h>

#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
#include <msp430.h>
#include <iomacros.h>
#elif defined(__IAR_SYSTEMS_ICC__)
/* This is the IAR compiler */
//#include <io430.h>
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

#define DBG_PRINTF printf

// Timer handling
// --------------
// Each TIMER_PERIOD_MS all timers are incremented
// Used by protothreads to launch tasks
#define NUM_TIMERS 4
static uint16_t timer[NUM_TIMERS];
#define TIMER_PERIOD_MS 10
#define TIMER_GET_TEMPERATURE timer[0]
#define TIMER_PRINT_TEMPERATURE timer[1]
#define TIMER_SEND_TEMPERATURE timer[2]
#define TIMER_RESET_CC2500 timer[3]

void timerA_cb() {
    int i;
    for(i = 0; i < NUM_TIMERS; i++) {
        if(timer[i] != UINT_MAX) timer[i]++;
    }
}

int timer_reached(uint16_t timer, uint16_t count) {
    return (timer >= count);
}

// Protothreads
// ------------
#define NUM_PT 5
static struct pt pt[NUM_PT];

// Radio
// ----- 
#define PKTLEN 7
#define MAX_HOPS 3
#define MSG_BYTE_TYPE 0U
#define MSG_BYTE_HOPS 1U
#define MSG_BYTE_SRC_ROUTE 2U
#define MSG_BYTE_CONTENT (MAX_HOPS + 2)
#define MSG_TYPE_ID_REQUEST 0x00
#define MSG_TYPE_ID_REPLY 0x01
#define MSG_TYPE_TEMPERATURE 0x02
static uint8_t radio_tx_buffer[PKTLEN];
static unsigned char node_id = 42;
static uint8_t radio_rx_buffer[PKTLEN];
static int radio_rx_flag = 0;

static void init_message() {
    unsigned int i;
    for (i = 0; i < PKTLEN; i++)
        radio_tx_buffer[i] = 0x00;
    radio_tx_buffer[MSG_BYTE_HOPS] = 0x01;
    radio_tx_buffer[MSG_BYTE_SRC_ROUTE] = node_id;
}

static void radio_send_message() {
    led_red_on();
    cc2500_utx(radio_tx_buffer, PKTLEN);
    led_red_off();
    cc2500_rx_enter();
}

void radio_rx_cb(uint8_t *buffer, int size, int8_t rssi) {
    led_green_on();
    printf("msg received\r\n");
    switch (size)
    {
        case 0:
            DBG_PRINTF("msg size 0\r\n");
            break;
        case -EEMPTY:
            DBG_PRINTF("msg emptyr\r\n");
            break;
        case -ERXFLOW:
            DBG_PRINTF("msg rx overflow\r\n");
            break;
        case -ERXBADCRC:
            DBG_PRINTF("msg rx bad CRC\r\n");
            break;
        case -ETXFLOW:
            DBG_PRINTF("msg tx overflow\r\n");
            break;
        default:
            if (size > 0)
            {
                /* register next available buffer in pool */
                /* post event to application */
                DBG_PRINTF("rssi %d\r\n", rssi);

                memcpy(radio_rx_buffer, buffer, PKTLEN);
                //FIXME what if radio_rx_flag == 1 already?
                //last_rssi = rssi;
                radio_rx_flag = 1;
            }
            else
            {
                /* packet error, drop */
                DBG_PRINTF("msg packet error size=%d\r\n",size);
            }
            break;
    }

    cc2500_rx_enter();
    led_green_off();
}

// App. logic
// ----------
static int last_temperature = 0;
static PT_THREAD(thread_get_last_temperature(struct pt *pt)) {
    // sets global variable last_temperature
    PT_BEGIN(pt);

    while (1) {
        TIMER_GET_TEMPERATURE = 0;
        PT_WAIT_UNTIL(pt, timer_reached(TIMER_GET_TEMPERATURE, 100)); // 1s
        last_temperature = adc10_sample_temp();
    }
    PT_END(pt);
}

static PT_THREAD(thread_print_temperature(struct pt *pt)) {
    PT_BEGIN(pt);

    while (1) {
        TIMER_PRINT_TEMPERATURE = 0;
        PT_WAIT_UNTIL(pt, timer_reached(TIMER_PRINT_TEMPERATURE, 100)); // 1s

        printf("temp,%d\r\n", last_temperature);
    }
    PT_END(pt);
}

static PT_THREAD(thread_send_temperature(struct pt *pt)) {
    PT_BEGIN(pt);

    while (1) {
        TIMER_SEND_TEMPERATURE = 0;

        PT_WAIT_UNTIL(pt, timer_reached(TIMER_SEND_TEMPERATURE, 100)); // 100 ticks = 1second

        init_message();
        radio_tx_buffer[MSG_BYTE_TYPE] = MSG_TYPE_TEMPERATURE;
        // Convert to network order as msp430 is little endian
        int temperature = last_temperature;
        char *pt = (char *) &temperature;
        radio_tx_buffer[MSG_BYTE_CONTENT] = pt[1];
        radio_tx_buffer[MSG_BYTE_CONTENT + 1] = pt[0];
        radio_send_message();
    }
    PT_END(pt);
}

static PT_THREAD(thread_process_message(struct pt *pt)) {
    PT_BEGIN(pt);

    while (1) {
        PT_WAIT_UNTIL(pt, radio_rx_flag == 1);
        radio_rx_flag = 0;
        printf("RECEIVED MESSAGE\r\n");
    }
    PT_END(pt);
}

static PT_THREAD(thread_reset_cc2500(struct pt *pt)) {
    PT_BEGIN(pt);

    while (1) {
        TIMER_RESET_CC2500 = 0;
        PT_WAIT_UNTIL(pt, timer_reached(TIMER_RESET_CC2500, 80)); // 80 ticks = 0.8 second
        printf("About to reset cc2500\r\n");
        cc2500_idle();
        cc2500_rx_enter();
    }
    PT_END(pt);
}

int main (void) {
    watchdog_stop();

    // Hardware init
    set_mcu_speed_dco_mclk_16MHz_smclk_8MHz();

    timerA_init();
    timerA_register_cb(&timerA_cb);
    timerA_start_milliseconds(TIMER_PERIOD_MS);

    uart_init(UART_9600_SMCLK_8MHZ);

    adc10_start();

    // Radio init
    spi_init();
    cc2500_init();
    cc2500_set_channel(0x83);
    cc2500_rx_register_buffer(radio_rx_buffer, PKTLEN);
    cc2500_rx_register_cb(radio_rx_cb);
    cc2500_rx_enter();
    radio_rx_flag = 0;

    __enable_interrupt();

    leds_init();
    led_green_off();
    led_red_off();

    // Protothreads init
    int i;
    for (i = 0 ; i < NUM_PT; i++) {
        PT_INIT(&pt[i]);
    }

    printf("Hello, world.\r\n");

    
    while (1) {
        thread_get_last_temperature(&pt[0]);
        thread_print_temperature(&pt[1]);
        thread_send_temperature(&pt[2]);
        thread_process_message(&pt[3]);
        // thread_reset_cc2500(&pt[4]); // not necessary?
    }
}
