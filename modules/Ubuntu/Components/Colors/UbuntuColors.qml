/*
    Copyright 2013 Canonical Ltd.
 *
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; version 3.
 *
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
 *
    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0

/*!
    \qmltype UbuntuColors
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup theming
    \brief Singleton defining the Ubuntu color palette.

    Example of use:

    \qml
    Rectangle {
       color: UbuntuColors.orange
    }
    \endqml
*/
QtObject {
    readonly property color orange: "#DD4814"
    readonly property color warmGrey: "#AEA79F"
    readonly property color coolGrey: "#333333"
    readonly property color lightAubergine: "#77216F"
    readonly property color midAubergine: "#5E2750"
    readonly property color darkAubergine: "#2C001E"
    property Gradient orangeGradient: Gradient {
        GradientStop { position: 0.0; color: "#DD4814" }
        GradientStop { position: 1.0; color: "#D9722D" }
    }
    property Gradient greyGradient: Gradient {
        GradientStop { position: 0.0; color: "#888888" }
        GradientStop { position: 1.0; color: "#BBBBBB" }
    }
}
