/*
 * Copyright 2013 Canonical Ltd.
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
import "../" 0.1

/*!
    \qmltype PickerDelegate
    \inqmlmodule Ubuntu.Components.Pickers 0.1
    \ingroup ubuntu-pickers
    \brief PickerDelegate component serves as base for Picker delegates.

    PickerDelegate is a holder component for delegates used in a Picker element.
    Each picker delegate must be derived from this type.
  */

AbstractButton {
    id: pickerDelegate

    /*!
      \qmlproperty bool highlighted
      \readonly
      The property notifies whether the current item is the highlighted one or not.
      */
    readonly property alias highlighted: internal.highlighted

    implicitHeight: units.gu(5)
    implicitWidth: parent.width

    /*! \internal */
    onClicked: {
        if (internal.tumblerItem.currentIndex === index) return;
        internal.tumbler.__clickedIndex = index;
        internal.tumblerItem.currentIndex = index;
    }

    style: Theme.createStyleComponent("PickerDelegateStyle.qml", pickerDelegate)

    // get the content into this component
    /*! \internal */
    default property alias content: body.data
    Item {
        id: body
        anchors {
            fill: parent
            margins: units.gu(0.2)
        }
    }

    QtObject {
        id: internal
        property bool inListView: QuickUtils.className(pickerDelegate.parent) !== "QQuickPathView"
        property Item tumblerItem: !inListView ? pickerDelegate.parent : pickerDelegate.parent.parent
        property Item tumbler: tumblerItem ? tumblerItem.tumbler : null

        property bool highlighted: tumblerItem.currentIndex === index;
    }
}
