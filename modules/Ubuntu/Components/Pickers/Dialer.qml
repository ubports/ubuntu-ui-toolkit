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
    \qmltype Dialer
    \inqmlmodule Ubuntu.Components.Pickers 0.1
    \ingroup ubuntu-pickers
    \brief Dialer is a phone dialer style picker component.

    The Dialer component is dedicated for value selection where the value is
    compound of several sections, i.e. hour, minute and second, or integral and
    decimal values. Each section is defined by a DialerHand, which shares the
    same range as the dialer is having. Dialer hand visuals are placed on the
    same dialer disk.

    \qml
    import QtQuick 2.0
    import Ubuntu.Components.Pickers 0.1

    Dialer {
        size: units.gu(20)

        DialerHand {
            id: mainHand
        }
    }
    \endqml
  */

StyledItem {

    property real minimumValue: 0.0

    property real maximumValue: 100.0

    property real step: 1.0

    /*!
      The property holds the size of the dialer. The component should be sized
      using this property instead of using width and/or height properties. Sizing
      with this property it is made sure that the component will scale evenly.
      */
    property real size: units.gu(32)

    /*!
      The property holds the size reserved for the dialer hands. This value cannot
      be higher than the \l size of the dialer.
      */
    property real handSpace: size * 41 / 100

    /*!
      The property holds the component from the center of the Dialer. Items wanted
      to be placed into the center of the Dialer must be reparented to this component.
      */
    readonly property alias centerDisk: center

    id: dialer
    implicitWidth: size
    implicitHeight: size

    style: Theme.createStyleComponent("DialerStyle.qml", dialer)

    Item {
        id: center
        height: size - handSpace
        width: size - handSpace
        anchors.centerIn: parent
        z: 1
    }
}
