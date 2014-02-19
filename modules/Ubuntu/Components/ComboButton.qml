/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.0
import Ubuntu.Components.Popups 0.1

/*!
    \qmltype ComboButton
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Ubuntu button with expanding list of components.
  */
Button {
    id: combo

    property bool expanded: false

    property int selectedIndex: -1

    property real expandedHeight

    property Item comboList

    style: Theme.createStyleComponent("ComboButtonStyle.qml", combo)

    // dropdown button
    AbstractButton {
        id: dropDown
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: __styleInstance ? __styleInstance.dropDownMargins : 0
        }
        width: __styleInstance ? __styleInstance.dropDownWidth: 0
    }

    // expansion list
    Popover {
        id: comboHolder
        anchors {
            left: parent.left
            right: parent.right
        }
    }
}
