/**
 *  \file   clock.h
 *  \brief  eZ430-RF2500 tutorial, clock
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/

#ifndef MSP430_CLOCK_H
#define MSP430_CLOCK_H

/* ************************************************** */
/* Clock                                              */
/* ************************************************** */

int get_dco_mhz();
int get_mclk_freq_mhz();

void set_mcu_speed_dco_mclk_1MHz_smclk_1MHz();

void set_mcu_speed_dco_mclk_8MHz_smclk_8MHz();
void set_mcu_speed_dco_mclk_8MHz_smclk_4MHz();
void set_mcu_speed_dco_mclk_8MHz_smclk_2MHz();
void set_mcu_speed_dco_mclk_8MHz_smclk_1MHz();

void set_mcu_speed_dco_mclk_12MHz_smclk_12MHz();
void set_mcu_speed_dco_mclk_12MHz_smclk_6MHz();
void set_mcu_speed_dco_mclk_12MHz_smclk_3MHz();
void set_mcu_speed_dco_mclk_12MHz_smclk_1_5MHz();

void set_mcu_speed_dco_mclk_16MHz_smclk_16MHz();
void set_mcu_speed_dco_mclk_16MHz_smclk_8MHz();
void set_mcu_speed_dco_mclk_16MHz_smclk_4MHz();
void set_mcu_speed_dco_mclk_16MHz_smclk_2MHz();

/* delay_ms and delay_usec are blocking,
a timer should be used instead in most cases */

/* blocks at least usec microseconds (consider using a timer instead) */
void delay_usec(unsigned int usec);
/* blocks at least ms milliseconds (consider using a timer instead) */
void delay_ms(unsigned int ms);
/* consumes 4*n+k CPU cycles with k constant */
extern void loop_4_cycles(unsigned long n);

/* ************************************************** */
/*                                                    */
/* ************************************************** */

#endif
