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
	this->_path = QObject::trUtf8("");
	this->_isModified = false;
}

void SettingsSaverLoader::Create(QString path, QString adc2TempPath)
{
	this->_path = path;
	this->_isModified = true;

	this->_adc2Temp->LoadSettings(adc2TempPath);
}

void SettingsSaverLoader::Load(QString path)
{
	this->_path = path;
}

void SettingsSaverLoader::Save()
{
	this->_isModified = false;
}

void SettingsSaverLoader::SaveAs(QString path)
{
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

const Interfaces::IAdcTemperatureConvertor* SettingsSaverLoader::GetADC2TempConvertorPtr()
{
	return this->_adc2Temp;
}

SettingsSaverLoader::~SettingsSaverLoader()
{
	SafeDelete(this->_adc2Temp);
}
