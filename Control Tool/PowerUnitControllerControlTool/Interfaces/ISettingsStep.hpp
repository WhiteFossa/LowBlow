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

#ifndef ISETTINGSSTEP_HPP
#define ISETTINGSSTEP_HPP

#include <QObject>
#include <QString>
#include <cstdint>
#include "Implementations/AdcTemperatureConvertor.hpp"

namespace Interfaces
{
  /**
   * @brief Class with one settings step (i.e. with temperature and RPM percent increments)
   */
  class ISettingsStep : public QObject
  {
     Q_OBJECT

    // Public members
    public:

      // Public methods

      /**
       * @brief Calculates primary values (i.e. current ADC level and current RPM) basing on previus step values.
       * If resulting ADC / RPM levels will be greater than ADC_MAX_VALUE / MAX_RPM respectively, then levels will be
       * set to ADC_MAX_VALUE / MAX_RPM
       * @param previous_adc Previous step ADC value
       * @param previous_rpm Previous step RPM value
       */
      virtual void CalculatePrimaryValues(uint previous_adc, uint previous_rpm) = 0;

      /**
       * @brief Returns "Off" if this->_CurrentRPM == 0, and string representation of this->GetCurrentRPMPercents() otherwise.
       * @return "Off" or current RPMs as string with percents
       */
      virtual QString GetCurrentRPMPercentsString() = 0;

      /**
       * @brief Call it to set this->adc_delta value. If delta more than MAX_ADC_DELTA (defined in descendant) set it to MAX_ADC_DELTA,
       * if it less then MIN_ADC_DELTA - set it to MIN_ADC_DELTA
       * @param delta This value added to ADC value when we are going to next step (see Firmware/main.h EEPROM STRUCTURE section for description)
       * @param skipLimitsCheck If set to true, then checks for MIN_ADC_DELTA and MAX_ADC_DELTA will be skipped. Us it only for setting base level,
       * because values outside [MIN_ADC_DELTA-MAX_ADC_DELTA] can't be programmed into MCU.
       */
      virtual void SetADCDelta(uint delta, bool skipLimitsCheck = false) = 0;

      /**
       * @brief Returns current ADC delta
       * @return ADC delta for this step
       */
      virtual uint GetADCDelta() = 0;

      /**
       * @brief As GetADCDelta(), but returns current ADC delta, converted to Celsius
       * @return Celsuis representation of ADC delta
       */
      virtual double GetTempDelta() = 0;

      /**
       * @brief Like SetADCDelta(), but for RPM delta. Making check for MAX_RPM_DELTA, if delta is greater then it, delta will be MAX_RPM_DELTA.
       * @param delta This value added to RPM raw value when we are going to next step (see Firmware/main.h EEPROM STRUCTURE section for description)
       */
      virtual void SetRPMDelta(uint delta, bool skipLimitsCheck = false) = 0;

      /**
       * @brief As GetADCDelta(), but for RPM delta
       * @return  RPM delta for this step
       */
      virtual uint GetRPMDelta() = 0;

      /**
       * @brief As GetRPMDelta(), but in percents
       * @return RPM delta for this step in percents
       */
      virtual double GetRPMDeltaPercents() = 0;

      /**
       * @brief Returns ADC level for this step
       * @return ADC level for this step
       */
      virtual uint GetCurrentADC() = 0;

      /**
       * @brief Get current RPM setting (see Interfaces/ISettingsGenerator.hpp for details)
       * @return Current step RPM setting
       */
      virtual uint GetCurrentRPM() = 0;

      /**
       * @brief As GetCurrentADC(), but returns temperature in Celsius
       * @return Current step temperature in Celsius
       */
      virtual double GetCurrentTemperature() = 0;

      /**
       * @brief As GetCurrentRPM(), but returns value in percents
       * @return Current step RPM setting in percents
       */
      virtual double GetCurrentRPMPercents() = 0;



    // Protected members
    protected:
      /**
       * @brief ADC<->Temperature convertor
       */
      Interfaces::IAdcTemperatureConvertor *conv = 0;
  };

}

#endif // ISETTINGSSTEP_HPP
