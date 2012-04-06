// timer128.h
// ----------
// A very limited implementation of timer support for the 8bit Timer0 in
// the AVR Mega1284p
// adapted from Pascal Stang's library (attribution below)


/*! \file timer128.h \brief System Timer function library for Mega128. */
//*****************************************************************************
//
// File Name	: 'timer128.h'
// Title		: System Timer function library for Mega128
// Author		: Pascal Stang - Copyright (C) 2000-2003
// Created		: 11/22/2000
// Revised		: 02/10/2003
// Version		: 1.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
///	\ingroup driver_avr
/// \defgroup timer128 Timer Function Library for ATmega128 (timer128.c)
/// \code #include "timer128.h" \endcode
/// \par Overview
///		This library provides functions for use with the timers internal to the
///		AVR ATmega128.  Functions include initialization, set prescaler,
///		calibrated pause function (in milliseconds), attaching and detaching
///		of user functions to interrupts, overflow counters, and PWM.
///
/// \par About Timers
///		The Atmel AVR-series processors each contain at least one
///		hardware timer/counter.  Many of the processors contain 2 or 3
///		timers.  Generally speaking, a timer is a hardware counter inside
///		the processor which counts at a rate related to the main CPU clock
///		frequency.  Because the counter value increasing (counting up) at
///		a precise rate, we can use it as a timer to create or measure 
///		precise delays, schedule events, or generate signals of a certain
///		frequency or pulse-width.
/// \par
///		As an example, the ATmega163 processor has 3 timer/counters.
///		Timer0, Timer1, and Timer2 are 8, 16, and 8 bits wide respectively.
///		This means that they overflow, or roll over back to zero, at a
///		count value of 256 for 8bits or 65536 for 16bits.  A prescaler is
///		avaiable for each timer, and the prescaler allows you to pre-divide
///		the main CPU clock rate down to a slower speed before feeding it to
///		the counting input of a timer.  For example, if the CPU clock
///		frequency is 3.69MHz, and Timer0's prescaler is set to divide-by-8,
///		then Timer0 will "tic" at 3690000/8 = 461250Hz.  Because Timer0 is
///		an 8bit timer, it will count to 256 in just 256/461250Hz = 0.555ms.
///		In fact, when it hits 255, it will overflow and start again at
///		zero.  In this case, Timer0 will overflow 461250/256 = 1801.76
///		times per second.
/// \par
///		Timer0 can be used a number of ways simultaneously.  First, the
///		value of the timer can be read by accessing the CPU register \c TCNT0.
///		We could, for example, figure out how long it takes to execute a
///		C command by recording the value of \c TCNT0 before and after
///		execution, then subtract (after-before) = time elapsed.  Or we can
///		enable the overflow interrupt which goes off every time T0
///		overflows and count out longer delays (multiple overflows), or
///		execute a special periodic function at every overflow.
/// \par
///		The other timers (Timer1 and Timer2) offer all the abilities of
///		Timer0 and many more features.  Both T1 and T2 can operate as
///		general-purpose timers, but T1 has special hardware allowing it to
///		generate PWM signals, while T2 is specially designed to help count
///		out real time (like hours, minutes, seconds).  See the
///		Timer/Counter section of the processor datasheet for more info.
///
//*****************************************************************************
//@{

#ifndef TIMER128_H
#define TIMER128_H

#include "global.h"

// constants/macros/typdefs

// Timer/clock prescaler values and timer overflow rates
// tics = rate at which the timer counts up
// 8bitoverflow = rate at which the timer overflows 8bits (or reaches 256)
// 16bit [overflow] = rate at which the timer overflows 16bits (65536)
// 
// overflows can be used to generate periodic interrupts
//
// for 8MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 8MHz			8bitoverflow= 31250Hz		16bit= 122.070Hz
// 2 = CLOCK/8		tics= 1MHz			8bitoverflow= 3906.25Hz		16bit=  15.259Hz
// 3 = CLOCK/64		tics= 125kHz		8bitoverflow=  488.28Hz		16bit=   1.907Hz
// 4 = CLOCK/256	tics= 31250Hz		8bitoverflow=  122.07Hz		16bit=	0.477Hz
// 5 = CLOCK/1024	tics= 7812.5Hz		8bitoverflow=   30.52Hz		16bit=   0.119Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 4MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 4MHz			8bitoverflow= 15625Hz		16bit=  61.035Hz
// 2 = CLOCK/8		tics= 500kHz		8bitoverflow= 1953.125Hz	16bit=   7.629Hz
// 3 = CLOCK/64		tics= 62500Hz		8bitoverflow=  244.141Hz	16bit=   0.954Hz
// 4 = CLOCK/256	tics= 15625Hz		8bitoverflow=   61.035Hz	16bit=   0.238Hz
// 5 = CLOCK/1024	tics= 3906.25Hz		8bitoverflow=   15.259Hz	16bit=   0.060Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 3.69MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 3.69MHz		8bitoverflow= 14414Hz		16bit=  56.304Hz
// 2 = CLOCK/8		tics= 461250Hz		8bitoverflow= 1801.758Hz	16bit=   7.038Hz
// 3 = CLOCK/64		tics= 57625.25Hz	8bitoverflow=  225.220Hz	16bit=   0.880Hz
// 4 = CLOCK/256	tics= 14414.063Hz	8bitoverflow=   56.305Hz	16bit=   0.220Hz
// 5 = CLOCK/1024	tics=  3603.516Hz	8bitoverflow=   14.076Hz	16bit=   0.055Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 32.768KHz crystal on timer 2 (use for real-time clock)
// 0 = STOP
// 1 = CLOCK		tics= 32.768kHz	8bitoverflow= 128Hz
// 2 = CLOCK/8		tics= 4096kHz		8bitoverflow=  16Hz
// 3 = CLOCK/64		tics= 512Hz			8bitoverflow=   2Hz
// 4 = CLOCK/256	tics= 128Hz			8bitoverflow=   0.5Hz
// 5 = CLOCK/1024	tics= 32Hz			8bitoverflow=   0.125Hz

