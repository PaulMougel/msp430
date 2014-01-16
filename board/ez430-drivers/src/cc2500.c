/**
 *  \file   cc2500.h
 *  \brief  eZ430-RF2500 tutorial, cc2500
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/

#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
#include <msp430.h>
#include <legacymsp430.h>
#elif defined(__IAR_SYSTEMS_ICC__)
/* This is the IAR compiler */
#include <io430.h>
#endif

#include "isr_compat.h"
#include "lpm_compat.h"
#include "debug_compat.h"
#include "leds.h"
#include "spi.h"
#include "clock.h"
#include "cc2500.h"

/**************************************/
/* C2500 Registers: table 36, page 60 */
/**************************************/

#define CC2500_REG_IOCFG2                       0x00
#define CC2500_REG_IOCFG1                       0x01
#define CC2500_REG_IOCFG0                       0x02
#define CC2500_REG_FIFOTHR                      0x03
#define CC2500_REG_SYNC1                        0x04	/* */
#define CC2500_REG_SYNC0                        0x05
#define CC2500_REG_PKTLEN                       0x06
#define CC2500_REG_PKTCTRL1                     0x07
#define CC2500_REG_PKTCTRL0                     0x08
#define CC2500_REG_ADDR                         0x09
#define CC2500_REG_CHANNR                       0x0A
#define CC2500_REG_FSCTRL1                      0x0B
#define CC2500_REG_FSCTRL0                      0x0C
#define CC2500_REG_FREQ2                        0x0D
#define CC2500_REG_FREQ1                        0x0E
#define CC2500_REG_FREQ0                        0x0F
#define CC2500_REG_MDMCFG4                      0x10
#define CC2500_REG_MDMCFG3                      0x11
#define CC2500_REG_MDMCFG2                      0x12
#define CC2500_REG_MDMCFG1                      0x13
#define CC2500_REG_MDMCFG0                      0x14
#define CC2500_REG_DEVIATN                      0x15
#define CC2500_REG_MCSM2                        0x16
#define CC2500_REG_MCSM1                        0x17
#define CC2500_REG_MCSM0                        0x18
#define CC2500_REG_FOCCFG                       0x19
#define CC2500_REG_BSCFG                        0x1A
#define CC2500_REG_AGCCTRL2                     0x1B
#define CC2500_REG_AGCCTRL1                     0x1C
#define CC2500_REG_AGCCTRL0                     0x1D
#define CC2500_REG_WOREVT1                      0x1E
#define CC2500_REG_WOREVT0                      0x1F
#define CC2500_REG_WORCTRL                      0x20
#define CC2500_REG_FREND1                       0x21
#define CC2500_REG_FREND0                       0x22
#define CC2500_REG_FSCAL3                       0x23
#define CC2500_REG_FSCAL2                       0x24
#define CC2500_REG_FSCAL1                       0x25
#define CC2500_REG_FSCAL0                       0x26
#define CC2500_REG_RCCTRL1                      0x27
#define CC2500_REG_RCCTRL0                      0x28
/* Test registers */
#define CC2500_REG_FSTEST                       0x29
#define CC2500_REG_PTEST                        0x2A
#define CC2500_REG_AGCTEST                      0x2B
#define CC2500_REG_TEST2                        0x2C
#define CC2500_REG_TEST1                        0x2D
#define CC2500_REG_TEST0                        0x2E
/* Read only registers, access with |0xc0 */
#define CC2500_REG_PARTNUM                      0x30	/* 0xF0 */
#define CC2500_REG_VERSION                      0x31	/* 0xF1 */
#define CC2500_REG_FREQEST                      0x32
#define CC2500_REG_LQI                          0x33
#define CC2500_REG_RSSI                         0x34
#define CC2500_REG_MARCSTATE                    0x35
#define CC2500_REG_WORTIME1                     0x36
#define CC2500_REG_WORTIME0                     0x37
#define CC2500_REG_PKTSTATUS                    0x38
#define CC2500_REG_VCO_VC_DAC                   0x39
#define CC2500_REG_TXBYTES                      0x3A
#define CC2500_REG_RXBYTES                      0x3B
#define CC2500_REG_RCCTRL1_STATUS               0x3C
#define CC2500_REG_RCCTRL0_STATUS               0x3D
#define CC2500_PATABLE_ADDR                     0x3E
#define CC2500_DATA_FIFO_ADDR                   0x3F

/***********************************************/
/* CC2500 RAM & register Access (table 37, 61) */
/***********************************************/

#define CC2500_REG_ACCESS_OP(x)                 (x & 0x80)
#define CC2500_REG_ACCESS_OP_READ               0x80
#define CC2500_REG_ACCESS_OP_WRITE              0x00
#define CC2500_REG_ACCESS_TYPE(x)               (x & 0x40)
#define CC2500_REG_ACCESS_BURST                 0x40
#define CC2500_REG_ACCESS_NOBURST               0x00
#define CC2500_REG_ACCESS_ADDRESS(x)            (x & 0x3F)

/*****************************/
/* STROBE: table 34, page 58 */
/*****************************/

