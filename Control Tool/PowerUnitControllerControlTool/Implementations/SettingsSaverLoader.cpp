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

QByteArray SettingsSaverLoader::EEPROMBuffer;

SettingsSaverLoader::SettingsSaverLoader()
{
	_adc2Temp = new AdcTemperatureConvertor();

	// EEPROM generator
	_eepromGen = new EEPROMGenerator();

	_path = "";
	_isModified = false;
}

void SettingsSaverLoader::Create(QString path, QString adc2TempPath)
{
	_path = path;
	_isModified = true;

	_adc2Temp->LoadSettings(adc2TempPath, Adc2TempSettingsPrefixFromSensorFile);

	// Regenerating settings generator
	SafeDelete(_setgen);
	_setgen = new SettingsGenerator(_adc2Temp);

	// Zero base levels
	_setgen->SetBaseRPM(0);
	_setgen->SetBaseTemperature(0);
}

bool SettingsSaverLoader::Load(QString path)
{
	_path = path;
	_isModified = false;

	// Loading sensor settings
	_adc2Temp->LoadSettings(_path, Adc2TempSettingsPrefixFromSettingsFile);

	SafeDelete(_setgen);
	_setgen = new SettingsGenerator(_adc2Temp);

	QFile *file = new QFile(_path);
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		SafeDelete(file);
		throw std::runtime_error(QString(QObject::tr("Cannot open file \"%1\" for reading.")).arg(_path).toStdString());
	}

	QXmlQuery query;

	// Binding settings file contents into $settingsFile variable
	query.bindVariable("settingsFile", file);

	// Is it our XML at all?
	query.setQuery(QString(QObject::tr("doc($settingsFile)/%1/@%2/string()")).arg(SettingsRootElement).arg(SettingsDeviceAttribute));
	QString deviceName = Fossa::Helpers::XmlHelper::GetTextValue(&query);

	// Version
	query.setQuery(QString(QObject::tr("doc($settingsFile)/%1/@%2/string()")).arg(SettingsRootElement).arg(SettingsVersionAttribute));
	uint version = Fossa::Helpers::XmlHelper::GetIntegerValue(&query);

	if ((deviceName != SettingsDeviceName) || (version != SettingsVersion))
	{
		// Wrong file
		file->close();
		SafeDelete(file);

		return false;
	}

	// Base levels
	QString baseLevelsXPath = QString(QObject::tr("doc($settingsFile)/%1/%2/")).arg(SettingsRootElement).arg(BaseLevelsElement);

	// ADC level
	query.setQuery(QString(QObject::tr("%1TemperatureADC/text()")).arg(baseLevelsXPath));
	_setgen->SetBaseTemperatureADC(Fossa::Helpers::XmlHelper::GetIntegerValue(&query));

	// Base RPMs PWM level
	query.setQuery(QString(QObject::tr("%1RPMsPWM/text()")).arg(baseLevelsXPath));
	_setgen->SetBaseRPM(Fossa::Helpers::XmlHelper::GetIntegerValue(&query));

	// Steps
	QString stepsXPath = QString(QObject::tr("doc($settingsFile)/%1/%2/")).arg(SettingsRootElement).arg(StepsElement);

	for (uint step = 0; step < Interfaces::ISettingsGenerator::StepsNumber; step ++)
	{
		Interfaces::ISettingsStep* stepData = _setgen->GetStepPtrRelative(step);

		QString xmlStepName = QString(QObject::tr("%1Step%2/")).arg(stepsXPath).arg(step);

		// ADC increase for step
		query.setQuery(QString(QObject::tr("%1%2/text()")).arg(xmlStepName).arg(ADCIncreaseElement));
		stepData->SetADCDelta(Fossa::Helpers::XmlHelper::GetIntegerValue(&query));

		// RPM increase for step
		query.setQuery(QString(QObject::tr("%1%2/text()")).arg(xmlStepName).arg(RPMsPWMIncreaseElement));
		stepData->SetRPMDelta(Fossa::Helpers::XmlHelper::GetIntegerValue(&query));
	}

	file->close();
	SafeDelete(file);

	return true;
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
		xsw.writeStartElement(SettingsRootElement); // Settings root

		xsw.writeAttribute(SettingsDeviceAttribute, SettingsDeviceName); // Device
		xsw.writeAttribute(SettingsVersionAttribute, QString::number(SettingsVersion)); // Settings version

		// Writing ADC->Temperature settings
		_adc2Temp->WriteADC2TemperatureSection(&xsw);

		// Base levels
		xsw.writeStartElement(BaseLevelsElement);
			xsw.writeTextElement(BaseADCLevelElement, QString::number(_setgen->GetBaseTemperatureADC())); // Temperature
			xsw.writeTextElement(BaseRPMsLevelElement, QString::number(_setgen->GetBaseRPM())); // RPMs
		xsw.writeEndElement();

		// Steps
		xsw.writeStartElement(StepsElement);
		for (uint step = 0; step < Interfaces::ISettingsGenerator::StepsNumber; step ++)
		{
			xsw.writeStartElement(StepElement.arg(step));
				Interfaces::ISettingsStep* stepPtr = _setgen->GetStepPtrRelative(step);
				xsw.writeTextElement(ADCIncreaseElement, QString::number(stepPtr->GetADCDelta()));
				xsw.writeTextElement(RPMsPWMIncreaseElement, QString::number(stepPtr->GetRPMDelta()));
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
	if (!_isModified)
	{
		return;
	}

	SaveAtGivenPath(_path);


	_isModified = false;
}

