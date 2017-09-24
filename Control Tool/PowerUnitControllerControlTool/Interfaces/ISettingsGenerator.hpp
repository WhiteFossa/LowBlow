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

#ifndef ISETTINGSGENERATOR_HPP
#define ISETTINGSGENERATOR_HPP

#include <QObject>
#include <Interfaces/ISettingsStep.hpp>
#include <cstdint>

namespace Interfaces
{
  /**
   * @brief Class, containing all settings steps. It being used to generate settings table, which will be converted to EEPROM data
   */
  class ISettingsGenerator : public QObject
  {
     Q_OBJECT

    // Public members
    public:
      // Public fields

      // Public methods
    /**
       * @brief Call this method to initialize steps list. It must be called before any other methods. Implementation must be ready to provide
       * the instance of IAdcTemperatureConvertor to steps.
       * @param steps_number How many steps we have
       */
      virtual void InitializeStepsList(uint steps_number) = 0;

    /**
       * @brief SetBaseTemperature - Call it to set base temperature (in Celsius). Please note, that all steps will be recalculated afterwards.
       * @param btemp Base temperature in Celsius
       */
      virtual void SetBaseTemperature(double btemp) = 0;

    /**
       * @brief GetBaseTemperature - Call it to get base temperature (in Celsius). Please note, that base temperature returned by this method MAY DIFFER from that, what
       * was set by SetBaseTemperature(). It is because implementation may store ADC value rather than temperature.
       * @return Base temperature in Celsius
       */
      virtual double GetBaseTemperature() = 0;


    /**
       * @brief GetBaseTemperatureADC Like GetBaseTemperature(), but returns ADC code for it.
       * @return ADC code for current base temperature.
       */
      virtual uint GetBaseTemperatureADC() = 0;


    /**
       * @brief SetBaseRpm Call this method to set base RPM. Base RPM - is a RPM at base temperature. Please note, that all steps will be recalculated afterwards.
       * @param base_rpm - Base RPM value. 0x00 - cooler is stopped. 0x01 - minimal possible RPM. 0x100 - maximal possible RPM.
       */
      virtual void SetBaseRPM(uint base_rpm) = 0;

    /**
       * @brief GetBaseRPM - Returns base RPM level. See SetBaseRPM() for details.
       * @return Base RPM level.
       */
      virtual uint GetBaseRPM() = 0;

    /**
       * @brief CalculateSteps Call this method to recalculate step ADC levels and RPMs
       */
      virtual void CalculateSteps() = 0;

    /**
       * @brief Returns pointer to step with given number to allow manipulations with step. Don't forget to call CalculateSteps() after making any changes.
       * @param step Step number
       * @return Pointer to step
       */
      virtual Interfaces::ISettingsStep* GetStepPtr(uint step) = 0;


    // Protected members
    protected:
      // Protected fields

      // Protected methods

  };

}

#endif // ISETTINGSGENERATOR_HPP