#define CC2500_STROBE_SRES                      0x30	/* reset                                       */
#define CC2500_STROBE_SFSTXON                   0x31	/* enable and calibrate                        */
#define CC2500_STROBE_SXOFF                     0x32	/* crystall off                                */
#define CC2500_STROBE_SCAL                      0x33	/* calibrate                                   */
#define CC2500_STROBE_SRX                       0x34	/* enable rx                                   */
#define CC2500_STROBE_STX                       0x35	/* enable tx                                   */
#define CC2500_STROBE_SIDLE                     0x36	/* go idle                                     */
							  /* #define CC2500_STROBE_SAFC                      0x37*//* AFC adjustment, removed Datasheet 1.1  */
#define CC2500_STROBE_SWOR                      0x38	/* wake on radio                               */
#define CC2500_STROBE_SPWD                      0x39	/* power down                                  */
#define CC2500_STROBE_SFRX                      0x3A	/* flush Rx fifo                               */
#define CC2500_STROBE_SFTX                      0x3B	/* flush Tx fifo                               */
#define CC2500_STROBE_SWORRST                   0x3C	/* Reset real time clock to Event1 value.      */
#define CC2500_STROBE_SNOP                      0x3D	/* no operation                                */

/***************************/
/* GDOx: table 33, page 54 */
/***************************/

#define CC2500_GDOx_RX_FIFO                     0x00	/* assert above threshold, deassert when below        */
#define CC2500_GDOx_RX_FIFO_EOP                 0x01	/* assert above threshold or EOP                      */
#define CC2500_GDOx_TX_FIFO                     0x02	/* assert above threshold, deassert below thr         */
#define CC2500_GDOx_TX_THR_FULL                 0x03	/* asserts when TX FIFO is full. De-asserts when      */
						     /* the TX FIFO is drained below TXFIFO_THR.           */
#define CC2500_GDOx_RX_OVER                     0x04	/* asserts when RX overflow, deassert when flushed    */
#define CC2500_GDOx_TX_UNDER                    0x05	/* asserts when RX underflow, deassert when flushed   */
#define CC2500_GDOx_SYNC_WORD                   0x06	/* assert SYNC sent/recv, deasserts on EOP            */
						     /* In RX, de-assert on overflow or bad address        */
						     /* In TX, de-assert on underflow                      */
#define CC2500_GDOx_RX_OK                       0x07	/* assert when RX PKT with CRC ok, de-assert on 1byte */
						     /* read from RX Fifo                                  */
#define CC2500_GDOx_PREAMB_OK                   0x08	/* assert when preamble quality reached : PQI/PQT ok  */
#define CC2500_GDOx_CCA                         0x09	/* Clear channel assessment. High when RSSI level is  */
						     /* below threshold (dependent on the current CCA_MODE) */

#define CC2500_GDOx_CHIP_RDY                    0x29	/* CHIP_RDY     */
#define CC2500_GDOx_XOSC_STABLE                 0x2B	/* XOSC_STABLE  */

/* ======================= */
/* cc2500 variables        */
/* ======================= */

volatile cc2500_cb_t radio_rx_cb;

/* pin configuration for interrupt handler */
volatile uint8_t cc2500_status_register;
volatile uint8_t cc2500_gdo2_cfg;
volatile uint8_t cc2500_gdo0_cfg;
uint8_t *cc2500_rx_packet;	/* data rx pkt  */
volatile uint8_t cc2500_rx_offset = 0x00;	/* data pkt ptr */
volatile uint8_t cc2500_rx_length = 0x00;	/* */

/**********************
 * Macros
 **********************/

/* Selector         =  0:GPIO     1:peripheral */
/* Direction        =  0:input    1:output     */
/* Interrupt enable =  0:disable  1:enable     */
/* Edge select      =  0:L to H   1:H to L     */
/* Resistor Enable  =  0:no       1:yes        */
/*
 * Port 2
 * ======
 *   2.7 : GDO2
 *   2.6 : GDO0
 */

#define SIVF static inline void
#define SIIF static inline int

#define GDOPORT(x) P2##x

SIVF CC2500_HW_GDO0_IRQ_ON_ASSERT(void)
{
	GDOPORT(IES) &= ~(GDO0_MASK);
}

SIVF CC2500_HW_GDO2_IRQ_ON_ASSERT(void)
{
	GDOPORT(IES) &= ~(GDO2_MASK);
}

SIVF CC2500_HW_GDO0_IRQ_ON_DEASSERT(void)
{
	GDOPORT(IES) |= GDO0_MASK;
}

SIVF CC2500_HW_GDO2_IRQ_ON_DEASSERT(void)
{
	GDOPORT(IES) |= GDO2_MASK;
}

SIVF CC2500_HW_GDO0_DINT(void)
{
	GDOPORT(IE) &= ~(GDO0_MASK);
}

SIVF CC2500_HW_GDO2_DINT(void)
{
	GDOPORT(IE) &= ~(GDO2_MASK);
}

SIVF CC2500_HW_GDO0_EINT(void)
{
	GDOPORT(IE) |= GDO0_MASK;
}

SIVF CC2500_HW_GDO2_EINT(void)
{
	GDOPORT(IE) |= GDO2_MASK;
}

