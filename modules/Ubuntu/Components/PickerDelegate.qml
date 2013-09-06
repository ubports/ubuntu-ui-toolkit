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

import QtQuick 2.0
import Ubuntu.Components 0.1 as Ubuntu

/*!
    \qmltype PickerDelegate
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief PickerDelegate component serves as base for Picker delegates.

    PickerDelegate is a holder component for delegates used in a Picker element.
    Each picker delegate must be derived from this type.
  */

Ubuntu.AbstractButton {
    id: pickerDelegate

    implicitHeight: units.gu(4)
    implicitWidth: parent.width

    /*! \internal */
    onClicked: {
        if (internal.tumblerItem.currentIndex === index) return;
        internal.tumbler.__clickedIndex = index;
        internal.tumblerItem.currentIndex = index;
    }

    style: Theme.createStyleComponent("PickerDelegateStyle.qml", pickerDelegate)

    QtObject {
        id: internal
        property bool inListView: QuickUtils.className(pickerDelegate.parent) !== "QQuickPathView"
        property Item tumblerItem: !inListView ? pickerDelegate.parent : pickerDelegate.parent.parent
        property Item tumbler: tumblerItem ? tumblerItem.tumbler : null
    }
}
