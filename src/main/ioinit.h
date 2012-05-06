/*
 * ioinit.h
 *
 *  Created on: May 6, 2012
 *      Author: gentijo
 */

#ifndef IOINIT_H_
#define IOINIT_H_

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "../avr/driver/uart2.h"


void ioinit (void);
extern FILE Serial0;
extern FILE Serial1;

#endif /* IOINIT_H_ */