SIVF CC2500_HW_GDO0_CLEAR_FLAG(void)
{
	GDOPORT(IFG) &= ~(GDO0_MASK);
}

SIVF CC2500_HW_GDO2_CLEAR_FLAG(void)
{
	GDOPORT(IFG) &= ~(GDO2_MASK);
}

SIIF CC2500_HW_GDO0_READ(void)
{
	return (GDOPORT(IN) >> GDO0_SHIFT) & 0x1;
}

SIIF CC2500_HW_GDO2_READ(void)
{
	return (GDOPORT(IN) >> GDO2_SHIFT) & 0x1;
}

SIVF CONFIG_GDO0_PIN_AS_INPUT(void)
{
	GDOPORT(SEL) &= ~(GDO0_MASK);
}

SIIF GDO0_PIN_IS_HIGH(void)
{
	return (GDOPORT(IN) & GDO0_MASK);
}

SIVF CC2500_HW_INIT(void)
{
	GDOPORT(SEL) &= ~(GDO0_MASK | GDO2_MASK);
	GDOPORT(DIR) &= ~(GDO0_MASK | GDO2_MASK);
	GDOPORT(IE) &= ~(GDO0_MASK | GDO2_MASK);
	GDOPORT(IES) &= ~(GDO0_MASK | GDO2_MASK);
	GDOPORT(IFG) &= ~(GDO0_MASK | GDO2_MASK);
	GDOPORT(REN) |= (GDO0_MASK | GDO2_MASK);
}

#undef SIVF
#undef SIIF

/* *********************
********************* */

#define CC2500_SPI_ENABLE()         spi_select_radio()
#define CC2500_SPI_DISABLE()        spi_deselect_radio()
#define CC2500_HW_CHECK_MISO_HIGH() spi_check_miso_high()

void CC2500_SPI_TX(int x)
{
	cc2500_status_register = spi_tx_rx(x);
}

#define CC2500_SPI_RX() spi_rx()

void CC2500_SPI_STROBE(int s)
{
	CC2500_SPI_ENABLE();
	CC2500_SPI_TX(s | CC2500_REG_ACCESS_OP_WRITE |
		      CC2500_REG_ACCESS_NOBURST);
	CC2500_SPI_DISABLE();
}

void CC2500_SPI_WREG(int a, int v)
{
	CC2500_SPI_ENABLE();
	CC2500_SPI_TX(a | CC2500_REG_ACCESS_OP_WRITE |
		      CC2500_REG_ACCESS_NOBURST);
	CC2500_SPI_TX(v);
	CC2500_SPI_DISABLE();
}

char CC2500_SPI_RREG(int a)
{
	char r;
	CC2500_SPI_ENABLE();
	CC2500_SPI_TX(a | CC2500_REG_ACCESS_OP_READ |
		      CC2500_REG_ACCESS_NOBURST);
	r = CC2500_SPI_RX();
	CC2500_SPI_DISABLE();
	return r;
}

char CC2500_SPI_ROREG(int a)
{
	char r;
	CC2500_SPI_ENABLE();
	CC2500_SPI_TX(a | CC2500_REG_ACCESS_OP_READ | CC2500_REG_ACCESS_BURST);
	r = CC2500_SPI_RX();
	CC2500_SPI_DISABLE();
	return r;
}

void CC2500_SPI_TX_BYTE(int a, int v)
{
	CC2500_SPI_ENABLE();
	CC2500_SPI_TX(a);
	CC2500_SPI_TX(v);
	CC2500_SPI_DISABLE();
}

char CC2500_SPI_RX_BYTE(int a)
{
	char r;
	CC2500_SPI_ENABLE();
	CC2500_SPI_TX(a | CC2500_REG_ACCESS_OP_READ);
	r = CC2500_SPI_RX();
	CC2500_SPI_DISABLE();
	return r;
}

void CC2500_SPI_TX_BURST(int addr, const char *val, int len)
{
	uint8_t cnt = 0;
	CC2500_SPI_ENABLE();
	CC2500_SPI_TX(addr | CC2500_REG_ACCESS_BURST);
	for (cnt = 0; cnt < (len); cnt++) {
		CC2500_SPI_TX(val[cnt]);
	}
	CC2500_SPI_DISABLE();
}

void CC2500_SPI_RX_BURST(int addr, uint8_t * val, int len)
{
	uint8_t cnt = 0;
	CC2500_SPI_ENABLE();
	CC2500_SPI_TX(addr | CC2500_REG_ACCESS_OP_READ |
		      CC2500_REG_ACCESS_BURST);
	for (cnt = 0; cnt < (len); cnt++) {
		val[cnt] = (uint8_t) CC2500_SPI_RX();
	}
	CC2500_SPI_DISABLE();
}

#define CC2500_SPI_TX_FIFO_BYTE(val)      CC2500_SPI_TX_BYTE (CC2500_DATA_FIFO_ADDR,val)
#define CC2500_SPI_TX_FIFO_BURST(val,len) CC2500_SPI_TX_BURST(CC2500_DATA_FIFO_ADDR,val,len)

char CC2500_SPI_RX_FIFO_BYTE()
{
	unsigned char r;
	r = CC2500_SPI_RX_BYTE(CC2500_DATA_FIFO_ADDR);
	return r;
}

