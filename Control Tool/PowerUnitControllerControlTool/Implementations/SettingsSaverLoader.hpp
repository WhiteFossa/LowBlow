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

#include <Interfaces/ISettingsSaverLoader.hpp>

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

		void Create(QString path, QString adc2TempPath);
		void Load(QString path);
		void Save();
		void SaveAs(QString path);
		QString GetFilePath();
		bool IsModified();


	protected:

	private:

		/**
		 * @brief _path Current file path.
		 */
		QString _path;

};

#endif // SETTINGSSAVERLOADER_HPP
