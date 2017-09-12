#ifndef SETTINGSSTEP_HPP
#define SETTINGSSTEP_HPP

#include <Interfaces/ISettingsStep.hpp>

/**
  * Minimal increment of ADC value per step (at least 1 to avoid multiple steps for the same temperature)
  */
#define MIN_ADC_DELTA 1

/**
  * Maximal increment of ADC value per step (we have 4 bits, but one is added inside MCU firmware, so [1-16] here will be translated to [0-15] in resulting HEX-file)
  */
#define MAX_ADC_DELTA 16

/**
  * Like MAX_ADC_DELTA, but for RPM
  */
#define MAX_RPM_DELTA 15

/**
  * Number of steps per 1% (NOT RPM value for 1%)
  */
#define RPM_STEPS_FOR_1_PERCENT 2.55

/**
  * Maximal possible RPM
  */
#define MAX_RPM 0x100

/**
 * @brief Class with one settings step (see Interfaces/ISettingsStep for information)
 */
class SettingsStep : public Interfaces::ISettingsStep
{
  Q_OBJECT

  public:
    SettingsStep(Interfaces::IAdcTemperatureConvertor *conv);

    void CalculatePrimaryValues(uint previous_adc, uint previous_rpm);

    QString GetCurrentRPMPercentsString();


    void SetADCDelta(uint delta, bool skipLimitsCheck = false);
    uint GetCurrentADC();
    double GetCurrentTemperature();
    uint GetADCDelta();
    double GetTempDelta();


    void SetRPMDelta(uint delta, bool skipLimitsCheck = false);
    uint GetCurrentRPM();
    double GetCurrentRPMPercents();
    uint GetRPMDelta();
    double GetRPMDeltaPercents();


  private:

    /**
     * @brief ADC value for this step
     */
    uint _CurrentADC;

    /**
     * @brief _ADCDelta Current ADC delta
     */
    uint _ADCDelta;

    /**
     * @brief RPM for this step
     */
    uint _CurrentRPM;


    /**
     * @brief Current RPM delta
     */
    uint _RPMDelta;


};

#endif // SETTINGSSTEP_HPP
