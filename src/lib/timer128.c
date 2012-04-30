// timer128.c
// ----------
// A very limited implementation of timer support for the 8bit Timer0 in
// the AVR Mega1284p
// adapted from Pascal Stang's library (attribution below)

/*! \file timer128.c \brief System Timer function library for Mega128. */
//*****************************************************************************
//
// File Name	: 'timer128.c'
// Title		: System Timer function library for Mega128
// Author		: Pascal Stang - Copyright (C) 2000-2003
// Created		: 11/22/2000
// Revised		: 02/24/2003
// Version		: 1.2
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "global.h"
#include "timer128.h"

// Program ROM constants
// the prescale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/64, CLK/256, CLK/1024
unsigned short __attribute__ ((progmem)) TimerPrescaleFactor[] = {0,1,8,64,256,1024};

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntFunc[TIMER_NUM_INTERRUPTS];


void timer0Init()
{
	// initialize timer 0
	timer0SetPrescaler( TIMER0PRESCALE );	// set prescaler
	timer0SetCounter(0);					// reset TCNT0; this is the actual 8bit counter value
	sbi(TIMSK0, TOIE0);						// enable TCNT0 overflow interrupt
	// ok, so TOIE0 is bit 0 of TIMSK0, and enables the overflow interrupt as specified
}

/**
 * initialize timer 2 for external 32kHz crystal
 */
void timer2Init()
{
	// set external clock source (32kHz crystal)
	sbi(ASSR, AS2);
	timer2SetPrescaler( TIMER2PRESCALE );
	timer2SetCounter(0);
	sbi(TIMSK2, TOIE2);
}

void timer0SetCounter(u08 value)
{
	outb(TCNT0, value);
}

void timer2SetCounter(u08 value)
{
	outb(TCNT2, value);
}

void timer0SetPrescaler(u08 prescale)
{
	// set prescaler on timer 0
	// TCCR0B - Timer / Counter Control Register B
	outb(TCCR0B, (inb(TCCR0B) & ~TIMER_PRESCALE_MASK) | prescale);
}

void timer2SetPrescaler(u08 prescale)
{
	// set prescaler on timer 2
	// TCCR2B - Timer / Counter Control Register B
	outb(TCCR2B, (inb(TCCR2B) & ~TIMER_PRESCALE_MASK) | prescale);
}

u16 timer0GetPrescaler(void)
{
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor+(inb(TCCR0B) & TIMER_PRESCALE_MASK)));
}

u16 timer2GetPrescaler(void)
{
	// get the current prescaler setting
	return (pgm_read_word(TimerPrescaleFactor+(inb(TCCR2B) & TIMER_PRESCALE_MASK)));
}

void timerAttach(u08 interruptNum, void (*userFunc)(void) )
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		TimerIntFunc[interruptNum] = userFunc;
	}
}

void timerDetach(u08 interruptNum)
{
	// make sure the interrupt number is within bounds
	if(interruptNum < TIMER_NUM_INTERRUPTS)
	{
		// set the interrupt function to run nothing
		TimerIntFunc[interruptNum] = 0;
	}
}

//! Interrupt handler for tcnt0 overflow interrupt
TIMER_INTERRUPT_HANDLER(TIMER0_OVF_vect)
{
	// if a user function is defined, execute it too
	if(TimerIntFunc[TIMER0OVERFLOW_INT])
		TimerIntFunc[TIMER0OVERFLOW_INT]();
}

//! Interrupt handler for tcnt2 overflow interrupt
TIMER_INTERRUPT_HANDLER(TIMER2_OVF_vect)
{
	// if a user function is defined, execute it too
	if(TimerIntFunc[TIMER2OVERFLOW_INT])
		TimerIntFunc[TIMER2OVERFLOW_INT]();
}
