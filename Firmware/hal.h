/*
	This file is part of Power Unit Cooler Controller firmware.

	Power Unit Cooler Controller firmware is free software:
	you can redistribute it and/or modify it under the terms of the
	GNU General Public License as published by the Free Software
	Foundation, either version 3 of the License, or (at your option)
	any later version.

	Power Unit Cooler Controller firmware is distributed in the hope
	that it will be useful, but WITHOUT ANY WARRANTY; without even
	the implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE.  See the GNU General Public License for more
	details.

	You should have received a copy of the GNU General Public License
	along with Notebook Cooler Controller firmware.

	If not, see <http://www.gnu.org/licenses/>.

	-------------------------------------------------------------------------

	hal.h

	Hardware abstraction layer header file

	Created 2016/02/07 by White Fossa, also known as Artyom Vetrov

	Feel free to contact: fossa-artem@mail.ru

	Repository: http://home.whitefossa.org/svn/PowerUnitCoolerController/

	If repository above is not available, try my LiveJournal:
	fossa-white.livejournal.com

	or as last measure try to search for #WhiteFossa hashtag.

	-------------------------------------------------------------------------
 */

#ifndef HAL_H_
#define HAL_H_

/**
 * Hardware:
 *
 *                             ATTINY25
 *                           +----__----+
 * RESET (PB5)          ->---| 1      8 |-----     VCC
 * NOT CONNECTED	->---| 2      7 |---<-     TACHO	(INT0/PB2)
 * TSENS (ADC2/PB4)     ->---| 3      6 |---<-     NOT CONNECTED
 * GND                  -----| 4      5 |--->-     PWM		(PB0/OC0A)
 *                           +----------+
 *
 * Directions:
 * PIN		DIR		DDRB
 *
 * PB0		OUT		1
 * PB1		IN		0
 * PB2		IN		0
 * PB3		IN		0
 * PB4		IN		0
 * PB5		IN		0
 *
 *
 * We can use two kinds on thermal sensors:
 *
 * 1) LM35 in direct mode, i.e. 10mV/°C. Uout=0.01*t°. Uncomment USE_LM35_SENSOR if you are using it. ADC will use 1.1V reference,
 * 10-bit mode, so ADC = 9.(309)*t°.
 *
 *   t °C		U V		ADC
 *   0			0		0
 *   100		1		233
 *
 * 233 steps per 100°C
 *
 * 2) LM335 with 2/3 divider, i.e. 6.(6)mV/°K. Uout=2*(t°+273.15)/300. ADC will use 2.56V reference, 10-bit moide. ADC=2.(6)*t°+728.4
 *
 *   t °C		U V		ADC
 *   0		   1.821	728
 *   100       2.487	995
 *
 * 267 steps per 100°C
 *
 * t° is temperature in Celsius.
 */

/**
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!!!!! UNCOMMENT IT IF LM35 SENSOR USED !!!!!!!!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * #define USE_LM35_SENSOR
 */

/* AVR Libc includes*/
#include <avr/io.h> /* Common */
#include <avr/wdt.h> /* Watchdog timer */
#include <avr/interrupt.h> /* Interrupts support */
#include <util/delay_basic.h> /* Basic delays */

/* Macros */


/* Get cooler duty cycle */
#define hal_get_cooler_duty_cycle() OCR0A


/* End of macros */

/* Constants */

/**
 *  MCU Frequency
 */
#define F_CPU 8000000UL

/** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *  !!PORTS DIRECTIONS - WRONG SETTINGS WILL DAMAGE HARDWARE!!
 *  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */
#define HAL_PORTB_DIR 0b00000001

/**
 * Port, where cooler connected
 */
#define HAL_COOLER_PORT PORTB

/**
 * Cooler connected here
 */
#define HAL_COOLER_PIN 0x00 /* PB0 */

/**
 *  Watchdog timeout interval
 */
#define HAL_WDT_INTERVAL WDTO_2S

