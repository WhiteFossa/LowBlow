﻿/*
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
*/

#include <Implementations/EEPROMGenerator.hpp>

QByteArray EEPROMGenerator::GetEEPROMContents(Interfaces::ISettingsGenerator* setGenPtr)
{
	uint eepromSize = BaseLevelsSize + Interfaces::ISettingsGenerator::StepsNumber;

	QByteArray result;

	result.resize(eepromSize);

	// Base temperature
	uint16_t baseTemperature = (uint16_t)setGenPtr->GetBaseTemperatureADC();
	result[BaseTemperatureLSBAddr] = baseTemperature & BaseTemperatureLSBMask;
	result[BaseTemperatureMSBAddr] = baseTemperature & BaseTemperatureMSBMask;

	// Base RPMs
	result[BaseRPMsAddr] = (uint8_t)((int)setGenPtr->GetBaseRPM() + BaseRPMsShift);

	// Steps
	for (uint stepIndex = 0; stepIndex < Interfaces::ISettingsGenerator::StepsNumber; stepIndex ++)
	{
		auto step = setGenPtr->GetStepPtrRelative(stepIndex);

		uint8_t adcDelta = (uint8_t)((int)step->GetADCDelta() + ADCDeltaShift) & HalfByteMaskL;
		uint8_t RPMsDelta = (uint8_t)step->GetRPMDelta() & HalfByteMaskL;

		result[stepIndex + BaseLevelsSize] = (uint8_t)((RPMsDelta << RPMsDeltaLShift) | adcDelta);
	}

	return result;
}