#define TIMER_CLK_STOP			0x00	///< Timer Stopped
#define TIMER_CLK_DIV1			0x01	///< Timer clocked at F_CPU
#define TIMER_CLK_DIV8			0x02	///< Timer clocked at F_CPU/8
#define TIMER_CLK_DIV64			0x03	///< Timer clocked at F_CPU/64
#define TIMER_CLK_DIV256		0x04	///< Timer clocked at F_CPU/256
#define TIMER_CLK_DIV1024		0x05	///< Timer clocked at F_CPU/1024
#define TIMER_CLK_T_FALL		0x06	///< Timer clocked at T falling edge
#define TIMER_CLK_T_RISE		0x07	///< Timer clocked at T rising edge
#define TIMER_PRESCALE_MASK		0x07	///< Timer Prescaler Bit-Mask

// default prescale settings for the timers
// these settings are applied when you call
// timerInit or any of the timer<x>Init
#define TIMER0PRESCALE		TIMER_CLK_DIV1024	///< timer 0 prescaler default

// interrupt macros for attaching user functions to timer interrupts
// use these with timerAttach( intNum, function )
// timer 0
#define TIMER0OVERFLOW_INT			0
#define TIMER0OUTCOMPARE_INT		1
// timer 1
#define TIMER1OVERFLOW_INT			2
#define TIMER1OUTCOMPAREA_INT		3
#define TIMER1OUTCOMPAREB_INT		4
#define TIMER1OUTCOMPAREC_INT		5
#define TIMER1INPUTCAPTURE_INT		6
// timer 2
#define TIMER2OVERFLOW_INT			7
#define TIMER2OUTCOMPARE_INT		8
// timer 3
#define TIMER3OVERFLOW_INT			9
#define TIMER3OUTCOMPAREA_INT		10
#define TIMER3OUTCOMPAREB_INT		11
#define TIMER3OUTCOMPAREC_INT		12
#define TIMER3INPUTCAPTURE_INT		13

#define TIMER_NUM_INTERRUPTS		14

// type of interrupt handler to use for timers
// *do not change unless you know what you're doing
// Value may be SIGNAL or INTERRUPT
#ifndef TIMER_INTERRUPT_HANDLER
#define TIMER_INTERRUPT_HANDLER		SIGNAL
#endif

// default initialization routines for each timer
void timer0Init(void);

void timer0SetCounter(u08 value);

// Clock prescaler set/get commands for each timer/counter
// For setting the prescaler, you should use one of the #defines
// above like TIMER_CLK_DIVx, where [x] is the division rate
// you want.
// When getting the current prescaler setting, the return value
// will be the [x] division value currently set.
void timer0SetPrescaler(u08 prescale);		///< set timer0 prescaler division index
u16  timer0GetPrescaler(void);				///< get timer0 prescaler division rate


// TimerAttach and Detach commands
//		These functions allow the attachment (or detachment) of any user function
//		to a timer interrupt.  "Attaching" one of your own functions to a timer
//		interrupt means that it will be called whenever that interrupt happens.
//		Using attach is better than rewriting the actual INTERRUPT() function
//		because your code will still work and be compatible if the timer library
//		is updated.  Also, using Attach allows your code and any predefined timer
//		code to work together and at the same time.  (ie. "attaching" your own
//		function to the timer0 overflow doesn't prevent timerPause from working,
//		but rather allows you to share the interrupt.)
//
//		timerAttach(TIMER1OVERFLOW_INT, myOverflowFunction);
//		timerDetach(TIMER1OVERFLOW_INT)
//
//		timerAttach causes the myOverflowFunction() to be attached, and therefore
//		execute, whenever an overflow on timer1 occurs.  timerDetach removes the
//		association and executes no user function when the interrupt occurs.
//		myOverflowFunction must be defined with no return value and no arguments:
//
//		void myOverflowFunction(void) { ... }

void timerAttach(u08 interruptNum, void (*userFunc)(void) );
void timerDetach(u08 interruptNum);

#endif