/**
 * Minimal possible duty cycle (set cycle to it to stop cooler).
 */
#define HAL_MIN_DUTY_CYCLE 0x00

/**
 * Maximal possible duty cycle
 */
#define HAL_MAX_DUTY_CYCLE 0xFF

/* This number of Timer1 overflows is near equal to 1 second
 * clock = 8.0 MHz, prescaler 16384, overflow occurs 8000000 / (16384 * 256) ~ 2 times per second.
 */
#define HAL_TIMER1_OVERFLOWS_PER_SECOND 2

/**
 * Number of bits, to what hal_adc_accumulator need to be shifted left to get hal_temperature.
 */
#define HAL_ADC_RESULT_SHIFT_BITS 8

/**
 * How many ADC measurements will be averaged to get temperature MINUS ONE (to keep it within uint8_t)
 */
#define HAL_ADC_NUM_OF_MEASUREMENTS (1 << HAL_ADC_RESULT_SHIFT_BITS) - 1

/* Flags */

/**
 *  Raised on exit from hal_second_tick() function. Can be used to know that new tachometer measurement completed
 */
#define HAL_F0_JUST_TICKED 0

/* End of flags */

/* End of constants */



/* Variables */

/**
 * Variable for different flags, related to HAL. By default all flags are cleared (zeros).
 */
volatile uint8_t hal_flags0;

/* Loaded with HAL_TIMER1_OVERFLOWS_PER_SECOND on hal_second_tick() and decreases on every Timer1 interrupt. If this
 * variable equals 0 AFTER decrement then new second passed, calling hal_second_tick()
 */
volatile uint16_t hal_timer1_overflows_number;

/** Increases with every pulse from tachometer (i.e. INT0), gets copied to hal_tacho when hal_second_tick() called and
 * set to 0. For most coolers 2 pulses from tachometer = 1 revolution, but as I heard some coolers with 8 pulses per
 * revolution exists.
 * With uint16_t, 8 pulses per revolution and 1 second interval maximal allowed RPM will be 65535 * 60 / 8 = 491512.5
 */
volatile uint16_t hal_tacho_counter;

/** Number of tachometer pulses per second
 */
volatile uint16_t hal_tacho;

/**
 * ADC measurements accumulating here before being averaged and placed into hal_temperature.
 */
volatile uint32_t hal_adc_accumulator;

/**
 * Counter to count ADC measurements.
 */
volatile uint8_t hal_adc_avg_counter;


/**
 * Averaged ADC measurement, i.e. temperature.
 */
volatile uint16_t hal_temperature;

/* End of variables */


/* Functions prototypes */

/** Very early initialization stage. Here we have to disable interrupts ad set-up watchdog to avoid reset during
 * initialization.
 */
void hal_early_init(void) __attribute__((naked)) __attribute__((section(".init3")));

/**
 * Hardware initialization function. Call it before any other functions.
 */
void hal_init();

/** Set PWM duty cycle. If cycle > HAL_MAX_DUTY_CYCLE then set to HAL_MAX_DUTY_CYCLE.
 * If cycle < HAL_MIN_DUTY_CYCLE then set it to HAL_MIN_DUTY_CYCLE.
 *
 * Setting duty cycle to HAL_MIN_DUTY_CYCLE will lead to cooler stop.
 *
 * @param cycle Duty cycle from HAL_MIN_DUTY_CYCLE to HAL_MIN_DUTY_CYCLE
 */
void hal_set_cooler_duty_cycle(int16_t cycle);

/**
 * Gets called every second
 */
void hal_second_tick();

/**
 * Call this function to stop cooler (PWM will be disabled, PWM output (PB0) will be held in 0).
 */
void hal_stop_cooler();

/**
 * Call this function to start cooler (PWM will be enabled, using last set PWM duty cycle).
 */
void hal_start_cooler();

/* End of function prototypes */

#endif /* HAL_H_ */
