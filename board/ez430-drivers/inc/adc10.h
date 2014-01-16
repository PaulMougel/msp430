/**
 *  \file   adc10.h
 *  \brief  eZ430-RF2500 tutorial, adc10
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/
#include <stdint.h>

#ifndef ADC10_H
#define ADC10_H

void adc10_start(void);
void adc10_stop(void);

void adc10_calibrate(uint16_t coeff1, uint16_t coeff2);

int adc10_sample_temp(void);
int adc10_sample_avcc(void);

#endif
