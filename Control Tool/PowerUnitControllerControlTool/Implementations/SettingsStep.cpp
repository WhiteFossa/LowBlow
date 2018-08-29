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

#include <Implementations/SettingsStep.hpp>

SettingsStep::SettingsStep(Interfaces::IAdcTemperatureConvertor *conv)
{
	_conv = conv;

	_RPMDelta = 0;
	_ADCDelta = MIN_ADC_DELTA;
	_CurrentADC = 0;
	_CurrentRPM = 0;
}

void SettingsStep::CalculatePrimaryValues(uint previous_adc, uint previous_rpm)
{
	_CurrentADC = previous_adc + _ADCDelta;
	_CurrentRPM = previous_rpm + _RPMDelta;

	if (_CurrentADC > AdcTemperatureConvertor::MaxADCValue)
	{
		_CurrentADC = AdcTemperatureConvertor::MaxADCValue;
	}

	if (_CurrentRPM > MAX_RPM)
	{
		_CurrentRPM = MAX_RPM;
	}
}

QString SettingsStep::GetCurrentRPMPercentsString()
{
	if (0 == _CurrentRPM)
	{
		return QObject::tr("Off");
	}

	return QString::number(GetCurrentRPMPercents(), 'f', 2);
}

void SettingsStep::SetADCDelta(uint delta, bool skipLimitsCheck)
{
	if (!skipLimitsCheck)
	{
		if (delta < MIN_ADC_DELTA)
		{
			delta = MIN_ADC_DELTA;
		}
		else if (delta > MAX_ADC_DELTA)
		{
			delta = MAX_ADC_DELTA;
		}
	}

	_ADCDelta = delta;

}

uint SettingsStep::GetADCDelta()
{
	return _ADCDelta;
}

double SettingsStep::GetTempDelta()
{
	return _conv->GetTempDelta(0, _ADCDelta);
}

void SettingsStep::SetRPMDelta(uint delta, bool skipLimitsCheck)
{
	if (!skipLimitsCheck)
	{
		if (delta > MAX_RPM_DELTA)
		{
			delta = MAX_RPM_DELTA;
		}
	}

	_RPMDelta = delta;

}

uint SettingsStep::GetRPMDelta()
{
	return _RPMDelta;
}

uint SettingsStep::GetCurrentADC()
{
	return _CurrentADC;
}

uint SettingsStep::GetCurrentRPM()
{
	return _CurrentRPM;
}

double SettingsStep::GetRPMDeltaPercents()
{
	return _RPMDelta / RPM_STEPS_FOR_1_PERCENT;
}

double SettingsStep::GetCurrentTemperature()
{
	return _conv->ADC2TEMP(_CurrentADC);
}

double SettingsStep::GetCurrentRPMPercents()
{
	if (0 == _CurrentRPM)
	{
		return 0;
	}

	return (_CurrentRPM - 1) / RPM_STEPS_FOR_1_PERCENT;
}

