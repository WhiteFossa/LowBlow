#include <Implementations/SettingsStep.hpp>

SettingsStep::SettingsStep(Interfaces::IAdcTemperatureConvertor *conv)
{
  this->conv = conv;

  this->_RPMDelta = 0;
  this->_ADCDelta = MIN_ADC_DELTA;
  this->_CurrentADC = 0;
  this->_CurrentRPM = 0;
}

void SettingsStep::CalculatePrimaryValues(uint previous_adc, uint previous_rpm)
{
  this->_CurrentADC = previous_adc + this->_ADCDelta;
  this->_CurrentRPM = previous_rpm + this->_RPMDelta;

  if (this->_CurrentADC > ADC_MAX_VALUE)
  {
    this->_CurrentADC = ADC_MAX_VALUE;
  }

  if (this->_CurrentRPM > MAX_RPM)
  {
    this->_CurrentRPM = MAX_RPM;
  }
}

QString SettingsStep::GetCurrentRPMPercentsString()
{
  if (0 == this->_CurrentRPM)
  {
    return QObject::trUtf8("Off");
  }

  return QString::number(this->GetCurrentRPMPercents(), 'f', 2);
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

  this->_ADCDelta = delta;

}

uint SettingsStep::GetADCDelta()
{
  return this->_ADCDelta;
}

double SettingsStep::GetTempDelta()
{
  return this->conv->ADC2TEMP(this->_ADCDelta);
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

  this->_RPMDelta = delta;

}

uint SettingsStep::GetRPMDelta()
{
  return this->_RPMDelta;
}

uint SettingsStep::GetCurrentADC()
{
  return this->_CurrentADC;
}

uint SettingsStep::GetCurrentRPM()
{
  return this->_CurrentRPM;
}

double SettingsStep::GetRPMDeltaPercents()
{
  return this->_RPMDelta / RPM_STEPS_FOR_1_PERCENT;
}

double SettingsStep::GetCurrentTemperature()
{
  return this->conv->ADC2TEMP(this->_CurrentADC);
}

double SettingsStep::GetCurrentRPMPercents()
{
  if (0 == this->_CurrentRPM)
  {
    return 0;
  }

  return (this->_CurrentRPM - 1) / RPM_STEPS_FOR_1_PERCENT;
}

