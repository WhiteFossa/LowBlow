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

#ifndef SETTINGSSAVERLOADER_HPP
#define SETTINGSSAVERLOADER_HPP

#include <QException>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <Auxiliary.hpp>
#include <Interfaces/ISettingsSaverLoader.hpp>
#include <Implementations/AdcTemperatureConvertor.hpp>
#include <Implementations/SettingsGenerator.hpp>
#include <Implementations/EEPROMGenerator.hpp>
#include <XmlHelper.hpp>
#include <QMessageBox>

/**
 * @brief Class to save and load settings to XML.
 */
class SettingsSaverLoader : public Interfaces::ISettingsSaverLoader
{
	public:

		/**
		 * @brief SettingsSaverLoader Constructor.
		 */
		SettingsSaverLoader();

		/**
		* @brief Destructor.
		*/
		~SettingsSaverLoader();

		void Create(QString path, QString adc2TempPath);
		bool Load(QString path);
		void Save();
		void SaveAs(QString path);
		QString GetFilePath();
		void MarkAsModified();
		bool IsModified();
		Interfaces::IAdcTemperatureConvertor* GetADC2TempConvertorPtr();
		Interfaces::ISettingsGenerator* GetSettingsGeneratorPtr();
		void ExportToEEPROM(QString path);

	protected:

	private:

		/**
		 * @brief Name of XML root element with settings.
		 */
		const QString SettingsRootElement = "Settings";

		/**
		 * @brief This attribute of root element stores device name.
		 */
		const QString SettingsDeviceAttribute = "Device";

		/**
		 * @brief Settings are for this device. Actually it's a signature to check that it's our file.
		 */
		const QString SettingsDeviceName = "LowBlow";

		/**
		 * @brief This attribute of root elemen stores settings version.
		 */
		const QString SettingsVersionAttribute = "Version";

		/**
		 * @brief Base levels section name.
		 */
		const QString BaseLevelsElement = "BaseLevels";

		/**
		 * @brief Settings file version.
		 */
		const uint SettingsVersion = 1;

		/**
		 * @brief Base temperature ADC level element name.
		 */
		const QString BaseADCLevelElement = "TemperatureADC";

		/**
		 * @brief Base RPMs PWM level element name.
		 */
		const QString BaseRPMsLevelElement = "RPMsPWM";

		/**
		 * @brief Steps element name.
		 */
		const QString StepsElement = "Steps";

		/**
		 * @brief Single step element name. %1 - step number (starting from 0, additional steps not included).
		 */
		const QString StepElement = "Step%1";

		/**
		 * @brief ADC level increase for given step stored in element with this name.
		 */
		const QString ADCIncreaseElement = "ADCIncrease";

		/**
		 * @brief PWM level increase for given step stored in element with this name,
		 */
		const QString RPMsPWMIncreaseElement = "RPMsPWMIncrease";

		/**
		 * @brief Prefix for loading ADC to Temperature convertor settings from settings file.
		 */
		const QString Adc2TempSettingsPrefixFromSettingsFile = QString(QObject::tr("doc($settingsFile)/%1/")).arg(SettingsRootElement);

				/**
		 * @brief Prefix for loading ADC to Temperature convertor settings from sensor file.
		 */
		const QString Adc2TempSettingsPrefixFromSensorFile = QString(QObject::tr("doc($settingsFile)/"));


		/**
		 * @brief _path Current file path.
		 */
		QString _path;

		/**
		 * @brief _isModified True if file is modified and not saved.
		 */
		bool _isModified;

		/**
		 * @brief Saves current settings to file with given path without any questions.
		 * @param path Where to save current settings.
		 */
		void SaveAtGivenPath(QString path);

};

#endif // SETTINGSSAVERLOADER_HPP