void SettingsSaverLoader::SaveAs(QString path)
{
	SaveAtGivenPath(path);
}

QString SettingsSaverLoader::GetFilePath()
{
	return _path;
}

void SettingsSaverLoader::MarkAsModified()
{
	if (!_path.isEmpty())
	{
		_isModified = true;
	}
}

bool SettingsSaverLoader::IsModified()
{
	return _isModified;
}

Interfaces::IAdcTemperatureConvertor* SettingsSaverLoader::GetADC2TempConvertorPtr()
{
	return _adc2Temp;
}

Interfaces::ISettingsGenerator* SettingsSaverLoader::GetSettingsGeneratorPtr()
{
	return _setgen;
}

void SettingsSaverLoader::ExportToEEPROM(QString path)
{
	auto eepromContents = _eepromGen->GetEEPROMContents(_setgen);

	EEPROMBuffer.clear();

	// Generating iHEX
	struct ihex_state ihex;
	ihex_init(&ihex);
	ihex_write_at_address(&ihex, 0); // Start address
	ihex_write_bytes(&ihex, eepromContents.constData(), eepromContents.count());
	ihex_end_write(&ihex);

	// Writing to file
	QFile *file = new QFile(path);
	if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
	{
		SafeDelete(file);
		throw std::runtime_error(QString(QObject::tr("Cannot open file \"%1\" for writing.")).arg(path).toStdString());
	}

	if (file->write(EEPROMBuffer) != EEPROMBuffer.count())
	{
		file->close();
		SafeDelete(file);
		throw std::runtime_error(QString(QObject::tr("Error during write to \"%1\".")).arg(path).toStdString());
	}
	file->close();
	SafeDelete(file);
}

SettingsSaverLoader::~SettingsSaverLoader()
{
	SafeDelete(_eepromGen);
	SafeDelete(_setgen);
	SafeDelete(_adc2Temp);
}

/**
 * @brief ihex_flush_buffer Callback function to store iHEX file data.
 */
void ihex_flush_buffer(struct ihex_state *ihex, char *buffer, char *eptr)
{
	Q_UNUSED(ihex);

	auto size = eptr - buffer;
	SettingsSaverLoader::EEPROMBuffer.append(buffer, size);
}

