#include "misc.c"

static unsigned char node_id = 10;
static int last_temperature = 0;

// Protothreads
// ------------
#define NUM_PT 4
static struct pt pt[NUM_PT];

// Application logic
// ------------------

static PT_THREAD(thread_send_status(struct pt *pt)) {
    PT_BEGIN(pt);
    while (1) {
        TIMER_SEND_STATUS = 0;
        PT_WAIT_UNTIL(pt, timer_reached(TIMER_SEND_STATUS, 100)); // 1s
        led_red_on();
        
        unsigned int i;
        for (i = 0; i < PKTLEN; i++) {
          radio_tx_buffer[i] = node_id + i;
        }

        printf("Sending: ");
        for (i = 0; i < PKTLEN; i++) {
          printf("%02X ", radio_tx_buffer[i]);
        }
        printf("\r\n");

        // Radio init
        spi_init();
        cc2500_init();
        cc2500_set_channel(0x83);
        cc2500_utx(radio_tx_buffer, PKTLEN);

        led_red_off();
    }
    PT_END(pt);
}

int main () {
    init();

    // Protothreads init
    int i;
    for (i = 0 ; i < NUM_PT; i++) {
        PT_INIT(&pt[i]);
    }

    while (1) {
        thread_led_green(&pt[0]);
        thread_led_red(&pt[1]);
        thread_send_status(&pt[3]);
    }
}