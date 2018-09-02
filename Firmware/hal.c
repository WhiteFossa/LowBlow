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

	hal.c

	Hardware abstraction layer sources file.

	Part of "LowBlow" project.

	Created 2016/02/07 by White Fossa, also known as Artyom Vetrov.

	Feel free to contact: whitefossa@gmail.com

	Repository: https://github.com/WhiteFossa/LowBlow

	If repository above is not available, try my LiveJournal:
	fossa-white.livejournal.com

	or as last measure try to search for #WhiteFossa hashtag.

	-------------------------------------------------------------------------
 */

#include "main.h"

/** Very early initialization stage. Here we have to disable interrupts ad set-up watchdog to avoid reset during
 * initialization.
 */
void hal_early_init(void)
{
	cli(); /* Disabling interrupts */

	wdt_enable(HAL_WDT_INTERVAL); /* Setting up watchdog timer */
}

/* Interrupt handlers */

/**
 *  Timer1 overflow. With 16384 prescaler and 8MHz clock it occurs
 *  ~2 times per second
 */
ISR(TIM1_OVF_vect)
{
	/* Processing seconds ticks */
	hal_timer1_overflows_number --;

	if (0x00U == hal_timer1_overflows_number)
	{
		hal_second_tick();
	}
}

/**
 * External interrupt 0 - tachometer
 */
ISR(INT0_vect)
{
	hal_tacho_counter ++;
}

/* ADC measurement completed */
ISR(ADC_vect)
{
	/* Adding current measurement */
	hal_adc_accumulator += ADCL; /* !! MUST read it first !! */
	hal_adc_accumulator += ((uint16_t)(ADCH & 0b00000011) << 8);

	/* Averaging completed? */
	if (hal_adc_avg_counter > 0)
	{
		hal_adc_avg_counter --;
		return;
	}

	hal_temperature = hal_adc_accumulator >> HAL_ADC_RESULT_SHIFT_BITS;
	hal_adc_accumulator = 0x00U;
	hal_adc_avg_counter = HAL_ADC_NUM_OF_MEASUREMENTS;
}

/* End of interrupt handlers */

/**
 * Gets called every second
 */
void hal_second_tick()
{
	hal_timer1_overflows_number = HAL_TIMER1_OVERFLOWS_PER_SECOND; /* Reload Timer1 overflows counter */

	/* Update tachometer measurements */
	hal_tacho = hal_tacho_counter;
	hal_tacho_counter = 0x00U;

	/* For mn_seconds_delay() */
	if (mn_seconds_delay_counter > 0x00U)
	{
		mn_seconds_delay_counter --;
	}

	/* Raising corresponding flag */
	hal_flags0 |= _BV(HAL_F0_JUST_TICKED);

	/* Calling tick function from main code */
	mn_second_tick();
}

/**
 * Hardware initialization function. Call it before any other functions.
 */
void hal_init()
{
	/* Setting ports directions */
	DDRB = HAL_PORTB_DIR;

	/* Starting cooler (precaution against wrong code) */
	PORTB = _BV(HAL_COOLER_PIN);


	/* Setting up Timer0 (PWM source), but do not start cooler
	 * Prescaler 1, fast PWM, non-inverting OC0A output */
	TCCR0A |= _BV(WGM00) | _BV(WGM01);
	TCCR0B |= _BV(CS00);

	/* Set full PWM for case of emergency (i.e. hang on init stage). */
	hal_set_cooler_duty_cycle(HAL_MAX_DUTY_CYCLE);
	hal_start_cooler();

	/* Setting up Timer1 (time source)
	 * Prescaler 16384, interrupt on overflow */
	TCCR1 |= _BV(CS10) | _BV(CS11) | _BV(CS12) | _BV(CS13);
	GTCCR |= _BV(PSR1); /* Resetting prescaler */
	TCNT1 = 0x00; /* And timer too */

	/* We need to initialize this variable, otherwise first second tick may occur too late*/
	hal_timer1_overflows_number = HAL_TIMER1_OVERFLOWS_PER_SECOND;

	TIMSK |= _BV(TOIE1); /* Overflow interrupt */

	/* Raising edge causes INT0 - tachometer */
	MCUCR |= _BV(ISC01) | _BV(ISC00);
	GIMSK |= _BV(INT0);

	/* Reset tachometer counters */
	hal_tacho_counter = 0x00U;
	hal_tacho = 0x00U;

	/* Setting up ADC */
	/* For LM35 we need 1.1V reference, for LM335 we need 2.56 */
	#ifdef USE_LM35_SENSOR
		/* LM35 - 1.1V reference*/
		ADMUX = _BV(REFS1);
	#else
		/* LM335 - 2.56V reference*/
		ADMUX = _BV(REFS2) | _BV(REFS1);
	#endif

	/* ADC2 as input */
	ADMUX |= _BV(MUX1);
	DIDR0 |= _BV(ADC2D); /* No digital input buffer needed on ADC2/PB4 pin */

	ADCSRB = 0x00; /* Just to be sure that we are in free running mode */

	/* Enabled, set start flag, enable auto-triggering (according ADCSRB it will be free-running mode),
	 * interrupt on conversion complete, prescaler 128, i.e. clock 8MHz / 128 = 62.5kHz*/
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

	/* Setting ADC-related stuff */
	hal_adc_accumulator = 0x00U;
	hal_temperature = 0x00U;
	hal_adc_avg_counter = HAL_ADC_NUM_OF_MEASUREMENTS;

	/* Clearing all flags */
	hal_flags0 = 0x00U;

	sei(); /* Enabling interrupts, ready to go*/
}

/** Set PWM duty cycle. If cycle > HAL_MAX_DUTY_CYCLE then set to HAL_MAX_DUTY_CYCLE.
 * If cycle < HAL_MIN_DUTY_CYCLE then set it to HAL_MIN_DUTY_CYCLE.
 *
 * Setting duty cycle to HAL_MIN_DUTY_CYCLE will lead to cooler stop.
 *
 * @param cycle Duty cycle from HAL_MIN_DUTY_CYCLE to HAL_MIN_DUTY_CYCLE
 */
void hal_set_cooler_duty_cycle(int16_t cycle)
{
	if (cycle > HAL_MAX_DUTY_CYCLE)
	{
		cycle = HAL_MAX_DUTY_CYCLE;
	}
	else if (cycle < HAL_MIN_DUTY_CYCLE)
	{
		cycle = HAL_MIN_DUTY_CYCLE;
	}

	OCR0A = (uint8_t)cycle;

	if (HAL_MIN_DUTY_CYCLE == cycle)
	{
		/* Switching cooler off */
		hal_stop_cooler();
	}
	else
	{
		/* Switching cooler on */
		hal_start_cooler();
	}
}

/**
 * Call this function to stop cooler (PWM will be disabled, PWM output (PB0) will be held in 0).
 */
void hal_stop_cooler()
{
	TCCR0A &= ~(_BV(COM0A0) | _BV(COM0A1));

	HAL_COOLER_PORT &= ~_BV(HAL_COOLER_PIN);
}

/**
 * Call this function to start cooler (PWM will be enabled, using last set PWM duty cycle).
 */
void hal_start_cooler()
{
	TCCR0A |= _BV(COM0A1);
}

