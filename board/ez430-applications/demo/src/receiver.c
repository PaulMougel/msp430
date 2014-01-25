#include "misc.c"

static unsigned char node_id = 15;

// Protothreads
// ------------
#define NUM_PT 3
static struct pt pt[NUM_PT];

// Application logic
// ------------------
static PT_THREAD(thread_process_msg(struct pt *pt)) {
    PT_BEGIN(pt);

    while (1) {
        PT_WAIT_UNTIL(pt, radio_rx_flag == 1);
        printf("Message received: ");
        unsigned int i;
        for (i = 0; i < PKTLEN; i++) {
          printf("%02X ", radio_rx_buffer[i]);
        }
        printf("\r\n");

        radio_rx_flag = 0;
        cc2500_rx_enter();
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

    printf("-----\r\n");

    // Radio init
    spi_init();
    cc2500_init();
    cc2500_set_channel(0x83);
    cc2500_rx_register_buffer(radio_rx_buffer, PKTLEN);
    cc2500_rx_register_cb(radio_cb);
    cc2500_rx_enter();

    while (1) {
        thread_led_green(&pt[0]);
        thread_led_red(&pt[1]);
        thread_process_msg(&pt[2]);
    }
}