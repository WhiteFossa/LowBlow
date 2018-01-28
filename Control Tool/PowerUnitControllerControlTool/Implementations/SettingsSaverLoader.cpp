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

	this->_adc2Temp->LoadSettings(adc2TempPath, "");

	// Regenerating settings generator
	SafeDelete(this->_setgen);
	this->_setgen = new SettingsGenerator(this->_adc2Temp);

	// Zero base levels
	this->_setgen->SetBaseRPM(0);
	this->_setgen->SetBaseTemperature(0);
}

bool SettingsSaverLoader::Load(QString path)
{
	this->_path = path;
	this->_isModified = false;

	// Loading sensor settings
	this->_adc2Temp->LoadSettings(this->_path, "");

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

	// Is it our XML at all?
	query.setQuery(QString(QObject::tr("doc($settingsFile)/%1/@%2/string()")).arg(SettingsRootElement).arg(SettingsDeviceAttribute));
	QString deviceName = Fossa::Helpers::XmlHelper::GetTextValue(&query);

	// Version
	query.setQuery(QString(QObject::tr("doc($settingsFile)/%1/@%2/string()")).arg(SettingsRootElement).arg(SettingsVersionAttribute));
	uint version = Fossa::Helpers::XmlHelper::GetIntegerValue(&query);

	if ((deviceName != SettingsDeviceName) ||(version != SettingsVersion))
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

	for (uint step = 0; step < SettingsGenerator::StepsNumber; step ++)
	{
		Interfaces::ISettingsStep* stepData = this->_setgen->GetStepPtrRelative(step);

		QString xmlStepName = QString(QObject::tr("%1Step%2/")).arg(stepsXPath).arg(step);

		// ADC increase for step
		query.setQuery(QString(QObject::tr("%1%2/text()")).arg(xmlStepName).arg(this->ADCIncreaseElement));
		stepData->SetADCDelta(Fossa::Helpers::XmlHelper::GetIntegerValue(&query));

		// RPM increase for step
		query.setQuery(QString(QObject::tr("%1%2/text()")).arg(xmlStepName).arg(this->RPMsPWMIncreaseElement));
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
		for (uint step = 0; step < SettingsGenerator::StepsNumber; step ++)
		{
			xsw.writeStartElement(this->StepElement.arg(step));
				Interfaces::ISettingsStep* stepPtr = this->_setgen->GetStepPtrRelative(step);
				xsw.writeTextElement(this->ADCIncreaseElement, QString::number(stepPtr->GetADCDelta()));
				xsw.writeTextElement(this->RPMsPWMIncreaseElement, QString::number(stepPtr->GetRPMDelta()));
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
