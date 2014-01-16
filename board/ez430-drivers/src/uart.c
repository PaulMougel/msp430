/**
 *  \file   uart.c
 *  \brief  eZ430-RF2500 tutorial, uart
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

#include <stdio.h>

#include "isr_compat.h"
#include "lpm_compat.h"
#include "uart.h"

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

/*
 * USCI doc:
 *   UART user manual page 461
 *   SPI  user manual page 497
 *
 * USCI_A : serial
 * USCI_B : SPI
 *
 * Port 3
 * ======
 *   3.7 :
 *   3.6 :
 *   3.5 : uart data in    -> battery expansion board
 *   3.4 : uart data out   -> battery expansion board
 *   3.3 : spi clock (timer + flash + radio)
 *   3.2 : spi somi  (timer + flash + radio)
 *   3.1 : spi simo  (timer + flash + radio)
 *   3.0 : radio CSN (~CS)
 */

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

#define PORT   P3
#define BIT_RX (1<<5)
#define BIT_TX (1<<4)

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

static volatile uart_cb_t uart_cb;

void uart_init(int config)
{
	P3SEL |= (BIT_TX | BIT_RX);	/* uart   */
	P3DIR |= (BIT_TX);	/* output */
	P3DIR &= ~(BIT_RX);	/* input  */

	switch (config) {
	case UART_9600_SMCLK_1MHZ:
		UCA0CTL1 = UCSSEL_2;	// SMCLK
		UCA0BR0 = 0x68;	// 9600 from 1Mhz
		UCA0BR1 = 0x00;
		UCA0MCTL = UCBRS_2;
		UCA0CTL1 &= ~UCSWRST;	// **Initialize USCI state machine**
		break;

	case UART_9600_SMCLK_8MHZ:
		UCA0CTL1 = UCSSEL_2;	// SMCLK
		UCA0BR0 = 0x41;	// 9600 from 8Mhz
		UCA0BR1 = 0x03;
		UCA0MCTL = UCBRS_2;
		UCA0CTL1 &= ~UCSWRST;	// **Initialize USCI state machine**
		break;

	default:
		break;
	}

	uart_cb = NULL;
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void uart_stop(void)
{
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

int putchar(int c)
{
	UCA0TXBUF = c;
	while (!(IFG2 & UCA0TXIFG)) ;	// USCI_A0 TX buffer ready?
	return (unsigned char)c;
}

int uart_getchar(void)
{
	int c;
	c = UCA0RXBUF;
	return c;
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void uart_eint(void)
{
	IE2 |= UCA0RXIE;	// Enable USCI_A0 RX interrupt
}

void uart_dint(void)
{
	IE2 &= ~UCA0RXIE;
}

void uart_register_cb(uart_cb_t cb)
{
	uart_cb = cb;
	if (cb == NULL) {
		uart_dint();
	} else {
		uart_eint();
	}
}

ISR(USCIAB0RX, usart0irq)
{
	volatile unsigned char dummy;
	/* Check status register for receive errors. */
	if (UCA0STAT & UCRXERR) {
		/* Clear error flags by forcing a dummy read. */
		dummy = UCA0RXBUF;
		dummy += 1; /* warning gcc otherwise! */
	} else {
		if (uart_cb(UCA0RXBUF) != 0) {
			LPM_OFF_ON_EXIT;
		}
	}
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
