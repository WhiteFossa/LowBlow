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
	* Save file Multiplicative (i.e. _a) attribute name
	*/
#define A2T_SETTINGS_MULTIPLICATIVE_EL "Multiplicative"

/**
	* Save file Additive (i.e. _b) attribute name
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

		void SetADC2TempConversionFactors(double _a, double _b);

		void GetADC2TempConversionFactors(double *_a, double *_b);

		void LoadSettings(QString filename);

		void SaveSettings(QString filename);

		void SetDescription(QString descr);

		QString GetDescription();

		void WriteADC2TemperatureSection(QXmlStreamWriter* writer);

	private:
		/**
		 * @brief Description of this instance (i.e. conversion settings)
		 */
		QString description;

		/**
		 * @brief a Factor for ADC->Temperature conversion. Tc = a * ADC + b, where Tc - Temperature in Celsius, ADC - ADC measurement.
		 */
		double _a;

		/**
		 * @brief a Factor for ADC->Temperature conversion. Tc = a * ADC + b, where Tc - Temperature in Celsius, ADC - ADC measurement.
		 */
		double _b;

};

#endif // ADCTEMPERATURECONVERTOR_HPP
