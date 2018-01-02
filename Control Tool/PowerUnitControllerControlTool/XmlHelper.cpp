﻿/*
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

#include <XmlHelper.hpp>

QString Fossa::Helpers::XmlHelper::GetTextValue(const QXmlQuery *query)
{
	QString parsedItem;
	query->evaluateTo(&parsedItem);

	return parsedItem.simplified();
}

int Fossa::Helpers::XmlHelper::GetIntegerValue(const QXmlQuery *query)
{
	QString parsedValue = Fossa::Helpers::XmlHelper::GetTextValue(query);

	bool isSuccess = false;

	int result = parsedValue.toInt(&isSuccess);

	if (!isSuccess)
	{
		throw std::runtime_error(QString(QObject::tr("%1 can't be converted to integer.")).arg(parsedValue).toStdString());
	}

	return result;
}

