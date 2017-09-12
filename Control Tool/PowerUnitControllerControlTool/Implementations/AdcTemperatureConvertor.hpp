#ifndef ADCTEMPERATURECONVERTOR_HPP
#define ADCTEMPERATURECONVERTOR_HPP

#include <cmath>
#include <Auxiliary.hpp>
#include <Interfaces/IAdcTemperatureConvertor.hpp>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttribute>
#include <QException>
#include <QDebug>

/**
  * Maximal value of thermometer ADC
  */
#define ADC_MAX_VALUE 1023

/**
  * Save file root element
  */
#define A2T_SETTINGS_ROOT_ELEMENT "ADC2TempSettings"

/**
  * Save file Device attribute name
  */
#define A2T_SETTINGS_DEVICE_ATTR "Device"

/**
 * Device name for use in saved settings
 */
#define A2T_SETTINGS_DEVICE_NAME "TermalController_mk1"

/**
  * Save file Version attribute name
  */
#define A2T_SETTINGS_VERSION_ATTR "Version"

/**
 * Version of settings file
 */
#define A2T_SETTINGS_VERSION "0"

/**
  * Save file Description attribute name
  */
#define A2T_SETTINGS_DESCRIPTION_EL "Description"

/**
  * Save file Multiplicative (i.e. this->a) attribute name
  */
#define A2T_SETTINGS_MULTIPLICATIVE_EL "Multiplicative"

/**
  * Save file Additive (i.e. this->b) attribute name
  */
#define A2T_SETTINGS_ADDITIVE_EL "Additive"

/**
 * @brief Implementation of Interfaces::IAdcTemperatureConvertor
 */
class AdcTemperatureConvertor : public Interfaces::IAdcTemperatureConvertor
{
  Q_OBJECT

  public:
    /**
     * @brief Constructor. Call SetADC2TempConversionFactors() or LoadSettings() after class construction
     */
    AdcTemperatureConvertor();


    double ADC2TEMP(uint adc);

    uint TEMP2ADC(double temp);

    void SetADC2TempConversionFactors(double a, double b);

    void GetADC2TempConversionFactors(double *a, double *b);

    void LoadSettings(QString filename);

    void SaveSettings(QString filename);

    void SetDescription(QString descr);

    QString GetDescription();

  private:
    /**
     * @brief Description of this instance (i.e. conversion settings)
     */
    QString description;

    /**
     * @brief a Factor for ADC->Temperature conversion. Tc = a * ADC + b, where Tc - Temperature in Celsius, ADC - ADC measurement.
     */
    double a;

    /**
     * @brief a Factor for ADC->Temperature conversion. Tc = a * ADC + b, where Tc - Temperature in Celsius, ADC - ADC measurement.
     */
    double b;

};

#endif // ADCTEMPERATURECONVERTOR_HPP