#define CC2500_SPI_RX_FIFO_BURST(val,len) CC2500_SPI_RX_BURST(CC2500_DATA_FIFO_ADDR,val,len)

/******************************************************/
/* CC2500 State Machine in status byte (table 17 p24) */
/* 7     chip ready                                   */
/* 6:4   cc2500_status                                */
/* 3:0   fifo_bytes_available                         */
/*
struct _cc2500_status_bits_t {
  uint8_t chip_ready :1;
  uint8_t state      :3;
  uint8_t fifo_bytes :4;
};
*/
/******************************************************/

#define CC2500_STATUS_IDLE                      0x00
#define CC2500_STATUS_RX                        0x01
#define CC2500_STATUS_TX                        0x02
#define CC2500_STATUS_FSTXON                    0x03
#define CC2500_STATUS_CALIBRATE                 0x04
#define CC2500_STATUS_SETTLING                  0x05
#define CC2500_STATUS_RXFIFO_OVERFLOW           0x06
#define CC2500_STATUS_TXFIFO_UNDERFLOW          0x07

#define cc2500_get_state_from_status()   ((cc2500_status_register >> 4) & 0x07)
#define cc2500_update_status()           CC2500_SPI_STROBE(CC2500_STROBE_SNOP)

void cc2500_wait_status(uint8_t state)
{
	uint8_t s;
	do {
		cc2500_update_status();
		s = cc2500_get_state_from_status();
	} while (s != state);
}

void cc2500_gdo0_set_signal(uint8_t signal)
{
	uint8_t reg = signal & 0x3F;
	cc2500_gdo0_cfg = reg;
	CC2500_SPI_WREG(CC2500_REG_IOCFG0, reg);
}

void cc2500_gdo2_set_signal(uint8_t signal)
{
	uint8_t reg = signal & 0x3F;
	cc2500_gdo2_cfg = reg;
	CC2500_SPI_WREG(CC2500_REG_IOCFG2, reg);
}

void cc2500_set_fifo_threshold(uint8_t thr)
{
	uint8_t reg;
	reg = (thr & 0x0F);
	CC2500_SPI_WREG(CC2500_REG_FIFOTHR, reg);
}

/* **************************************************
 * RX overflow / TX underflow
 * **************************************************/

static inline int cc2500_check_tx_underflow(void)
{
	int ret;
	cc2500_update_status();
	ret =
	    (cc2500_get_state_from_status() == CC2500_STATUS_TXFIFO_UNDERFLOW);
	return ret;
}

static inline int cc2500_check_rx_overflow(void)
{
	char ret;
	cc2500_update_status();
	ret = (cc2500_get_state_from_status() == CC2500_STATUS_RXFIFO_OVERFLOW);
	return ret;
}

#define CC2500_FLUSH_RX()  CC2500_SPI_STROBE(CC2500_STROBE_SFRX)
#define CC2500_FLUSH_TX()  CC2500_SPI_STROBE(CC2500_STROBE_SFTX)

void cc2500_check_fifo_xflow_flush(void)
{
	if (cc2500_check_tx_underflow()) {
		CC2500_FLUSH_TX();
	}
	if (cc2500_check_rx_overflow()) {
		CC2500_FLUSH_RX();
	}
}

 /*
    void cc2500_check_fifo_xflow_flush(void)  
    {
    cc2500_update_status();
    switch (cc2500_get_state_from_status())
    {
    case CC2500_STATUS_TXFIFO_UNDERFLOW:
    CC2500_FLUSH_TX();
    break;
    case CC2500_STATUS_RXFIFO_OVERFLOW:
    CC2500_FLUSH_RX();
    break;
    }
    }
  */

/***************************************************************
 *  SmartRF Studio(tm) Export
 *
 *  Radio register settings specifed with C-code
 *  compatible #define statements.
 *
 ***************************************************************/
