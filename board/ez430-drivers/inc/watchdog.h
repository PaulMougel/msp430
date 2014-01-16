#ifndef MSP430_WATCHDOG_H
#define MSP430_WATCHDOG_H

/* save watchdog configuration and stop it */
void watchdog_stop();
/* restore the last saved watchdog configuration */
void watchdog_restore();

#endif
