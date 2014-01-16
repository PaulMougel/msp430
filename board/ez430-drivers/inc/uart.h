/**
 *  \file   uart.h
 *  \brief  eZ430-RF2500 tutorial, uart
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/

#ifndef UART_H
#define UART_H

/* ************************************************** */
/* UART                                               */
/* ************************************************** */

#define UART_9600_SMCLK_1MHZ   0x01
#define UART_9600_SMCLK_8MHZ   0x08

typedef int (*uart_cb_t) (unsigned char data);

void uart_init(int config);
void uart_stop(void);
void uart_register_cb(uart_cb_t);

int putchar(int);
int getchar(void);

/* ************************************************** */
/*                                                    */
/* ************************************************** */

#endif