#if 1
#warning "Using library config 0"
// Chipcon
// Product = CC2500
// Chip version = E   (VERSION = 0x03)
// Crystal accuracy = 10 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 541.666667 kHz
// Phase = 1
// Datarate = 249.938965 kBaud
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 2432.999908 MHz
// Channel spacing = 199.951172 kHz
// Channel number = 0
// Optimization = Current
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (41) CHIP_RDY
const RF_SETTINGS rfSettings_default_config = {
	0x12,			// FSCTRL1   Frequency synthesizer control.
	0x00,			// FSCTRL0   Frequency synthesizer control.
	0x5D,			// FREQ2     Frequency control word, high byte.
	0x93,			// FREQ1     Frequency control word, middle byte.
	0xB1,			// FREQ0     Frequency control word, low byte.
	0x2D,			// MDMCFG4   Modem configuration.
	0x3B,			// MDMCFG3   Modem configuration.
	0xF3,			// MDMCFG2   Modem configuration.
	0x22,			// MDMCFG1   Modem configuration.
	0xF8,			// MDMCFG0   Modem configuration.
	0x00,			// CHANNR    Channel number.
	0x01,			// DEVIATN   Modem deviation setting (when FSK modulation is enabled).
	0xB6,			// FREND1    Front end RX configuration.
	0x10,			// FREND0    Front end TX configuration.
	0x18,			// MCSM0     Main Radio Control State Machine configuration.
	0x1D,			// FOCCFG    Frequency Offset Compensation Configuration.
	0x1C,			// BSCFG     Bit synchronization Configuration.
	0xC7,			// AGCCTRL2  AGC control.
	0x00,			// AGCCTRL1  AGC control.
	0xB0,			// AGCCTRL0  AGC control.
	0xEA,			// FSCAL3    Frequency synthesizer calibration.
	0x0A,			// FSCAL2    Frequency synthesizer calibration.
	0x00,			// FSCAL1    Frequency synthesizer calibration.
	0x11,			// FSCAL0    Frequency synthesizer calibration.
	0x59,			// FSTEST    Frequency synthesizer calibration.
	0x88,			// TEST2     Various test settings.
	0x31,			// TEST1     Various test settings.
	0x0B,			// TEST0     Various test settings.
	0x07,			// FIFOTHR   RXFIFO and TXFIFO thresholds.
	0x29,			// IOCFG2    GDO2 output pin configuration.
	0x06,			// IOCFG0D   GDO0 output pin configuration. Refer to SmartRF® Studio User Manual for detailed pseudo register explanation.
	0x04,			// PKTCTRL1  Packet automation control.
	0x05,			// PKTCTRL0  Packet automation control.
	0x00,			// ADDR      Device address.
	0xFF			// PKTLEN    Packet length.
};
#else
  /** 
   * manual configuration, kept only for compatibility with previous
   * applications. Do not use in new applications.
   **/
const RF_SETTINGS rfSettings_default_config = {
	0x0A,			/* ,freq if                       */
	0x00,			/* ,freq offset                   */
	0x5D,			/* ,ctrl word high                */
	0x93,			/* ,ctrl word middle              */
	0xB1,			/* ,ctrl word low                 */
	0x2D,			/* ,chann BW                      */
	0x3B,			/* ,drate                         */
	0x73,			/* MODEM CONTROL                  */
	0x22,			/* FEC+preamble+chann spacing     */
	0xF8,			/* ,chann spacing                 */
	0x00,			/* channel 0                      */
	0x01,			/* ,modem deviation               */
	0xB6,			/* ,front end RX                  */
	0x10,			/* ,front end TX                  */
	//    SMARTRF_SETTING_MCSM2           /* RSSI + WOR             */
	//    SMARTRF_SETTING_MCSM1           /* cca_mode + ??off_mode  */
	0x18,			/* autocal + ...          */
	0x1D,			/* ,freq offset                   */
	0x1C,			/* ,bit synchro                   */
	0xC7,			/* ,agc                           */
	0x00,			/* ,agc                           */
	0xB0,			/* ,agc                           */
	0xEA,			/* ,calibration                   */
	0x0A,			/* ,calibration                   */
	0x00,			/* ,calibration                   */
	0x11,			/* ,calibration                   */
	0x59,			/* , */
	0x88,			/* , */
	0x31,			/* , */
	0x0B,			/* , */
	0x07,			/* 7 == TX=33 RX=32               */
	0x29,			/* default=chip ready             */
	0x06,			/*                                */
	0x04,			/* 4 = append status              */
	0x05,			/* 5 = CRC+variable length        */
	0x00,			/*                                */
	0xFF			/* packet length                  */
};
#endif
/***************************************************************
 *  SmartRF Studio(tm) Export End
 ***************************************************************/

#define PATABLE_VALUE     0xFE

