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

#ifndef IEEPROMGENERATOR_HPP
#define IEEPROMGENERATOR_HPP

#include <QObject>
#include <QVector>
#include <Interfaces/ISettingsGenerator.hpp>

namespace Interfaces
{
	/**
	 * @brief Interface to convert settings into EEPROM content.
	 */
	class IEEPROMGenerator : public QObject
	{
		Q_OBJECT

		public:
			/**
			 * @brief Generates EEPROM content using data from ISettingsGenerator.
			 * @param setGenPtr Pointer to ISettingsGenerator instance.
			 * @return EEPROM contents as QVector, starting from 0x00 address.
			 */
			virtual QByteArray GetEEPROMContents(ISettingsGenerator* setGenPtr) = 0;
	};
}

#endif // IEEPROMGENERATOR_HPP
