/**
 *  \file   spi.h
 *  \brief  eZ430-RF2500 tutorial, spi
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/

#ifndef SPI_H
#define SPI_H
#ifndef BV
#define BV(x) (1 << (x))
#endif

//#include <stdint.h>

/* ************************************************** */
/* SPI                                                */
/* ************************************************** */

#define SPI_DUMMY_BYTE 0x55

void spi_init(void);

int spi_tx_rx(int x);
void spi_tx_burst(char *, int);
void spi_rx_burst(char *, int);
int spi_check_miso_high(void);

#define spi_rx()               spi_tx_rx(SPI_DUMMY_BYTE)

void spi_select_radio(void);
void spi_deselect_radio(void);

/* ************************************************** */
/*                                                    */
/* ************************************************** */

#endif