void cc2500_configure(RF_SETTINGS const *cfg)
{
	/* */
	CC2500_SPI_WREG(CC2500_REG_FSCTRL1, cfg->fsctrl1);
	CC2500_SPI_WREG(CC2500_REG_FSCTRL0, cfg->fsctrl0);
	CC2500_SPI_WREG(CC2500_REG_FREQ2, cfg->freq2);
	CC2500_SPI_WREG(CC2500_REG_FREQ1, cfg->freq1);
	CC2500_SPI_WREG(CC2500_REG_FREQ0, cfg->freq0);
	CC2500_SPI_WREG(CC2500_REG_MDMCFG4, cfg->mdmcfg4);
	CC2500_SPI_WREG(CC2500_REG_MDMCFG3, cfg->mdmcfg3);
	CC2500_SPI_WREG(CC2500_REG_MDMCFG2, cfg->mdmcfg2);
	CC2500_SPI_WREG(CC2500_REG_MDMCFG1, cfg->mdmcfg1);
	CC2500_SPI_WREG(CC2500_REG_MDMCFG0, cfg->mdmcfg0);
	CC2500_SPI_WREG(CC2500_REG_CHANNR, cfg->channr);
	CC2500_SPI_WREG(CC2500_REG_DEVIATN, cfg->deviatn);
	CC2500_SPI_WREG(CC2500_REG_FREND1, cfg->frend1);
	CC2500_SPI_WREG(CC2500_REG_FREND0, cfg->frend0);
	CC2500_SPI_WREG(CC2500_REG_MCSM0, cfg->mcsm0);
	CC2500_SPI_WREG(CC2500_REG_FOCCFG, cfg->foccfg);
	CC2500_SPI_WREG(CC2500_REG_BSCFG, cfg->bscfg);
	CC2500_SPI_WREG(CC2500_REG_AGCCTRL2, cfg->agcctrl2);
	CC2500_SPI_WREG(CC2500_REG_AGCCTRL1, cfg->agcctrl1);
	CC2500_SPI_WREG(CC2500_REG_AGCCTRL0, cfg->agcctrl0);
	CC2500_SPI_WREG(CC2500_REG_FSCAL3, cfg->fscal3);
	CC2500_SPI_WREG(CC2500_REG_FSCAL2, cfg->fscal2);
	CC2500_SPI_WREG(CC2500_REG_FSCAL1, cfg->fscal1);
	CC2500_SPI_WREG(CC2500_REG_FSCAL0, cfg->fscal0);
	CC2500_SPI_WREG(CC2500_REG_FSTEST, cfg->fstest);
	CC2500_SPI_WREG(CC2500_REG_TEST2, cfg->test2);
	CC2500_SPI_WREG(CC2500_REG_TEST1, cfg->test1);
	CC2500_SPI_WREG(CC2500_REG_TEST0, cfg->test0);
	CC2500_SPI_WREG(CC2500_REG_FIFOTHR, cfg->fifothr);
	CC2500_SPI_WREG(CC2500_REG_IOCFG2, cfg->iocfg2);
	CC2500_SPI_WREG(CC2500_REG_IOCFG0, cfg->iocfg0d);
	CC2500_SPI_WREG(CC2500_REG_PKTCTRL1, cfg->pktctrl1);
	CC2500_SPI_WREG(CC2500_REG_PKTCTRL0, cfg->pktctrl0);
	CC2500_SPI_WREG(CC2500_REG_ADDR, cfg->addr);
	CC2500_SPI_WREG(CC2500_REG_PKTLEN, cfg->pktlen);

	CC2500_SPI_WREG(CC2500_PATABLE_ADDR, PATABLE_VALUE);

    /*******************
     *
     *******************/

	// cc2500_calibrate();

	/* GDO0 asserted when rx fifo above threshold */
	cc2500_set_fifo_threshold(15);
	cc2500_gdo0_set_signal(CC2500_GDOx_RX_FIFO_EOP);
	CC2500_HW_GDO0_IRQ_ON_ASSERT();
	CC2500_HW_GDO0_DINT();

	/* GDO2 Deasserted when packet rx/tx or fifo xxxflow */
	cc2500_gdo2_set_signal(CC2500_GDOx_SYNC_WORD);
	CC2500_HW_GDO2_IRQ_ON_ASSERT();
	CC2500_HW_GDO2_DINT();
}

void cc2500_set_channel(uint8_t chan)
{
	CC2500_SPI_WREG(CC2500_REG_CHANNR, chan);
}

void cc2500_calibrate(void)
{
	cc2500_idle();
	CC2500_SPI_STROBE(CC2500_STROBE_SCAL);
	cc2500_wait_status(CC2500_STATUS_IDLE);
}

/* **************************************************
 * Tx operations
 * **************************************************/

/* pkt < 64 bytes, wait EOP                              */
/* blocking send, used for packet size below 64 bytes    */
/* this function does not require any interrupt handler  */
/* both modes wtr. READ_TX_FIFO_BYTE should work equally */
/* although register read might be cleaner               */

void cc2500_utx(const char *buffer, const uint8_t length)
{
	DBG_PRINTF("utx_enter\n");
	cc2500_idle();

	CC2500_HW_GDO2_DINT();

	/* Fill tx fifo */
	CC2500_SPI_TX_FIFO_BYTE(length);
	CC2500_SPI_TX_FIFO_BURST(buffer, length);
	DBG_PRINTF("utx 1\n");

	/* Send packet and wait for complete */
	CC2500_SPI_STROBE(CC2500_STROBE_STX);
	DBG_PRINTF("utx 2\n");

#define     STOP_READ_TX_FIFO_BYTES
#if defined(STOP_READ_TX_FIFO_BYTES)
	{
		uint8_t txbytes;
		do {
			txbytes = CC2500_SPI_ROREG(CC2500_REG_TXBYTES);
		} while ((txbytes & 0x7f) > 0);
	}
	DBG_PRINTF("utx 3\n");

#else
	/* this might not work for very small packets */
	while ((CC2500_HW_GDO2_READ()) == 0) ;	/* GDO2 goes high = SYNC TX */
	DBG_PRINTF("utx 3.1\n");
	while ((CC2500_HW_GDO2_READ()) != 0) ;	/* GDO2 goes low  = EOP     */
	DBG_PRINTF("utx 3.2\n");
#endif

	CC2500_HW_GDO0_CLEAR_FLAG();
	CC2500_HW_GDO2_CLEAR_FLAG();
	DBG_PRINTF("utx out\n");
}

/* **************************************************
 * Rx operations
 * **************************************************/

/* cc2500 operations */
void cc2500_rx_register_buffer(uint8_t * buffer, uint8_t length);
void cc2500_rx_enter(void);
void cc2500_rx_pkt_eop(void);

