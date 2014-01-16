/**
 *  \file   leds.h
 *  \brief  eZ430-RF2500 tutorial, leds
 *  \author Antoine Fraboulet, Tanguy Risset,
 *          Dominique Tournier, Sebastien Mazy
 *  \date   2010
 **/

#ifndef LEDS_H
#define LEDS_H

/* ************************************************** */
/* Leds                                               */
/* ************************************************** */

void leds_init(void);
void leds_on(void);
void leds_off(void);
void led_green_off(void);
void led_green_on(void);
void led_green_switch(void);
void led_red_off(void);
void led_red_on(void);
void led_red_switch(void);

#endif
