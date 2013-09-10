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
    \qmltype DialerHand
    \inqmlmodule Ubuntu.Components.Pickers 0.1
    \ingroup ubuntu-pickers
    \brief DialerHand represents a value selector on a Dialer.

    The DialerHands can be placed inside Dialer components only.
  */
StyledItem {
    id: hand

    /*!
      The property holds the selected value the dialer hand points to.
      */
    property real value

    /*!
      Specifies whether to show the hand marker or not. Similar to setting
      size to (0x0), however this does not alter the size.

      The default value is true.
      */
    property bool isHandVisible: true

    /*!
      The size of the hand. The height of the hand must be in the [0..Dialer.handSpace]
      range in order to have the hand displayed in the hand area, however there is no
      restriction applied on the size of the dialer hand.

      The hand can be hidden by setting 0x0 value to the size.

      The default value is (0.5GUxdialer.handSpace / 2).
      */
    property size size: Qt.size(units.gu(0.5), dialer.handSpace / 2)

    /*!
      Specifies whether the hand is draggable or not. When set to not draggable,
      the hand is used only to indicate the given value.

      The default value is true.
      */
    property bool draggable: true

    /*!
      The property holds the dialer instance the hand is assigned to. This is a
      helper property to enable access to the dialer component hosting the hand.
      */
    readonly property Dialer dialer: parent

    /*!
      \qmlproperty list<var> content
      \default
      The property holds the items that can be added to the hand. Note that these
      items will not be rotated together with the hand.
      */
    default property alias content: contentItem.data

    anchors.centerIn: parent
    width: parent.width
    height: parent.height
    style: Theme.createStyleComponent("DialerHandStyle.qml", hand)

    /*! \internal */
    onParentChanged: {
        if (QuickUtils.className(dialer) !== "Dialer") {
            console.log("WARNING: DialerHand can be a child of Dialer only.");
        }
    }

    /*! \internal */
    onValueChanged: {
        grabber.updateHand();
    }
    /*! \internal */
    Component.onCompleted: {
        grabber.updateHand();
    }

    Item {
        id: grabber
        parent: __styleInstance
        width: units.gu(4)
        height: width
        anchors {
            top: parent.top
            topMargin: units.gu(0.5)
            horizontalCenter: parent.horizontalCenter
        }
        Item {
            id: contentItem
            anchors.fill: parent
            rotation: 360 - __styleInstance.rotation
        }

        function updateHand() {
            if (!dialer) return;
            __styleInstance.rotation = MathUtils.projectValue(value,
                                                              dialer.minimumValue, dialer.maximumValue,
                                                              0.0, 360.0);
        }

        MouseArea{
            anchors.fill: parent;
            preventStealing: true;
            enabled: hand.draggable;
            property real centerX : hand.width / 2
            property real centerY : hand.height / 2
            property bool internalChange: false

            onPositionChanged:  {
                if (internalChange) return;
                internalChange = true;
                var point =  mapToItem (hand, mouse.x, mouse.y);
                var diffX = (point.x - centerX);
                var diffY = -1 * (point.y - centerY);
                var rad = Math.atan (diffY / diffX);
                var deg = (rad * 180 / Math.PI);

                if (diffX > 0 && diffY > 0) {
                    __styleInstance.rotation = 90 - Math.abs (deg);
                }
                else if (diffX > 0 && diffY < 0) {
                    __styleInstance.rotation = 90 + Math.abs (deg);
                }
                else if (diffX < 0 && diffY > 0) {
                    __styleInstance.rotation = 270 + Math.abs (deg);
                }
                else if (diffX < 0 && diffY < 0) {
                    __styleInstance.rotation = 270 - Math.abs (deg);
                }

                hand.value = MathUtils.projectValue(__styleInstance.rotation,
                                                    0.0, 360.0,
                                                    dialer.minimumValue, dialer.maximumValue);
                internalChange = false;
            }
        }
    }
}
