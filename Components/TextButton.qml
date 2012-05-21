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

/*!
    \qmlclass TextButton
    \inqmlmodule UbuntuUIToolkit
    \brief The TextButton class is DOCME

    \b{This component is under heavy development.}

    The TextButton class is part of the \l{UbuntuUIToolkit} module.
*/
ColoredButton {
    id: textButton

    /*!
       \preliminary
       DOCME
    */
    property alias text: label.text

    /*!
       \preliminary
       DOCME
    */
    property alias textColor: label.color

    /*!
       \preliminary
       DOCME
    */
    property alias textSize: label.fontSize

    TextCustom {
        id: label
        anchors.centerIn: parent
        fontSize: "medium"
    } // label
} // button
