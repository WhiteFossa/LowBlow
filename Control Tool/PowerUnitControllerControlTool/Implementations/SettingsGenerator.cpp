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
*/

#include <Implementations/SettingsGenerator.hpp>
#include <Implementations/SettingsStep.hpp>

SettingsGenerator::SettingsGenerator(Interfaces::IAdcTemperatureConvertor* convertor)
{
	_ADC2TempConvertor = convertor;

	// Setting up steps number
	InitializeStepsList(StepsNumber);
}

void SettingsGenerator::InitializeStepsList(uint steps_number)
{
	// Destroying old ones
	DestroySteps();

	_steps.clear();

	// And creating a new ones
	for (uint i = 0; i < steps_number + AdditionalSteps; i++)
	{
		SettingsStep* step = new SettingsStep(_ADC2TempConvertor);
		_steps.push_back(step);
	}

	// Calculating steps data
	CalculateSteps();
}

void SettingsGenerator::SetBaseTemperature(double btemp)
{
	_baseADC = _ADC2TempConvertor->TEMP2ADC(btemp);

	CalculateSteps(); // Recalculating
}

void SettingsGenerator::SetBaseTemperatureADC(uint adc)
{
	_baseADC = adc;
	CalculateSteps();
}

double SettingsGenerator::GetBaseTemperature()
{
	return _ADC2TempConvertor->ADC2TEMP(_baseADC);
}

uint SettingsGenerator::GetBaseTemperatureADC()
{
	return _baseADC;
}

void SettingsGenerator::SetBaseRPM(uint base_rpm)
{
	_baseRPM = base_rpm;

	CalculateSteps(); // Recalculating
}

uint SettingsGenerator::GetBaseRPM()
{
	return _baseRPM;
}

void SettingsGenerator::DestroySteps()
{
	// Destroying all steps
	for (QList<Interfaces::ISettingsStep*>::iterator i = _steps.begin(); i != _steps.end(); ++i)
	{
		SafeDelete(*i);
	}
}

void SettingsGenerator::CalculateSteps()
{
	// Zero levels step
	Interfaces::ISettingsStep *stepPtr = _steps[ZeroLevelsStepIndex];

	stepPtr->SetADCDelta(0, true);
	stepPtr->SetRPMDelta(0);
	stepPtr->CalculatePrimaryValues(0, 0);

	// Base levels step
	stepPtr = _steps[BaseLevelsStepIndex];
	stepPtr->SetADCDelta(_baseADC, true);
	stepPtr->SetRPMDelta(_baseRPM, true);

	// Iterating through steps (except first)
	for (int i = 1; i < _steps.size(); i++)
	{
		// Previous element
		Interfaces::ISettingsStep* prev = _steps[i - 1];
		_steps[i]->CalculatePrimaryValues(prev->GetCurrentADC(), prev->GetCurrentRPM());
	}
}

Interfaces::ISettingsStep* SettingsGenerator::GetStepPtr(uint step)
{
	return _steps[step];
}

Interfaces::ISettingsStep* SettingsGenerator::GetStepPtrRelative(uint step)
{
	return _steps[step + AdditionalSteps];
}

SettingsGenerator::~SettingsGenerator()
{
	DestroySteps();
}
