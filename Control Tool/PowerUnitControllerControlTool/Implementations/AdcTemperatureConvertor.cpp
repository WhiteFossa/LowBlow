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
	this->description = QObject::tr("Not initialized yet");
	this->a = 0;
	this->b = 0;
}

double AdcTemperatureConvertor::ADC2TEMP(uint adc)
{
	return this->a * adc + this->b;
}

uint AdcTemperatureConvertor::TEMP2ADC(double temp)
{
	int pre_result = (int)floor((temp - this->b) / this->a + 0.5);
	if(pre_result < 0)
	{
		return 0;
	}
	else if (pre_result > ADC_MAX_VALUE)
	{
		return ADC_MAX_VALUE;
	}

	return (uint)pre_result;
}

void AdcTemperatureConvertor::SetADC2TempConversionFactors(double a, double b)
{
	this->a = a;
	this->b = b;
}

void AdcTemperatureConvertor::GetADC2TempConversionFactors(double *a, double *b)
{
	*a = this->a;
	*b = this->b;
}


void AdcTemperatureConvertor::LoadSettings(QString filename)
{
	QFile *file = new QFile(filename);
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		SafeDelete(file);
		throw std::runtime_error(QString(QObject::tr("Cannot open file \"%1\" for reading")).arg(filename).toStdString());
	}

	QXmlStreamReader xsr(file);

	// Reading
	bool start_found = false; // Load progress flags
	bool description_loaded = false;
	bool mul_loaded = false;
	bool add_loaded = false;
	while (!xsr.atEnd()) // atEnd() true not only for end of file, but for error too
	{
		QXmlStreamReader::TokenType token = xsr.readNext();
		if (QXmlStreamReader::TokenType::StartElement == token)
		{
			if (A2T_SETTINGS_ROOT_ELEMENT == xsr.name())
			{
				// Root element
				QXmlStreamAttributes attrs = xsr.attributes();

				if (!attrs.hasAttribute(A2T_SETTINGS_DEVICE_ATTR) || !attrs.hasAttribute(A2T_SETTINGS_VERSION_ATTR))
				{
					// Missing required attributes
					file->close();
					SafeDelete(file);
					throw std::runtime_error(QString(QObject::tr("Either %1 or %2 attributes missing at %3 element"))
						.arg(A2T_SETTINGS_DEVICE_ATTR)
						.arg(A2T_SETTINGS_VERSION_ATTR)
						.arg(A2T_SETTINGS_ROOT_ELEMENT).toStdString());
				}

				if (attrs.value(A2T_SETTINGS_DEVICE_ATTR) != A2T_SETTINGS_DEVICE_NAME)
				{
					// Wrong device
					file->close();
					SafeDelete(file);
					throw std::runtime_error(QString(QObject::tr("%1 file is for another device")).arg(filename).toStdString());
				}

				if (attrs.value(A2T_SETTINGS_VERSION_ATTR) != A2T_SETTINGS_VERSION)
				{
					// Wrong version
					file->close();
					SafeDelete(file);
					throw std::runtime_error(QString(QObject::tr("%1 file have wrong version")).arg(filename).toStdString());
				}

				start_found = true;
			}
			else if (A2T_SETTINGS_DESCRIPTION_EL == xsr.name())
			{
				this->description = xsr.readElementText();
				description_loaded = true;
			}
			else if (A2T_SETTINGS_MULTIPLICATIVE_EL == xsr.name())
			{
				bool success = false;
				this->a = xsr.readElementText().toDouble(&success);

				if (!success)
				{
					file->close();
					SafeDelete(file);
					throw std::runtime_error(QString(QObject::tr("Multiplicative parameter in %1 file is not a floating point number")).arg(filename).toStdString());
				}

				mul_loaded = true;
			}

			else if (A2T_SETTINGS_ADDITIVE_EL == xsr.name())
			{
				bool success = false;
				this->b = xsr.readElementText().toDouble(&success);

				if (!success)
				{
					file->close();
					SafeDelete(file);
					throw std::runtime_error(QString(QObject::tr("Additive parameter in %1 file is not a floating point number")).arg(filename).toStdString());
				}

				add_loaded = true;
			}
		}
	}

	file->close();
	SafeDelete(file);

	// Is it error?
	if (xsr.hasError())
	{
		throw std::runtime_error(QString(QObject::tr("Error %1 while parsing file %2")).arg(xsr.errorString()).arg(filename).toStdString());
	}

	// Do we have all data?
	if (!(start_found && description_loaded && mul_loaded && add_loaded))
	{
		 throw std::runtime_error(QString(QObject::tr("File %1 have missing required records")).arg(filename).toStdString());
	}
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
		this->WriteADC2TemperatureSection(&xsw);
	xsw.writeEndDocument();

	file->close();
	SafeDelete(file);
}

void AdcTemperatureConvertor::WriteADC2TemperatureSection(QXmlStreamWriter *writer)
{
	writer->writeStartElement(A2T_SETTINGS_ROOT_ELEMENT); // Settings root
		writer->writeAttribute(A2T_SETTINGS_DEVICE_ATTR, A2T_SETTINGS_DEVICE_NAME); // For what device
		writer->writeAttribute(A2T_SETTINGS_VERSION_ATTR, A2T_SETTINGS_VERSION); // Settings file version
		writer->writeTextElement(A2T_SETTINGS_DESCRIPTION_EL, this->description); // Settings description
		writer->writeTextElement(A2T_SETTINGS_MULTIPLICATIVE_EL, FormatDoubleForXML(this->a)); // a
		writer->writeTextElement(A2T_SETTINGS_ADDITIVE_EL, FormatDoubleForXML(this->b)); // b
	writer->writeEndElement(); // End of settings root
}

void AdcTemperatureConvertor::SetDescription(QString descr)
{
	this->description = descr;
}

QString AdcTemperatureConvertor::GetDescription()
{
	return this->description;
}
