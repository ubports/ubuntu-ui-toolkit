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

    Do not set the component's height directly, as expansion controls its value and
    overriding it may destroy component's proper functioning.
  */
Button {
    id: combo

    property bool expanded: false

    property int selectedIndex: -1

    property real collapsedHeight: implicitHeight
    property real expandedHeight: collapsedHeight + units.gu(15)

    property alias comboList: comboHolder.data
    property alias comboListItem: comboHolder

    style: Theme.createStyleComponent("ComboButtonStyle.qml", combo)

//    height: expanded ? expandedHeight : collapsedHeight

    Component.onCompleted: {
        // update mouse area to report clicks only on the main button area
        // area excluding dropDown button and combo list
        // we must do separate bindings as __mouseArea is a read-only property
        __mouseArea.anchors.fill = undefined;
        __mouseArea.anchors.left = Qt.binding(function() {return combo.left;});
        __mouseArea.anchors.top = Qt.binding(function() {return combo.top;});
        __mouseArea.anchors.right = Qt.binding(function() {return combo.right;});
        __mouseArea.anchors.rightMargin = Qt.binding(function() {return combo.__styleInstance.dropDownWidth;});
        __mouseArea.height = Qt.binding(function() {return collapsedHeight;});
    }

    // dropdown button
    AbstractButton {
        id: dropDown
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        width: combo.__styleInstance ? combo.__styleInstance.dropDownWidth : 0
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
