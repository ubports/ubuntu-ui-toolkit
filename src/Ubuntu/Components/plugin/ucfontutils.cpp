/*
 * Copyright 2015 Canonical Ltd.
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

/*!
 * \qmltype FontUtils
 * \instantiates UCFontUtils
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief FontUtils is a \l {http://qt-project.org/doc/qt-5.0/qtqml/qqmlcontext.html#details} {context property},
 * which provides utility functions for font manipulations.
 *
 * FontUtils cannot be instantiated and it is already available as a context
 * property, i.e. 'FontUtils'  is an instance  that can be used from anywhere in
 * the code.
 *
 * Example of a Label implementation which uses the default base scale for the font
 * size:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * Text {
 *     property string fontSize: "medium"
 *     font.pixelSize: FontUtils.sizeToPixels(fontSize)
 * }
 * \endqml
 *
 * Another example of a custom text input component exposing a fontSize property and a base
 * font unit size to scale its font:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * TextInput {
 *     property string fontSize: "small"
 *     property int baseFontUnits: units.dp(20)
 *     font.pixelSize: FontUtils.modularScale(fontSize) * units.dp(baseFontUnits)
 * }
 * \endqml
 *
 */

/*!
 * \qmlmethod real FontUtils::sizeToPixels(string size)
 * The function calculates the pixel size of a given scale. The size scale can be
 * one of the strings specified at modularScale function. On failure returns 0.
 */
qreal UCFontUtils::sizeToPixels(const QString &size)
{
    return modularScale(size) * UCUnits::instance().dp(fontUnits);
}

/*!
 * \qmlmethod real FontUtils::modularScale(string size)
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
    if (size.size() < 4) {
        return 0.0;
    }

    switch (SCALE_CODE(size)) {
        case SCALE_MEDIUM: { return mediumScale; }
        case SCALE_LARGE: { return largeScale; }
        case SCALE_SMALL: { return smallScale; }
        case SCALE_XLARGE: { return xLargeScale; }
        case SCALE_XSMALL: { return xSmallScale; }
        case SCALE_XXSMALL: { return xxSmallScale; }
        default: { return 0.0; }
    }
}
