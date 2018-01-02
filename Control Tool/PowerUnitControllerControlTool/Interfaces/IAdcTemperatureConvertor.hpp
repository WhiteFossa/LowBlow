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

#ifndef IADCTEMPERATURECONVERTOR_HPP
#define IADCTEMPERATURECONVERTOR_HPP

#include <QObject>
#include <cstdint>
#include <QXmlStreamWriter>

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
			 * @brief Load conversion settings from given file. File may contain only settings, or another data too.
			 * @param filename Full path to file with settings
			 */
			virtual void LoadSettings(QString filename) = 0;

			/**
			 * @brief Save conversion settings to given file. If file already exists it will be overwritten.
			 * @param filename Full path to file where settings will be saved
			 */
			virtual void SaveSettings(QString filename) = 0;

			/**
			 * @brief Writes ADC to temperature convertor settings section into given XML writer. Use this method to embed
			 * ADC -> Temperature settings into existing XML file.
			 * @param writer Pointer to XML writer.
			 */
			virtual void WriteADC2TemperatureSection(QXmlStreamWriter* writer) = 0;
	};
}

#endif // IADCTEMPERATURECONVERTOR_HPP
