/**
 *  \file   cc2500.h
 *  \brief  eZ430-RF2500 tutorial, cc2500
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/

#ifndef CC2500_H
#define CC2500_H

#include <stdint.h>

/************************************************/
/* Init                                         */
/************************************************/

void cc2500_init(void);
void cc2500_reset(void);

/************************************************/
/* Configuration                                */
/************************************************/

typedef uint8_t BYTE;

// a configuration consumes 36 bytes of memory

typedef struct RF_SETTINGS_t {
	BYTE fsctrl1;		// Frequency synthesizer control.
	BYTE fsctrl0;		// Frequency synthesizer control.
	BYTE freq2;		// Frequency control word, high byte.
	BYTE freq1;		// Frequency control word, middle byte.
	BYTE freq0;		// Frequency control word, low byte.
	BYTE mdmcfg4;		// Modem configuration.
	BYTE mdmcfg3;		// Modem configuration.
	BYTE mdmcfg2;		// Modem configuration.
	BYTE mdmcfg1;		// Modem configuration.
	BYTE mdmcfg0;		// Modem configuration.
	BYTE channr;		// Channel number.
	BYTE deviatn;		// Modem deviation setting (when FSK modulation is enabled).
	BYTE frend1;		// Front end RX configuration.
	BYTE frend0;		// Front end TX configuration.
	BYTE mcsm0;		// Main Radio Control State Machine configuration.
	BYTE foccfg;		// Frequency Offset Compensation Configuration.
	BYTE bscfg;		// Bit synchronization Configuration.
	BYTE agcctrl2;		// AGC control.
	BYTE agcctrl1;		// AGC control.
	BYTE agcctrl0;		// AGC control.
	BYTE fscal3;		// Frequency synthesizer calibration.
	BYTE fscal2;		// Frequency synthesizer calibration.
	BYTE fscal1;		// Frequency synthesizer calibration.
	BYTE fscal0;		// Frequency synthesizer calibration.
	BYTE fstest;		// Frequency synthesizer calibration.
	BYTE test2;		// Various test settings.
	BYTE test1;		// Various test settings.
	BYTE test0;		// Various test settings.
	BYTE fifothr;		// RXFIFO and TXFIFO thresholds.
	BYTE iocfg2;		// GDO2 output pin configuration.
	BYTE iocfg0d;		// GDO0 output pin configuration. Refer to SmartRF® Studio User Manual 
	// for detailed pseudo register explanation.
	BYTE pktctrl1;		// Packet automation control.
	BYTE pktctrl0;		// Packet automation control.
	BYTE addr;		// Device address.
	BYTE pktlen;		// Packet length.
} RF_SETTINGS;

void cc2500_configure(RF_SETTINGS const *cfg);

/************************************************/
/* IRQ Handler                                  */
/************************************************/

#define CC2500_GDO0   0x01
#define CC2500_GDO2   0x02

#define GDO0_SHIFT    6		/* P2.6 - Input: General Digital Output 0 (GDOO)   */
#define GDO2_SHIFT    7		/* P2.7 - Input: General Digital Output 2 (GDO2)   */
#define GDO0_MASK     (1 << GDO0_SHIFT)
#define GDO2_MASK     (1 << GDO2_SHIFT)

void cc2500_gdox_signal_handler(uint8_t mask);

/************************************************/
/* Rx/Tx                                        */
/************************************************/

/* Tx size < 63B packet limitation  */
void cc2500_utx(const char *buffer, const uint8_t length);

#define EEMPTY        1
#define ERXFLOW       2
#define ERXBADCRC     3
#define ETXFLOW       4

typedef void (*cc2500_cb_t) (uint8_t * buffer, int size, int8_t rssi);

void cc2500_rx_register_cb(cc2500_cb_t);
void cc2500_rx_register_buffer(uint8_t * buffer, uint8_t length);

/************************************************/
/* Major modes                                  */
/************************************************/

void cc2500_calibrate(void);	/* puts the CC2500 in idle mode = tx/rx ready */
void cc2500_idle(void);		/* puts the CC2500 in idle mode = tx/rx ready */
void cc2500_sleep(void);	/* enter sleep mode */
void cc2500_wakeup(void);
void cc2500_rx_enter(void);	/* Start Rx mode */

int cc2500_cca(void);		/* 0: busy, 1: clear */
uint8_t cc2500_get_rssi(void);
void cc2500_set_channel(uint8_t chan);

/************************************************/
/*                                              */
/************************************************/

#endif
