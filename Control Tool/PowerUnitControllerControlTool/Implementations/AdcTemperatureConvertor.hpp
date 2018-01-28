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
#include <QString>

/**
 * @brief Implementation of Interfaces::IAdcTemperatureConvertor
 */
class AdcTemperatureConvertor : public Interfaces::IAdcTemperatureConvertor
{
	Q_OBJECT

	public:
		/**
		 * @brief Maximal ADC value.
		 */
		static const uint MaxADCValue = 1023;

		/**
		 * @brief Constructor. Call SetADC2TempConversionFactors() or LoadSettings() after class construction
		 */
		AdcTemperatureConvertor();


		double ADC2TEMP(uint adc);

		uint TEMP2ADC(double temp);

		void SetADC2TempConversionFactors(double _a, double _b);

		void GetADC2TempConversionFactors(double *_a, double *_b);

		bool LoadSettings(QString filename, QString prefix);

		void SaveSettings(QString filename);

		void SetDescription(QString descr);

		QString GetDescription();

		void WriteADC2TemperatureSection(QXmlStreamWriter* writer);

	private:


		/**
		* This element will be root XML element for ADC to temperature settings file.
		*/
		const QString SettingsRootElement = "ADC2TempSettings";

		/**
		 * @brief Store device name in attribute with this name.
		 */
		const QString DeviceAttribute = "Device";

		/**
		 * @brief Device name for settings file.
		 */
		const QString DeviceName = "LowBlow";

		/**
		 * @brief Store settings version in attribute with this name.
		 */
		const QString VersionAttribute = "Version";

		/**
		 * @brief Settings file version.
		 */
		const uint Version = 1;

		/**
		 * @brief Store settings description in this XML element.
		 */
		const QString DescriptionElement = "Description";

		/**
		 * @brief Store multiplicative factor in this element.
		 */
		const QString	MultiplicativeElement = "Multiplicative";

		const QString AdditiveElement = "Additive";

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
