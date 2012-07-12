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
 */

import QtQuick 1.1
import ".."

/*!
    \qmlclass SingleValue
    \inqmlmodule UbuntuUIToolkit
    \brief The SingleValue class is DOCME

    \b{This component is under heavy development.}

*/
Default {
    id: listItem

    property alias value: valueText.text

    property color __valueColor: Qt.rgba(0.5, 0.5, 0.5, 1.0)

    control: TextCustom {
        id: valueText
        elide: Text.ElideRight
        opacity: listItem.enabled ? 1.0 : 0.5
        fontSize: "medium"
        style: Text.Raised
        color: listItem.__valueColor
        styleColor: "white"
    }
}
