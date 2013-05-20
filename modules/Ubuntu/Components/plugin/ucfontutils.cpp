/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ucfontutils.h"
#include "ucunits.h"
#include <QtCore/QMetaEnum>
#include <QtQml/QQmlInfo>

/*!
 * \qmltype FontUtils
 * \instantiates UCFontUtils
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief The FontUtils component provides utility functions for font manipulations.
 */

/*!
 * The function calculates the pixel size of a given scale. The size scale can be
 * one of the strings specified at modularScale function. On failure returns 0.
 */
qreal UCFontUtils::sizeToPixels(const QString &size)
{
    return modularScale(size) * UCUnits::instance().dp(14);
}

/*!
 * The function returns the number interpretation of a given font scale. The scale
 * can have one of the following values:
 * \list
 * \li \b{xx-small}
 * \li \b{x-small}
 * \li \b{small}
 * \li \b{medium}
 * \li \b{large}
 * \li \b{x-large}
 * \endlist
 */
qreal UCFontUtils::modularScale(const QString &size)
{
    if (size == "xx-small") {
        return 0.606;
    } else if (size == "x-small") {
        return 0.707;
    } else if (size == "small") {
        return 0.857;
    } else if (size == "medium") {
        return 1.0;
    } else if (size == "large") {
        return 1.414;
    } else if (size == "x-large") {
        return 2.423;
    }
    return 0.0;
}
