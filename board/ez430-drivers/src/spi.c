/**
 *  \file   spi.c
 *  \brief  eZ430-RF2500 tutorial, spi
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/

#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
#include <msp430.h>
#elif defined(__IAR_SYSTEMS_ICC__)
/* This is the IAR compiler */
#include <io430.h>
#include <intrinsics.h>
#endif

#include "io_compat.h"
#include <stdio.h>

#include "spi.h"

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

/*
** SPI port and Uart are separated and can be used at the same time
**
** USI doc: user manual page 443
**
** USCI doc:
**   UART user manual page 461
**   SPI  user manual page 497
**
** USCI_A : serial
** USCI_B : SPI
**/

/*
 *
 * Port 2
 * ======
 *   2.7 : GDO2
 *   2.6 : GDO0
 *
 * Port 3
 * ======
 *   3.3 : spi clock (radio)
 *   3.2 : spi somi  (radio)
 *   3.1 : spi simo  (radio)
 *   3.0 : radio CSN (~CS)
 *
*/

#define SPI_PORT_IN      P3IN
#define BIT_CLK          BV(3)
#define BIT_SO           BV(2)
#define BIT_SI           BV(1)

#define RADIO_CS_PIN     (1<<0)

#define RADIO_DISABLE()  P3OUT |= RADIO_CS_PIN
#define RADIO_ENABLE()   P3OUT &= ~RADIO_CS_PIN

#define SPI_TX           UCB0TXBUF
#define SPI_RX           UCB0RXBUF

#define SPI_WAIT_EOT()	 do { } while (! (IFG2 & UCB0TXIFG) )
#define	SPI_WAIT_EOR()	 do { } while (! (IFG2 & UCB0RXIFG) )

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

#define st(x) do { x } while (0)

/* CSn Pin Configuration */
#define __SPI_CSN_GPIO_BIT__             0
#define SPI_CONFIG_CSN_PIN_AS_OUTPUT()   st( P3DIR |=  BV(__SPI_CSN_GPIO_BIT__); )
#define SPI_DRIVE_CSN_HIGH()             st( P3OUT |=  BV(__SPI_CSN_GPIO_BIT__); )	/* atomic operation */
#define SPI_DRIVE_CSN_LOW()              st( P3OUT &= ~BV(__SPI_CSN_GPIO_BIT__); )	/* atomic operation */
#define SPI_CSN_IS_HIGH()                 (  P3OUT &   BV(__SPI_CSN_GPIO_BIT__) )

/* SCLK Pin Configuration */
#define __SPI_SCLK_GPIO_BIT__            3
#define SPI_CONFIG_SCLK_PIN_AS_OUTPUT()  st( P3DIR |=  BV(__SPI_SCLK_GPIO_BIT__); )
#define SPI_DRIVE_SCLK_HIGH()            st( P3OUT |=  BV(__SPI_SCLK_GPIO_BIT__); )
#define SPI_DRIVE_SCLK_LOW()             st( P3OUT &= ~BV(__SPI_SCLK_GPIO_BIT__); )

/* SI Pin Configuration */
#define __SPI_SI_GPIO_BIT__              1
#define SPI_CONFIG_SI_PIN_AS_OUTPUT()    st( P3DIR |=  BV(__SPI_SI_GPIO_BIT__); )
#define SPI_DRIVE_SI_HIGH()              st( P3OUT |=  BV(__SPI_SI_GPIO_BIT__); )
#define SPI_DRIVE_SI_LOW()               st( P3OUT &= ~BV(__SPI_SI_GPIO_BIT__); )

/* SO Pin Configuration */
#define __SPI_SO_GPIO_BIT__              2
#define SPI_CONFIG_SO_PIN_AS_INPUT()	/* nothing to required */
#define SPI_SO_IS_HIGH()                 ( P3IN & BV(__SPI_SO_GPIO_BIT__) )

/* SPI Port Configuration */
#define SPI_CONFIG_PORT()						\
    st( P3SEL |=								\
            BV( __SPI_SCLK_GPIO_BIT__ ) |					\
            BV( __SPI_SI_GPIO_BIT__   ) |					\
            BV( __SPI_SO_GPIO_BIT__   ); )

#define SPI_INIT() \
    st ( \
            UCB0CTL1 = UCSWRST;                           \
            UCB0CTL1 = UCSWRST | UCSSEL1;                 \
            UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;   \
            UCB0BR0  = 2;                                 \
            UCB0BR1  = 0;                                 \
            SPI_CONFIG_PORT();				\
            UCB0CTL1 &= ~UCSWRST;                         \
       )

void spi_init(void)
{
	/* configure all SPI related pins */
	SPI_CONFIG_CSN_PIN_AS_OUTPUT();
	SPI_CONFIG_SCLK_PIN_AS_OUTPUT();
	SPI_CONFIG_SI_PIN_AS_OUTPUT();
	SPI_CONFIG_SO_PIN_AS_INPUT();

	/* set CSn to default high level */
	SPI_DRIVE_CSN_HIGH();

	/* initialize the SPI registers */
	SPI_INIT();
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void inline spi_clear_rx_IFG() {
	__asm__ __volatile__("BIC.B #4, &0x3	/* BIC.B #UCB0RXIFG, #IFG2 */ \n");
}

int spi_tx_rx(int data)
{
	char res;
	spi_clear_rx_IFG();
	SPI_TX = data;
	SPI_WAIT_EOR();
	res = SPI_RX;
	return res;
}



int spi_check_miso_high(void)
{
	return SPI_SO_IS_HIGH();
}

void spi_tx_burst(char *data, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		spi_clear_rx_IFG();
		SPI_TX = data[i];
		SPI_WAIT_EOR();
	}
}

void spi_rx_burst(char *data, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		SPI_TX = SPI_DUMMY_BYTE;
		SPI_WAIT_EOR();
		data[i] = SPI_RX;
	}
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void spi_select_radio(void)
{
	RADIO_ENABLE();
}

void spi_deselect_radio(void)
{
	RADIO_DISABLE();
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
