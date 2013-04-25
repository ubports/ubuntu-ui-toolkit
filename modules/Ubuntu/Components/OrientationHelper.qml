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
import QtQuick.Window 2.0

Item {
    id: orientationHelper

    /*!
      \preliminary
      Sets whether the application will be automatically rotating when the
      device is.

      The default value is false.

      \qmlproperty bool automaticOrientation
     */
    property bool automaticOrientation: true

    /*! \internal */
    // FIXME: to be removed
    property bool __orientationDebugging: false

    /*! \internal */
    property int __orientationAngle: automaticOrientation ? Screen.angleBetween(Screen.primaryOrientation, Screen.orientation) : 0

    anchors.fill: parent

    Item {
        id: stateWrapper

        state: __orientationAngle.toString()

        states: [
            State {
                name: "0"
                PropertyChanges {
                    target: orientationHelper
                    rotation: 0
                }
            },
            State {
                name: "180"
                PropertyChanges {
                    target: orientationHelper
                    rotation: 180
                }
            },
            State {
                name: "270"
                PropertyChanges {
                    target: orientationHelper
                    rotation: 270
                    anchors {
                        leftMargin: (parent.width - parent.height) / 2
                        rightMargin: anchors.leftMargin
                        topMargin: -anchors.leftMargin
                        bottomMargin: anchors.topMargin
                    }
                }
            },
            State {
                name: "90"
                PropertyChanges {
                    target: orientationHelper
                    rotation: 90
                    anchors {
                        leftMargin: (parent.width - parent.height) / 2
                        rightMargin: anchors.leftMargin
                        topMargin: -anchors.leftMargin
                        bottomMargin: anchors.topMargin
                    }
                }
            }
        ]

        transitions: [
            Transition {
                id: orientationTransition
                ParallelAnimation {
                    SequentialAnimation {
                        PauseAnimation {
                            duration: 25
                        }
                        PropertyAction {
                            target: orientationHelper
                            properties: "anchors.topMargin,anchors.bottomMargin,anchors.rightMargin,anchors.leftMargin"
                        }
                    }
                    RotationAnimation {
                        target: orientationHelper
                        properties: "rotation"
                        duration: 250
                        easing.type: Easing.OutQuint
                        direction: RotationAnimation.Shortest
                    }
                }
            }
        ]
    }

    // FIXME: Orientation debugging to be removed
    /*! \internal */
    function __modulo(x, n) {
        return ((x % n) + n) % n;
    }

    /*! \internal */
    function __rotateBy(increment) {
        __orientationAngle = __modulo(__orientationAngle + increment, 360);
    }

    focus: true

    Keys.onReleased: {
        if (__orientationDebugging && !event.isAutoRepeat) {
            if (event.key == Qt.Key_Left) {
                __rotateBy(-90);
            } else if (event.key == Qt.Key_Right) {
                __rotateBy(90);
            }
        }
    }
}
