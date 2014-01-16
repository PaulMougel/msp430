/**
 *  \file   timer.h
 *  \brief  eZ430-RF2500 tutorial, timers
 *  \author Antoine Fraboulet, Tanguy Risset, Dominique Tournier
 *  \date   2009
 **/

#ifndef MSP430_TIMER_H
#define MSP430_TIMER_H

/* ************************************************** */
/*                                                    */
/* ************************************************** */

typedef void (*timer_cb) (void);

/* timer A is set on VLO at 12 kHz */
void timerA_init(void);
void timerA_register_cb(timer_cb);
void timerA_set_wakeup(int);
void timerA_start_ticks(unsigned int ticks);
void timerA_start_milliseconds(unsigned int ms);
void timerA_stop(void);

/* timer B is set on VLO at 12kHz */
void timerB_init(void);
void timerB_register_cb(timer_cb);
void timerB_set_wakeup(int);
void timerB_start_ticks(unsigned ticks);
void timerB_start_milliseconds(unsigned ms);
void timerB_stop(void);

/* ************************************************** */
/*                                                    */
/* ************************************************** */

#endif