void cc2500_rx_register_buffer(uint8_t * buffer, uint8_t length)
{
	cc2500_rx_packet = buffer;
	cc2500_rx_offset = 0x00;
	CC2500_SPI_WREG(CC2500_REG_PKTLEN, length); /* simpler than testing in ISR */
}

void cc2500_rx_enter(void)
{
	cc2500_idle();
	cc2500_rx_offset = 0;

	CC2500_HW_GDO0_CLEAR_FLAG();	/* clear pending irq     */
	CC2500_HW_GDO2_CLEAR_FLAG();	/* clear pending irq     */
	CC2500_HW_GDO0_EINT();
	CC2500_HW_GDO2_DINT();

	CC2500_SPI_STROBE(CC2500_STROBE_SRX);
	cc2500_wait_status(CC2500_STATUS_RX);
}

/* ****************** */
/* ** RX EOP     **** */
/* ****************** */

void cc2500_rx_pkt_eop(void)
{				/* called from IRQ context */
	uint8_t rxbytes;
	int l;

	/* read RX bytes on general registers */
	rxbytes = CC2500_SPI_ROREG(CC2500_REG_RXBYTES);
	do {
		l = rxbytes;
		rxbytes = CC2500_SPI_ROREG(CC2500_REG_RXBYTES);
	} while (rxbytes < 2 && rxbytes != l);

	if ((0 < rxbytes)) {
		if ((rxbytes & 0x80) == 0) {	/* RX overflow == false */
			uint8_t size;

			/*
			 * This reads the first DATA bytes, this byte should be
			 * equal to rxbytes-1 (rxbytes == size + data))
			 */
			size = CC2500_SPI_RX_FIFO_BYTE();

			/* 
			 * rxbytes can be different from size+1 
			 * this happens on transmission errors 
			 * (ex: if packets is filled with a serie of bytes eq 0)
			 */
			CC2500_SPI_RX_FIFO_BURST(cc2500_rx_packet, rxbytes - 1);

			/* if (size +1 != rxbytes then it means we have frame options */

			/*
			 * Frame
			 * -----
			 * -------------------------------------------
			 * | S | ... data ... | CRC | RSSI | CRC+LQI |
			 * -------------------------------------------
			 *      0       size-1 
			 * 
			 * size + 0 : CRC          --> removed in RX_FIFO
			 * size + 1 : CRC          --> removed in RX_FIFO
			 * size + 2 : RSSI         --> size + 0
			 * size + 3 : CRCbit + LQI --> size + 1
			 *
			 * RSSI offset @ 250kbps, 72 [doc page 36, table 25]
			 */
#define FRAME_RSSI_OFFSET 0
#define FRAME_LQI_OFFSET  1
#define rssi_offset       72

			int rssi_dec;
			int rssi_dbm;
			rssi_dec = cc2500_rx_packet[size + FRAME_RSSI_OFFSET];
			if (rssi_dec >= 128) {
				rssi_dbm = (rssi_dec - 256) / 2 - rssi_offset;
			} else {
				rssi_dbm = (rssi_dec) / 2 - rssi_offset;
			}

			if (cc2500_rx_packet[size + FRAME_LQI_OFFSET] & 0x80) {	/* crc ok */
				/* ok */
				radio_rx_cb(cc2500_rx_packet, size, rssi_dbm);
			} else {
				cc2500_check_fifo_xflow_flush();
				radio_rx_cb(cc2500_rx_packet, -ERXBADCRC, 0);
			}
		} else {
			cc2500_check_fifo_xflow_flush();
			radio_rx_cb(cc2500_rx_packet, -ERXFLOW, 0);
		}
	} else {
		cc2500_check_fifo_xflow_flush();
		radio_rx_cb(cc2500_rx_packet, -EEMPTY, 0);
	}

	CC2500_HW_GDO0_CLEAR_FLAG();
	CC2500_HW_GDO2_CLEAR_FLAG();
}

/* **************************************************
 * utils
 * **************************************************/

void cc2500_rx_register_cb(cc2500_cb_t f)
{
	radio_rx_cb = f;
}

uint8_t cc2500_packet_status(void)
{
	uint8_t ps;
	ps = CC2500_SPI_ROREG(CC2500_REG_PKTSTATUS);
	return ps;
}

int cc2500_cca(void)
{
	uint8_t cca = cc2500_packet_status();
	return (cca >> 4) & 0x01;
}

uint8_t cc2500_get_rssi(void)
{
	uint8_t rssi;
	/* cc2500 should be in Rx */
	rssi = CC2500_SPI_ROREG(CC2500_REG_RSSI);
	return rssi;
}

/* **************************************************
 * Modes Idle/Sleep/Xoff operations
 * **************************************************/

/* idle mode
 * - wait for idle
 */
void cc2500_idle(void)
{
	CC2500_HW_GDO0_DINT();
	CC2500_HW_GDO2_DINT();
	cc2500_check_fifo_xflow_flush();
	CC2500_SPI_STROBE(CC2500_STROBE_SIDLE);
	cc2500_wait_status(CC2500_STATUS_IDLE);
}

/* sleep mode
 * - crystal is off
 * - configuration saved except power table and test registers
 * - calibration and power table update needed after sleep
 */
