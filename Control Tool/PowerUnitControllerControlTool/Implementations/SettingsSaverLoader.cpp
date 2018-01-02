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

#include <Implementations/SettingsSaverLoader.hpp>

SettingsSaverLoader::SettingsSaverLoader()
{
	this->_adc2Temp = new AdcTemperatureConvertor();

	this->_path = "";
	this->_isModified = false;
}

void SettingsSaverLoader::Create(QString path, QString adc2TempPath)
{
	this->_path = path;
	this->_isModified = true;

	this->_adc2Temp->LoadSettings(adc2TempPath);

	// Regenerating settings generator
	SafeDelete(this->_setgen);
	this->_setgen = new SettingsGenerator(this->_adc2Temp);

	// Zero base levels
	this->_setgen->SetBaseRPM(0);
	this->_setgen->SetBaseTemperature(0);
}

void SettingsSaverLoader::Load(QString path)
{
	this->_path = path;
	this->_isModified = false;

	// Loading sensor settings
	this->_adc2Temp->LoadSettings(this->_path);

	SafeDelete(this->_setgen);
	this->_setgen = new SettingsGenerator(this->_adc2Temp);

	QFile *file = new QFile(_path);
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		SafeDelete(file);
		throw std::runtime_error(QString(QObject::tr("Cannot open file \"%1\" for reading.")).arg(_path).toStdString());
	}

	QXmlQuery query;

	// Binding settings file contents into $settingsFile variable
	query.bindVariable("settingsFile", file);

	// Base levels
	// ADC level
	query.setQuery("doc($settingsFile)/Settings/BaseLevels/TemperatureADC/text()");
	_setgen->SetBaseTemperatureADC(Fossa::Helpers::XmlHelper::GetIntegerValue(&query));

	// Base RPMs PWM level
	query.setQuery("doc($settingsFile)/Settings/BaseLevels/RPMsPWM/text()");
	_setgen->SetBaseRPM(Fossa::Helpers::XmlHelper::GetIntegerValue(&query));

//	QXmlStreamReader xsr(file);

//	bool rootFound = false;
//	bool baseLevelsElementStarted = false;
//	bool baseLevelsElementFound = false;
//	bool baseTemperatureFound = false;
//	bool baseRPMsFound = false;

//	while (!xsr.atEnd())
//	{
//		QXmlStreamReader::TokenType token = xsr.readNext();
//		if (QXmlStreamReader::TokenType::StartElement == token)
//		{
//			QString elementName = xsr.Name();

//			if (!baseLevelsElementStarted && (SettingsRootElement == elementName))
//			{
//				// Root
//				rootFound = true;

//				QXmlStreamAttributes attrs = xsr.attributes();

//				// We need device and version
//				if (!attrs.hasAttribute(SettingsDeviceAttribute) || !attrs.hasAttribute(SettingsVersionAttribute))
//				{
//					// Missing required attributes
//					file->close();
//					SafeDelete(file);
//					throw std::runtime_error(QString(QObject::tr("Either %1 or %2 attributes missing at %3 element"))
//						.arg(SettingsDeviceAttribute)
//						.arg(SettingsVersionAttribute)
//						.arg(SettingsRootElement));

//					// Is it our file?
//					if (attrs.value(SettingsDeviceAttribute) != SettingsDeviceName)
//					{
//						// Wrong device
//						file->close();
//						SafeDelete(file);
//						throw std::runtime_error(QString(QObject::tr("%1 file is for another device")).arg(_path).toStdString());
//					}

//					if (attrs.value(SettingsVersionAttribute) != SettingsVersion)
//					{
//						// Wrong version
//						file->close();
//						SafeDelete(file);
//						throw std::runtime_error(QString(QObject::tr("%1 file have wrong version")).arg(filename).toStdString());
//					}
//				}
//				else if (!baseLevelsElementStarted && !baseLevelsElementFound && (BaseLevelsElement == elementName))
//				{
//					baseLevelsElementStarted = true;
//				}
//				else if (baseLevelsElementStarted && !baseLevelsElementFound && (BaseADCLevelElement == elementName))
//				{
//					// Base ADC level

