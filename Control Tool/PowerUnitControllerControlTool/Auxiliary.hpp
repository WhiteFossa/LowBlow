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

---

Part of "LowBlow" project.

Feel free to contact: whitefossa@gmail.com

Repository: https://github.com/WhiteFossa/LowBlow

If repository above is not available, try my LiveJournal: fossa-white.livejournal.com
or as last measure try to search for #WhiteFossa hashtag.
*/

#ifndef AUXILIARY_HPP
#define AUXILIARY_HPP

#include <QString>

/* Auxiliary functions */

/**
 * @brief Use it to safely remove anything. It calls delete obj, and then sets obj to 0.
 * @param obj What we need to delete
 */
template <typename T>
void SafeDelete(T obj)
{
	delete obj;
	obj = 0;
}

/**
 * @brief Formats double for saving in XML as string.
 * @param d Double to format.
 * @return String with formatted value.
 */
QString FormatDoubleForXML(double d);

#endif // AUXILIARY_HPP
