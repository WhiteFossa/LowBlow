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

#include <Implementations/AdcTemperatureConvertor.hpp>

/**
 * @brief Constructor. Call SetADC2TempConversionFactors() or LoadSettings() after class construction
 */
AdcTemperatureConvertor::AdcTemperatureConvertor()
{
	_description = QObject::tr("Not initialized yet");
	_a = 0;
	_b = 0;
}

double AdcTemperatureConvertor::ADC2TEMP(uint adc)
{
	return _a * adc + _b;
}

uint AdcTemperatureConvertor::TEMP2ADC(double temp)
{
	int pre_result = (int)floor((temp - _b) / _a + 0.5);
	if(pre_result < 0)
	{
		return 0;
	}
	else if (pre_result > MaxADCValue)
	{
		return MaxADCValue;
	}

	return (uint)pre_result;
}

double AdcTemperatureConvertor::GetTempDelta(uint adc1, uint adc2)
{
	return ADC2TEMP(adc2) - ADC2TEMP(adc1);
}

void AdcTemperatureConvertor::SetADC2TempConversionFactors(double a, double b)
{
	_a = a;
	_b = b;
}

void AdcTemperatureConvertor::GetADC2TempConversionFactors(double *a, double *b)
{
	*a = _a;
	*b = _b;
}


bool AdcTemperatureConvertor::LoadSettings(QString filename, QString prefix)
{
	QFile *file = new QFile(filename);
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		SafeDelete(file);
		return false;
	}

	QXmlQuery query;

	// Binding settings file contents into $settingsFile variable
	query.bindVariable("settingsFile", file);

		// Is it our XML at all?
	query.setQuery(QString(QObject::tr("%1/%2/@%3/string()")).arg(prefix).arg(SettingsRootElement).arg(DeviceAttribute));
	QString deviceName = Fossa::Helpers::XmlHelper::GetTextValue(&query);

	query.setQuery(QString(QObject::tr("%1/%2/@%3/string()")).arg(prefix).arg(SettingsRootElement).arg(VersionAttribute));
	uint version = Fossa::Helpers::XmlHelper::GetIntegerValue(&query);

	if ((deviceName != DeviceName) || (version != Version))
	{
		file->close();
		SafeDelete(file);
		return false;
	}

	// Description
	query.setQuery(QString(QObject::tr("%1/%2/%3/text()")).arg(prefix).arg(SettingsRootElement).arg(DescriptionElement));
	_description = Fossa::Helpers::XmlHelper::GetTextValue(&query);

	// Multiplicative coefficient
	query.setQuery(QString(QObject::tr("%1/%2/%3/text()")).arg(prefix).arg(SettingsRootElement).arg(MultiplicativeElement));
	_a = Fossa::Helpers::XmlHelper::GetDoubleValue(&query);

	// Additive coefficient
	query.setQuery(QString(QObject::tr("%1/%2/%3/text()")).arg(prefix).arg(SettingsRootElement).arg(AdditiveElement));
	_b = Fossa::Helpers::XmlHelper::GetDoubleValue(&query);

	file->close();
	SafeDelete(file);

	return true;
}

void AdcTemperatureConvertor::SaveSettings(QString filename)
{
	QFile *file = new QFile(filename);
	if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
	{
		SafeDelete(file);
		throw std::runtime_error(QString(QObject::tr("Cannot open file \"%1\" for writing")).arg(filename).toStdString());
	}

	QXmlStreamWriter xsw(file); // Stream writer for XML data
	xsw.setAutoFormatting(true);
	xsw.writeStartDocument();
		WriteADC2TemperatureSection(&xsw);
	xsw.writeEndDocument();

	file->close();
	SafeDelete(file);
}

void AdcTemperatureConvertor::WriteADC2TemperatureSection(QXmlStreamWriter *writer)
{
	writer->writeStartElement(SettingsRootElement); // Settings root
		writer->writeAttribute(DeviceAttribute, DeviceName); // For what device
		writer->writeAttribute(VersionAttribute, QString(QObject::tr("%1")).arg(Version)); // Settings file version
		writer->writeTextElement(DescriptionElement, _description); // Settings description
		writer->writeTextElement(MultiplicativeElement, FormatDoubleForXML(_a)); // a
		writer->writeTextElement(AdditiveElement, FormatDoubleForXML(_b)); // b
	writer->writeEndElement(); // End of settings root
}

void AdcTemperatureConvertor::SetDescription(QString descr)
{
	_description = descr;
}

QString AdcTemperatureConvertor::GetDescription()
{
	return _description;
}
