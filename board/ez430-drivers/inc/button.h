/**
 *  \file   button.h
 *  \brief  eZ430-RF2500 tutorial, button
 *  \author Antoine Fraboulet, Tanguy Risset,
 *          Dominique Tournier, Sebastien Mazy
 *  \date   2010
 **/

#ifndef BUTTON_H
#define BUTTON_H

typedef void (*button_cb) (void);

void button_init(void);
void button_stop(void);
int button_is_pressed(void);
void button_register_cb(button_cb f);
void button_enable_interrupt(void);
void button_disable_interrupt(void);

#endif
