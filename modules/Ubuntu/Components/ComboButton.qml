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

    property real collapsedHeight: implicitHeight
    property real expandedHeight: collapsedHeight + units.gu(15)

    property alias comboList: comboHolder.data

    style: Theme.createStyleComponent("ComboButtonStyle.qml", combo)

    height: expanded ? expandedHeight : collapsedHeight

    // dropdown button
    AbstractButton {
        id: dropDown
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        width: combo.__styleInstance ? combo.__styleInstance.dropDownWidth: 0
        onClicked: {
            // toggle expanded
            combo.expanded = !combo.expanded;
        }
    }

    // expansion list
    Item {
        id: comboHolder
        parent: combo.__styleInstance ? combo.__styleInstance.comboList : combo
        anchors.fill: parent
    }
}
