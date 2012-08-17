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
    /qmlclass ActivityIndicator
    \inqmlmodule Ubuntu.Components 0.1
    \brief The ActivityIndicator class is DOCME

    \b{This component is under heavy development.}

    Example:
    \qml

        ActivityIndicator {
            id: activity
        }

        Button {
            id: toggleActive
            text: (activity.active) ? "Deactivate" : "Activate"
            onClicked: activity.active = !activity.active
        }
        Button {
            id: toggleStalled
            text: (activity.stalled) ? "Resume" : "Stall"
            onClicked: activity.stalled = !activity.stalled
        }
    \endqml
*/
Item {
    id: indicator

    width: height
    height: 62

    /*!
       \preliminary
       Presents whether there is activity to be visualized or not. The default value is false.
       When activated (set to true), an animation is shown indicating an ongoing activity. Ones
       is set to false (deactivated) the finished() signal is triggered.
    */
    property bool active: false

    /*!
       \preliminary
       Controls the stalled state of the indicator. Useful in long-running operations to indicate
       that the operation got stuck or the connection is interruptive. Altering its value does have
       effect only if the indicator is active. When the state is reset (set to false), the resumed()
       signal is triggered.
    */
    property bool stalled: false

    /*!
       \preliminary
       Signal emitted when the indicator gets activated.
    */
    signal started
    /*!
       \preliminary
       Signal emitted when the indicator's stalled state gets reset. The indicator must be active.
    */
    signal resumed
    /*!
       \preliminary
       Signal emitted when the indicator gets deactivated.
    */
    signal finished

    // Yet we'll do the indication with image rotation animation
    // timm we get the proper graphics and description from UX
    Image {
        id: animation
        anchors.fill: parent
        source: internals.payload
        visible: indicator.active & indicator.enabled
        fillMode: Image.Stretch
        NumberAnimation on rotation {running: indicator.active; from: 0; to: 360; loops: Animation.Infinite; duration: internals.animationDuration}

        states: [
            State {
                name: "on"
                when: indicator.active && !indicator.stalled
            },
            State {
                name: "stall"
                extend: "on"
                when: indicator.active && indicator.stalled
                PropertyChanges {
                    target: animation
                    opacity: internals.stalledOpacity
                }
            }
        ]
        transitions: [
            Transition {
                from: ""
                to: "on"
                ScriptAction {
                    script: {
                        indicator.started()
                        indicator.stalled = false
                        internals.wasActive = true
                    }
                }
            },
            Transition {
                from: "on"
                to: "stall"
                SequentialAnimation {
                     loops: Animation.Infinite
                     PropertyAnimation { property: "opacity"; duration: internals.animationDuration / 2}
                     PropertyAnimation { properties: "opacity"; to: 1.0; duration: internals.animationDuration / 2}
                }
            },
            Transition {
                from: "stall"
                to: "on"
                ScriptAction { script: indicator.resumed(); }
            },
            Transition {
                from: "*"
                to: ""
                ScriptAction {
                    script: if (internals.wasActive) {
                                internals.reset()
                                indicator.finished()
                            }
                }
            }

        ]

        // for debuging purposes only
        // onStateChanged: console.debug("ActivityIndicator animation state = " + state)
    }

    // internal properties
    QtObject {
        id: internals
        property bool wasActive: false
        // styling!
        property url payload: Qt.resolvedUrl("artwork/ActivityIndicator.png")
        property int animationDuration: 1000
        property real stalledOpacity: 0.1

        function reset()
        {
            wasActive = false
            indicator.stalled = false
        }
    }
}