//					if (baseRPMsFound && baseTemperatureFound)
//					{
//						baseLevelsElementStarted = false;
//						baseLevelsElementFound = true;
//					}
//				}
//				else if (baseLevelsElementStarted && !baseLevelsElementFound && (BaseRPMsLevelElement == elementName))
//				{
//					// Base RPMs level

//					if (baseRPMsFound && baseTemperatureFound)
//					{
//						baseLevelsElementStarted = false;
//						baseLevelsElementFound = true;
//					}
//				}
//			}

//		}
//	}

	file->close();
	SafeDelete(file);

//	// Is it error?
//	if (xsr.hasError())
//	{
//		throw std::runtime_error(QString(QObject::tr("Error %1 while parsing file %2")).arg(xsr.errorString()).arg(_path).toStdString());
//	}

//	// Do we have everything?
//	if (!(rootFound))
//	{
//		 throw std::runtime_error(QString(QObject::tr("File %1 have missing required records")).arg(_path).toStdString());
//	}
}

void SettingsSaverLoader::SaveAtGivenPath(QString path)
{
	if (path.isEmpty())
	{
		throw std::runtime_error(QObject::tr("Attempt to save file while path is not specified.").toStdString());
	}

	// Saving
	QFile *file = new QFile(path);
	if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
	{
		SafeDelete(file);
		throw std::runtime_error(QString(QObject::tr("Cannot open file \"%1\" for writing.")).arg(path).toStdString());
	}

	QXmlStreamWriter xsw(file); // Stream writer for XML data
	xsw.setAutoFormatting(true);
	xsw.writeStartDocument();
		// Writing version and device
		xsw.writeStartElement(this->SettingsRootElement); // Settings root

		xsw.writeAttribute(this->SettingsDeviceAttribute, this->SettingsDeviceName); // Device
		xsw.writeAttribute(this->SettingsVersionAttribute, QString::number(this->SettingsVersion)); // Settings version

		// Writing ADC->Temperature settings
		this->_adc2Temp->WriteADC2TemperatureSection(&xsw);

		// Base levels
		xsw.writeStartElement(this->BaseLevelsElement);
			xsw.writeTextElement(this->BaseADCLevelElement, QString::number(this->_setgen->GetBaseTemperatureADC())); // Temperature
			xsw.writeTextElement(this->BaseRPMsLevelElement, QString::number(this->_setgen->GetBaseRPM())); // RPMs
		xsw.writeEndElement();

		// Steps
		xsw.writeStartElement(this->StepsElement);
		for (uint i = ADDITIONAL_STEPS; i < STEPS_NUMBER; i ++)
		{
			xsw.writeStartElement(this->StepElement.arg(i - ADDITIONAL_STEPS));
				Interfaces::ISettingsStep* step = this->_setgen->GetStepPtr(i);
				xsw.writeTextElement(this->ADCIncreaseElement, QString::number(step->GetADCDelta()));
				xsw.writeTextElement(this->RPMsPWMIncreaseElement, QString::number(step->GetRPMDelta()));
			xsw.writeEndElement();
		}

		xsw.writeEndElement();

		xsw.writeEndElement();

	xsw.writeEndDocument();

	file->close();
	SafeDelete(file);
}

void SettingsSaverLoader::Save()
{
	// Do we need to save?
	if (!this->_isModified)
	{
		return;
	}

	this->SaveAtGivenPath(this->_path);


	this->_isModified = false;
}

void SettingsSaverLoader::SaveAs(QString path)
{
	this->SaveAtGivenPath(path);
}

QString SettingsSaverLoader::GetFilePath()
{
	return this->_path;
}

void SettingsSaverLoader::MarkAsModified()
{
	if (!this->_path.isEmpty())
	{
		this->_isModified = true;
	}
}

bool SettingsSaverLoader::IsModified()
{
	return this->_isModified;
}

Interfaces::IAdcTemperatureConvertor* SettingsSaverLoader::GetADC2TempConvertorPtr()
{
	return this->_adc2Temp;
}

Interfaces::ISettingsGenerator* SettingsSaverLoader::GetSettingsGeneratorPtr()
{
	return this->_setgen;
}

SettingsSaverLoader::~SettingsSaverLoader()
{
	SafeDelete(this->_setgen);
	SafeDelete(this->_adc2Temp);
}
