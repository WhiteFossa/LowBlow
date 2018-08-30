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

	main.h

	Main header file

	Created 2016/02/07 by White Fossa, also known as Artyom Vetrov

	Feel free to contact: fossa-artem@mail.ru

	Repository: http://home.whitefossa.org/svn/PowerUnitCoolerController/

	If repository above is not available, try my LiveJournal:
	fossa-white.livejournal.com

	or as last measure try to search for #WhiteFossa hashtag.

	-------------------------------------------------------------------------
 */

#ifndef MAIN_H_
#define MAIN_H_

/**
 * Main header file
 */

/* Includes */

#include "hal.h" /* HAL */
#include <avr/eeprom.h> /* EEPROM utilities */

/**
 * EEPROM STRUCTURE
 *
 * We have 128 bytes of EEPROM
 *
 * First 2 bytes (word) - ADC base level (ABL) AKA base temperature, in ADC code. Temperature values are relative to this base value.
 * MN_LOOKUP_ABL_ADDRESS points to EEPROM address of ABL.
 *
 * Then - base RPMs, 1 byte, i.e. RPMs at base temperature. Obviously it can't be "off", so actual base RPMs is this value + MN_BRPM_INCREMENT.
 * MN_LOOKUP_BRPM_ADDRESS points to it.
 *
 * Then, temperature/RPM lookup table follows. Each lookup packed into 1 byte as follow:
 * RRRRTTTT, where RRRR is 4-bit RPM delta, and TTTT is 4-bit temperature delta.
 *
 * Temperature delta can't be 0, so actually we have TTTT+1, i.e. 0000 means 1.
 *
 * MN_LOOKUP_FIRST_VALUE_ADDRESS points to EEPROM address of the first lookup value.
 *
 * When looking up for RPM for given temperature the next algorithm is used:
 * 1) Declare RPM and Temperature variables;
 * 2) If temperature < ABL, then RPM = 0 and exit;
 * 3) RPM = Base RPM, Temperature = ABL (ADC base level);
 * 4) EEPROM address points to the first lookup table byte;
 * 5) If measured temperature <= Temperature, then return RPM and exit;
 * 6) Read lookup byte, parse it, RPM += RPM delta; Temperature += Temperature delta + MN_LOOKUP_TEMPERATURE_INCREMENT;
 * 7) Increment EEPROM address;
 * 8) Goto 5.
 *
 * Temperature here is the ADC value, and RPM is the argument of mn_set_rpm_in_percents() function.
 */

/* Constants */

/** When powered on controller will turn cooler off and then will wait until cooler stopped. If no tachometer pulses
 * came during given (in seconds) interval, cooler treated as stopped.
 * */
#define MN_WAIT_FOR_COOLER_STOP 0x01U

/**
 * If this flag set to 1, then duty cycle adjustment will be disabled.
 */
#define MN_F0_DISABLE_RPM_LOOP 0x00

/**
 * When determining minimal duty cycle we starting from duty cycle = HAL_MIN_DUTY_CYCLE and then increasing it every
 * hal_second_tick() until cooler reach minimal RPM. This constant defines duty cycle increment if cooler not started yet.
 */
#define MN_COOLER_SPIN_UP_DUTY_CYCLE_STEP 0x01U

/**
 * When powered on, we starting from minimal duty cycle and then increasing it until next tachometer measurement will me
 * greater or equal to this constant. When it occurs firmware believe that cooler started and spinning stable.
 * For 2 pulses per revolution cooler we have 1 = 30 RPM.
 */
#define MN_COOLER_SPIN_UP_MINIMAL_RPM 0x04U

/**
 * Wait this time (in seconds) before remembering RPM at spin-up stage
 */
#define MN_COOLER_SPIN_UP_WAIT 0x05U

/**
 * Wait this time (in seconds) when starting cooler before enabling RPM loop
 */
#define MN_WAIT_BEFORE_RPM_LOOP 0x03U

/**
 *  EEPROM address of ABL (ADC base level).
 */
#define MN_LOOKUP_ABL_ADDRESS 0x00U

/**
 * EEPROM address of base RPMs.
 */
#define MN_LOOKUP_BRPM_ADDRESS 0x02U

/**
 * EEPROM address of the first temperature to RPM lookup value.
 */
#define MN_LOOKUP_FIRST_VALUE_ADDRESS 0x03U

/**
 * Actual base RMPs is value at EEPROM[MN_LOOKUP_BRPM_ADDRESS] + this value.
 */
#define MN_BRPM_INCREMENT 0x01U

/**
 * Full throttle for cooler. Maximal value for mn_set_rpm_in_percents().
 */
#define MN_FULL_THROTTLE 0x100U

/**
 * Actual temperature increment per step is value from EEPROM low halfbyte + this value.
 */
#define MN_LOOKUP_TEMPERATURE_INCREMENT 0x01U


/* End of constants */

/* Variables */

/**
 * Flags variable 0
 */
volatile uint8_t mn_flags0;

/**
 * Used as counter in in mn_seconds_delay(). Decreases every second, then stays at zero
 */
volatile uint8_t mn_seconds_delay_counter;

/**
 *  Minimal RPM for given cooler
 */
volatile uint16_t mn_min_rpm;

/**
 * Duty cycle, for which mn_min_rpm reached
 */
volatile uint8_t mn_min_duty_cycle;

/**
 * Maximal RPM for given cooler
 */
volatile uint16_t mn_max_rpm;

/**
 * If RPM control loop is enabled, then it will try to get this RPM.
 * DO NOT SET IT MANUALLY, USE mn_set_target_rpm() INSTEAD!
 */
volatile uint16_t mn_target_rpm;


/* End of variables */



/* Functions prototypes */

/**
 * Function waits given number of seconds.
 */
void mn_seconds_delay(uint8_t num_of_seconds);

/**
 *  This function gets called every second (from hal_second_tick())
 */
void mn_second_tick();

/**
 * Call this function to set target RPM for cooler.
 *
 * @param target_rpm For 2 tachometer pulses per revolution (i.e. common cooler) 1 = 30 RPM.
 * If set to 0, then cooler will be stopped and RPM loop disabled.
 * If not 0, then cooler will be started and RPM loop enabled.
 * If set higher than mn_max_rpm - will be treated as mn_max_rpm.
 * If set lower than mn_min_rpm but higher then 0 - will be treated as mn_min_rpm.
 */
void mn_set_target_rpm(uint16_t target_rpm);

/**
 * Stops cooler and disables RPMs feedback loop.
 */
void mn_stop_cooler();

/**
 * Call this function to set target RPM in percents.
 * @param target_rpm Target RPM in percents:
 * 0x00 - cooler stopped;
 * 0x01 - mn_min_rpm;
 * 0x100 - mn_max_rpm.
 * Values higher than 0x100 is not checked.
 */
void mn_set_rpm_in_percents(uint16_t target_rpm);

/**
 * This function used to convert ADC value to the desired RPM. For details see EEPROM STRUCTURE section.
 * @param tmes Measured temperature (ADC value).
 */
uint16_t mn_lookup_rpm(uint16_t tmes);

/* End of function prototypes */

#endif /* MAIN_H_ */
