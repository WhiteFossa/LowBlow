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
			 * @brief Creates new settings file (in memory) and setting ADC->Temperature settings for it by calling
			 * LoadADC2Temp().
			 */
			virtual void Create() = 0;

			/**
			 * @brief Loads settings from given file.
			 * @param path File to load from.
			 */
			virtual void Load(QString path) = 0;

			/**
			 * @brief Saves settings to given file, then sets this file as current.
			 * @param path Path to save.
			 */
			virtual void SaveAs(QString path) = 0;

			/**
			 * @brief Saves settings to current file or throws exception if current file is not set.
			 */
			virtual void Save() = 0;

	};
}

#endif // ISETTINGSSAVERLOADER_HPP
