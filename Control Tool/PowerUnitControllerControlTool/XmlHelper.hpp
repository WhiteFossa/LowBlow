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

#ifndef XMLHELPER_HPP
#define XMLHELPER_HPP

#include <QXmlQuery>
#include <QString>

namespace	Fossa
{
	namespace	Helpers
	{
		/**
		 * @brief Helper class to work with XML (parse it, to be precise).
		 */
		class XmlHelper
		{
			public:
				/**
				 * @brief Returns XML node value as integer. Throws exception if node value is not integer.
				 * @param query Query to select node value, for example "doc($settingsFile)/Settings/BaseLevels/TemperatureADC/text()".
				 * @return Integer node value.
				 */
				static int GetIntegerValue(const QXmlQuery *query);

				/**
				 * @brief As GetIntegerValue, but for double.
				 * @param query Query to select node value, for example "doc($settingsFile)/Settings/BaseLevels/TemperatureADC/text()".
				 * @return Double node value.
				 */
				static double GetDoubleValue(const QXmlQuery *query);

				/**
				 * @brief GetTextValue As GetIntegerValue(), but returns node text value, trimmed by QString::simplified()
				 * @param query Query to select node value, for example "doc($settingsFile)/Settings/BaseLevels/TemperatureADC/text()".
				 * @return Text node value.
				 */
				static QString GetTextValue(const QXmlQuery *query);
		};
	}
}


#endif // XMLHELPER_HPP
