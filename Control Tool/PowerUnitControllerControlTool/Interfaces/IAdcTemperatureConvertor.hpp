#ifndef IADCTEMPERATURECONVERTOR_HPP
#define IADCTEMPERATURECONVERTOR_HPP

#include <QObject>
#include <cstdint>

namespace Interfaces
{
  /**
   * @brief Class, used to convert ADC values to Temperature and vice versa
   */
  class IAdcTemperatureConvertor : public QObject
  {
    Q_OBJECT

    /* Public methods */
    public:
      /**
       * @brief Convert ADC measurement to temperature (Celsius).
       * @param adc ADC measurement
       * @return Temperature in Celsius
       */
      virtual double ADC2TEMP(uint adc) = 0;

      /**
       * @brief Convert temperature in Celsius to ADC measurement. Imitates ADC saturation, i.e. returns 0 if value must be less than 0, and ADC_MAX_VALUE if it must
       * be more than ADC_MAX_VALUE
       * @param temp Temperature in Celsius
       * @return ADC measurement
       */
      virtual uint TEMP2ADC(double temp) = 0;

      /**
       * @brief Call it to set ADC to Temperature conversion factors. Tc = a * ADC + b, where Tc - Temperature in Celsius, ADC - ADC measurement.
       * Reverse conversion factors (i.e. Temperature to ADC will be calculated automatically).
       * @param a Tc = a * ADC + b
       * @param b Tc = a * ADC + b
       */
      virtual void SetADC2TempConversionFactors(double a, double b) = 0;

      /**
       * @brief Returns temperature conversion factors (look SetADC2TempConversionFactors() for details).
       * @param a Tc = a * ADC + b
       * @param b Tc = a * ADC + b
       */
      virtual void GetADC2TempConversionFactors(double *a, double *b) = 0;

      /**
       * @brief Set description of this instance (i.e. for this conversion settings).
       * @param desct Description
       */
      virtual void SetDescription(QString descr) = 0;

      /**
       * @brief Get description of this instance (i.e. for this conversion settings).
       * @return Description
       */
      virtual QString GetDescription() = 0;

      /**
       * @brief Load conversion settings from given file.
       * @param filename Full path to file with settings
       */
      virtual void LoadSettings(QString filename) = 0;

      /**
       * @brief Save conversion settings to given file. If file already exists it will be overwritten.
       * @param filename Full path to file where settings will be saved
       */
      virtual void SaveSettings(QString filename) = 0;
  };
}

#endif // IADCTEMPERATURECONVERTOR_HPP
