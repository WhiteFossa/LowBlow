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
		throw std::runtime_error(QString(QObject::tr("Cannot open file \"%1\" for writing")).arg(path).toStdString());
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
