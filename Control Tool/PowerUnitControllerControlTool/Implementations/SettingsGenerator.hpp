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

#ifndef SETTINGSGENERATOR_HPP
#define SETTINGSGENERATOR_HPP

#include <Interfaces/ISettingsStep.hpp>
#include <Interfaces/ISettingsGenerator.hpp>
#include <Auxiliary.hpp>

/**
 * @brief Class, being used to generate settings (see Interfaces/ISettingsGenerator for information)
 */
class SettingsGenerator : public Interfaces::ISettingsGenerator
{
	Q_OBJECT

	public:
		/**
		 * @brief Total steps number.
		 */

		/**
		 * @brief Step with this index stores zero levels.
		 */
		static const uint ZeroLevelsStepIndex = 0;

		/**
		 * @brief Step with this index stores base levels.
		 */
		static const uint BaseLevelsStepIndex = 1;

		/**
		 * @brief Constructor. Initializes steps list, default base temperature and so on.
		 * @param convertor - Pointer to instance of AdcTemperatureConvertor class.
		 */
		SettingsGenerator(Interfaces::IAdcTemperatureConvertor* convertor);

		/**
		 * @brief Destructor
		 */
		~SettingsGenerator();

		void InitializeStepsList(uint steps_number);


		void SetBaseTemperature(double btemp);
		void SetBaseTemperatureADC(uint adc);
		double GetBaseTemperature();
		uint GetBaseTemperatureADC();

		void SetBaseRPM(uint base_rpm);
		uint GetBaseRPM();

		void CalculateSteps();

		Interfaces::ISettingsStep* GetStepPtr(uint step);
		Interfaces::ISettingsStep* GetStepPtrRelative(uint step);

	protected:

	private:
		Interfaces::IAdcTemperatureConvertor* _ADC2TempConvertor = NULL;

		/**
		 * @brief _steps - List of pointers to RPM and temperature change steps.
		 */
		QList <Interfaces::ISettingsStep*> _steps;

		/**
		 * @brief _baseADC Base temperature ADC level.
		 */
		uint _baseADC;

		/**
		 * @brief _baseRPM - Base RPM level. See SetBaseRPM() method description in ISettingsGenerator for details.
		 */
		uint _baseRPM;

		/**
		 * @brief Call this method to destroy steps, contained in _steps. _steps size will not be changed, but pointers will be NULL.
		 */
		void DestroySteps();

};

#endif // SETTINGSGENERATOR_HPP
