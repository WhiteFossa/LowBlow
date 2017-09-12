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

	main.c

	Main code file

	Created 2016/02/07 by White Fossa, also known as Artyom Vetrov

	Feel free to contact: fossa-artem@mail.ru

	Repository: http://home.whitefossa.org/svn/PowerUnitCoolerController/

	If repository above is not available, try my LiveJournal:
	fossa-white.livejournal.com

	or as last measure try to search for #WhiteFossa hashtag.

	-------------------------------------------------------------------------
 */

/**
 * Main header file
 */

#include "main.h"


/**
 * Entry point
 */
void main()
{
	/* RPM loop disabled at start-up */
	mn_flags0 = _BV(MN_F0_DISABLE_RPM_LOOP);

	/* Initialize hardware */
	hal_init();

	/* Switching cooler off and waiting for stop */
	hal_set_cooler_duty_cycle(HAL_MIN_DUTY_CYCLE);

	hal_tacho = 0xFFU; /* To avoid immediate exit from loop */
	while(hal_tacho > 0x00U)
	{
		mn_seconds_delay(MN_WAIT_FOR_COOLER_STOP);
	}

	/* Cooler stopped, slowly increasing duty cycle until MN_COOLER_SPIN_UP_MINIMAL_RPM reached to find
	 * minimal RPM.
	 */
	while(1)
	{
		if ((hal_flags0 & _BV(HAL_F0_JUST_TICKED)) != 0x00U)
		{
			/* New measurement arrived */
			if (hal_tacho < MN_COOLER_SPIN_UP_MINIMAL_RPM)
			{
				/* We need more */
				hal_set_cooler_duty_cycle(hal_get_cooler_duty_cycle() + MN_COOLER_SPIN_UP_DUTY_CYCLE_STEP);
			}
			else
			{
				break;
			}

			hal_flags0 &= ~_BV(HAL_F0_JUST_TICKED); /* Ready for the next measurement */
		}

		wdt_reset();
	}

	/* Wait and measuring RPM */
	mn_seconds_delay(MN_COOLER_SPIN_UP_WAIT);
	mn_min_duty_cycle = hal_get_cooler_duty_cycle();
	mn_min_rpm = hal_tacho;

	/* Full throttle and measure again */
	hal_set_cooler_duty_cycle(HAL_MAX_DUTY_CYCLE);

	mn_seconds_delay(MN_COOLER_SPIN_UP_WAIT);
	mn_max_rpm = hal_tacho;

	/* Do we measured RPM correctly? */
	if (mn_max_rpm <= mn_min_rpm)
	{
		/* Just hang, because we have full throttle set */
		while(1)
		{
			wdt_reset();
		}
	}

	/* Main loop */
	while(1)
	{
		mn_set_rpm_in_percents(mn_lookup_rpm(hal_temperature));
		wdt_reset();
	}
}

/**
 * This function gets called every second (from hal_second_tick())
 */
void mn_second_tick()
{
	if (0x00 == (mn_flags0 & _BV(MN_F0_DISABLE_RPM_LOOP)))
	{
		/* Adjusting RPM */
		int16_t error = (int16_t)hal_tacho - (int16_t)mn_target_rpm; /* RMP error level */

		hal_set_cooler_duty_cycle((int16_t)hal_get_cooler_duty_cycle() - error);
	}
}

/**
 * Function waits given number of seconds.
 */
void mn_seconds_delay(uint8_t num_of_seconds)
{
	mn_seconds_delay_counter = num_of_seconds;
	while(mn_seconds_delay_counter > 0)
	{
		wdt_reset();
	}
}

/**
 * Call this function to set target RPM for cooler.
 *
 * @param target_rpm For 2 tachometer pulses per revolution (i.e. common cooler) 1 = 30 RPM.
 * If set to 0, then cooler will be stopped and RPM loop disabled.
 * If not 0, then cooler will be started and RPM loop enabled.
 * If set higher than mn_max_rpm - will be treated as mn_max_rpm.
 * If set lower than mn_min_rpm but higher then 0 - will be treated as mn_min_rpm.
 */
void mn_set_target_rpm(uint16_t target_rpm)
{
	/* Do we need to stop cooler? */
	if (0x00U == target_rpm)
	{
		if (0x00 == (mn_flags0 & _BV(MN_F0_DISABLE_RPM_LOOP)))
		{
			/* Yes */
			hal_set_cooler_duty_cycle(0x00);
			mn_flags0 |= _BV(MN_F0_DISABLE_RPM_LOOP);
		}
		return;
	}

	if (target_rpm > mn_max_rpm)
	{
		target_rpm = mn_max_rpm;
	}
	else
	{
		if (target_rpm < mn_min_rpm)
		{
			target_rpm = mn_min_rpm;
		}
	}

	mn_target_rpm = target_rpm;

	/* Do we need to start cooler? */
	if ((mn_flags0 & _BV(MN_F0_DISABLE_RPM_LOOP)) != 0x00U)
	{
		/* Yes */
		hal_set_cooler_duty_cycle(mn_min_duty_cycle);

		mn_seconds_delay(MN_WAIT_BEFORE_RPM_LOOP);

		/* Activating loop */
		mn_flags0 &= ~_BV(MN_F0_DISABLE_RPM_LOOP);
	}
}

/**
 * Call this function to set target RPM in percents.
 * @param target_rpm Target RPM in percents:
 * 0x00 - cooler stopped;
 * 0x01 - mn_min_rpm;
 * 0x100 - mn_max_rpm.
 * Values higher than 0x100 is not checked.
 */
void mn_set_rpm_in_percents(uint16_t target_rpm)
{
	uint8_t rpm;

	if (0 == target_rpm)
	{
		rpm = 0;
	}
	else
	{
		rpm = (((uint32_t)(target_rpm - 1) * (uint32_t)(mn_max_rpm - mn_min_rpm)) >> 8) + mn_min_rpm;
	}

	mn_set_target_rpm(rpm);
}

/**
 * This function used to convert ADC value to the desired RPM. For details see EEPROM STRUCTURE section.
 * @param tmes Measured temperature (ADC value).
 */
uint16_t mn_lookup_rpm(uint16_t tmes)
{
	uint16_t rpm = 0; /* Desired RPM */
	uint16_t temp = eeprom_read_word(MN_LOOKUP_ABL_ADDRESS); /* Temperature (reading base ADC level) */

	uint8_t *eeprom_lookup_ptr = (uint8_t*)MN_LOOKUP_FIRST_VALUE_ADDRESS;

	uint8_t lookup_byte; /* RRRR TTTT */
	while(tmes > temp)
	{
		/* Reading lookup value */
		lookup_byte = eeprom_read_byte(eeprom_lookup_ptr);
		eeprom_lookup_ptr ++;

		if (eeprom_lookup_ptr > E2END)
		{
			/* Probably EEPROM unprogrammed */
			return 0x100; /* Maximal value for mn_set_rpm_in_percents() */
		}

		temp += (lookup_byte & 0b00001111);
		rpm += (lookup_byte & 0b11110000) >> 4;
	}

	return rpm;
}






