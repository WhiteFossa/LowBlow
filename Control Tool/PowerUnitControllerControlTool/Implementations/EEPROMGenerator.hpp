/*
Copyright 2017 White Fossa aka Artyom Vetrov.

This file is part of project "LowBlow" (advanced programmable cooler controller).

All parts of "LowBlow" is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

"LowBlow" project files is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with project "LowBlow" files. If not, see <http://www.gnu.org/licenses/>.

---

Part of "LowBlow" project.

Feel free to contact: whitefossa@gmail.com

Repository: https://github.com/WhiteFossa/LowBlow

If repository above is not available, try my LiveJournal: fossa-white.livejournal.com
or as last measure try to search for #WhiteFossa hashtag.
*/

/*
 * See EEPROM Structure.odt for detains on EEPROM structure.
*/

#ifndef EEPROMGENERATOR_HPP
#define EEPROMGENERATOR_HPP

#include <Interfaces/IEEPROMGenerator.hpp>

/**
 * @brief Class to generate EEPROM
 */
class EEPROMGenerator : public Interfaces::IEEPROMGenerator
{
	public:
		QByteArray GetEEPROMContents(Interfaces::ISettingsGenerator* setGenPtr);

	protected:

	private:

		/**
		 * @brief Amount of EEPROM, taken by base levels.
		 */
		const uint BaseLevelsSize = 3U;

		/**
		 * @brief EEPROM address of base temperature least signigicant byte.
		 */
		const uint BaseTemperatureLSBAddr = 0U;

		/**
		 * @brief EEPROM address of base temperature most significant byte.
		 */
		const uint BaseTemperatureMSBAddr = 1;

		/**
		 * @brief And this mask with base temperature to get base temperature LSB.
		 */
		const uint BaseTemperatureLSBMask = 0xFFU;

		/**
		 * @brief And this mask with base temperature to get base temperature MSB.
		 */
		const uint BaseTemperatureMSBMask = 0xFF00U;

		/**
		 * @brief Shift base temperature & BaseTemperatureMSBMask right to this value to get base temperature MSB.
		 */
		const uint BaseTemperatureMSBRShift = 8U;

		/**
		 * @brief EEPROM address of base RPMs.
		 */
		const uint BaseRPMsAddr = 2U;

		/**
		 * @brief Add this shift to base RPMs before storing it in EEPROM. Read EEPROM Structure.odt for details.
		 */
		const int BaseRPMsShift = -1;

		/**
		 * @brief Add this shift to step's ADC delta before storing it in EEPROM. Read EEPROM Structure.odt for details.
		 */
		const int ADCDeltaShift = -1;

		/**
		 * @brief Mask for 4 least significant bits.
		 */
		const uint HalfByteMaskL = 0b1111U;

		/**
		 * @brief Shift RPMs delta left to this amount of bits before combining it with ADC delta.
		 */
		const uint RPMsDeltaLShift = 4U;
};

#endif // EEPROMGENERATOR_HPP
