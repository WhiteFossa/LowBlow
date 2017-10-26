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

#ifndef ISETTINGSSAVERLOADER_HPP
#define ISETTINGSSAVERLOADER_HPP

#include <QObject>
#include <QString>
#include <Interfaces/IAdcTemperatureConvertor.hpp>
#include <Interfaces/ISettingsGenerator.hpp>

namespace Interfaces
{
	/**
	 * @brief Class to save and load controller settings into XML.
	 */
	class ISettingsSaverLoader : public QObject
	{
		Q_OBJECT

		// Public methods.
		public:

			/**
			 * @brief Creates settings file at path and embeds ADC->Temperature settings
			 * from adc2TempPath into it.
			 * @param path Path where created file will be stored (actual write occurs on "Save" call).
			 * @param adc2TempPath path to file with ADC->Temperature settings.
			 */
			virtual void Create(QString path, QString adc2TempPath) = 0;

			/**
			 * @brief Loads settings from given file.
			 * @param path File to load from.
			 */
			virtual void Load(QString path) = 0;

			/**
			 * @brief Saves settings to current file or throws exception if current file is not set.
			 */
			virtual void Save() = 0;

			/**
			 * @brief Saves settings to given file, then sets this file as current.
			 * @param path Path to save.
			 */
			virtual void SaveAs(QString path) = 0;

			/**
			 * @brief GetFilePath Returns current file full path. If null then file wasn't loaded/created at all.
			 * @return Current file path.
			 */
			virtual QString GetFilePath() = 0;

			/**
			 * @brief Makrs current file is modified. If no file loaded does nothing.
			 */
			virtual void MarkAsModified() = 0;

			/**
			 * @brief IsModified True if file was modified and haven't saved till now.
			 * @return True if file modified.
			 */
			virtual bool IsModified() = 0;

			/**
			 * @brief Returns pointer to ADC->Temperature convertor instance.
			 * @return Pointer to ADC->Temperature convertor.
			 */
			virtual Interfaces::IAdcTemperatureConvertor* GetADC2TempConvertorPtr() = 0;

			/**
			 * @brief Returns pointer to settings generator.
			 * @return Pointer to settings generator.
			 */
			virtual Interfaces::ISettingsGenerator* GetSettingsGeneratorPtr() = 0;

		protected:
			/**
			 * @brief ADC to temperature convertor.
			 */
			Interfaces::IAdcTemperatureConvertor* _adc2Temp = nullptr;

			/**
			* @brief _setgen Settings generator
			*/
			Interfaces::ISettingsGenerator *_setgen = nullptr;
	};
}

#endif // ISETTINGSSAVERLOADER_HPP