void cc2500_sleep(void)
{
	CC2500_SPI_STROBE(CC2500_STROBE_SPWD);
}

/* **************************************************
 * Init / Reset
 * **************************************************/

void cc2500_reset(void)
{
	CC2500_SPI_STROBE(CC2500_STROBE_SRES);
	cc2500_wait_status(CC2500_STATUS_IDLE);
}

void cc2500_wakeup(void)
{
	CC2500_SPI_ENABLE();
	/* wait for MISO to go high indicating the oscillator is stable */
	while (CC2500_HW_CHECK_MISO_HIGH()) ;
	/* wakeup is complete, drive CSn high and continue */
	CC2500_SPI_DISABLE();

	cc2500_idle();
	cc2500_wait_status(CC2500_STATUS_IDLE);
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void cc2500_variables_bootstrap(void)
{
	/* status */
	cc2500_status_register = 0;

	/* Internal driver variables for tx/rx */
	cc2500_rx_packet = 0x00;
	cc2500_rx_offset = 0x00;
	cc2500_rx_length = 0x00;
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void CC_ASSERT(int cond)
{
	if (!cond) {
		//LED_BLINK_ME();
	}
}

void cc2500_init(void)
{
	/* ------------------------------
	 * Init
	 * ------------------------------ */

	cc2500_variables_bootstrap();	/* bootstrap global cc2500 variables */

	DBG_PRINTF("init enter\n");

	CC2500_HW_INIT();

	DBG_PRINTF("  hw_init ok\n");

	/* initialize GPIO pins */
	CONFIG_GDO0_PIN_AS_INPUT();

	/* ------------------------------
	 * Power on reset sequence
	 * ------------------------------ */

	/* CC_ASSERT(spi_check_miso_high()); */

	DBG_PRINTF("  miso ok\n");

	/* pulse CSn low then high */
	spi_select_radio();
	delay_usec(10);
	spi_deselect_radio();
	/* hold CSn high for at least 40 microseconds */
	delay_usec(40);
	/* pull CSn low and wait for SO to go low */

	DBG_PRINTF("  reset 1 ok\n");

	spi_select_radio();
	while (spi_check_miso_high()) ;

	/* directly send strobe command - cannot use function as it affects CSn pin */
	spi_tx_rx(CC2500_STROBE_SRES);

	/* wait for SO to go low again, reset is complete at that point */
	while (spi_check_miso_high()) ;

	/* return CSn pin to its default high level */
	spi_deselect_radio();

	DBG_PRINTF("  reset 2 ok\n");

	/* ------------------------------
	 * Run time checks
	 * ------------------------------ */

	/* verify that SPI is working, PKTLEN is an arbitrary read/write register used for testing */
#define TEST_VALUE 0xA5
	unsigned short v;
	CC2500_SPI_WREG(CC2500_REG_PKTLEN, TEST_VALUE);
	v = CC2500_SPI_RREG(CC2500_REG_PKTLEN);
	CC_ASSERT((v & 0xff) == TEST_VALUE);	/* SPI is not responding */
	DBG_PRINTF("  t2 ok\n");

	/* verify the correct radio is installed */
#define RADIO_PARTNUM          0x80
#define RADIO_MIN_VERSION      3
#define SETTING_PATABLE0       0xFE
	v = CC2500_SPI_ROREG(CC2500_REG_PARTNUM);
	CC_ASSERT((v & 0xff) == RADIO_PARTNUM);	/* incorrect radio specified */
	DBG_PRINTF("  t3 ok\n");

	v = CC2500_SPI_ROREG(CC2500_REG_VERSION);
	CC_ASSERT((v & 0xff) >= RADIO_MIN_VERSION);	/* obsolete radio specified  */
	DBG_PRINTF("  t4 ok\n");

	/* ------------------------------
	 * Configuration
	 * ------------------------------ */

	cc2500_configure(&rfSettings_default_config);

	DBG_PRINTF("init done\n");
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void cc2500_gdox_signal_handler(uint8_t mask)
{
	if (mask & CC2500_GDO0) {
		/* rx fifo above threshold */
		cc2500_rx_pkt_eop();
	}

	if (mask & CC2500_GDO2) {
		/* SYNC_WORD */
	}
}

/* **************************************************
 * Interupts handler
 * **************************************************/

ISR(PORT2, port2_irq_handler)
{
	uint8_t cc_cmd = 0;
	uint8_t cc_alive = 0;

	DBG_PRINTF("port2 irq\n");

	if (P2IFG & (P2IE & GDO0_MASK)) {
		DBG_PRINTF("IRQ GDO0\n");
		cc_cmd |= CC2500_GDO0;
		cc_alive |= 1;
		P2IFG &= GDO0_MASK;
	}

	if (P2IFG & (P2IE & GDO2_MASK)) {
		DBG_PRINTF("IRQ GDO2\n");
		cc_cmd |= CC2500_GDO2;
		cc_alive |= 1;
		P2IFG &= GDO2_MASK;
	}

	if (cc_cmd != 0) {
		cc2500_gdox_signal_handler(cc_cmd);
	}

	if (cc_alive) {
		LPM_OFF_ON_EXIT;
	}

	P2IFG = 0;
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
