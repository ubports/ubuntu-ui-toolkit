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
    \qmlclass ActivityIndicator
    \inqmlmodule Ubuntu.Components 0.1
    \brief The ActivityIndicator component provides ability to visualize indetermined
    term operations, e.g. busy indication, connection in progress indication, etc. The
    graphics and their animation shown as indication is driven by themes.


    \b{This component is under heavy development.}

    Example:
    \qml
        ActivityIndicator {
            id: activity

            onStarted: console.log("activity started")
            onFinished: console.log("activity finished")
        }

        Button {
            id: toggleActive
            text: (activity.active) ? "Deactivate" : "Activate"
            onClicked: activity.active = !activity.active
        }
    \endqml
*/
Item {
    id: indicator

    width: animation.sourceSize.width
    height: animation.sourceSize.height

    /*!
       \preliminary
       Presents whether there is activity to be visualized or not. The default value is false.
       When activated (set to true), an animation is shown indicating an ongoing activity, and
       the started() signal is emitted. Upon deactivation the finished() signal is emitted.
    */
    property bool active: false

    /*!
       \preliminary
       This is a helper signal emitted when the indicator gets activated.
    */
    signal started
    /*!
       \preliminary
       This is a helper signal emitted when the indicator gets deactivated.
    */
    signal finished

    // Yet we'll do the indication with image rotation animation
    // till we get the proper graphics and description from UX
    Image {
        id: animation
        anchors.fill: parent
        source: internals.source
        smooth: internals.smooth
        visible: indicator.active & indicator.enabled
        NumberAnimation on rotation {running: indicator.active; from: 0; to: 360; loops: Animation.Infinite; duration: internals.animationDuration}

        states: [
            State {
                name: "on"
                when: indicator.active
            }
        ]
        transitions: [
            Transition {
                from: ""
                to: "on"
                ScriptAction { script: indicator.started() }
            },
            Transition {
                from: "*"
                to: ""
                ScriptAction { script: indicator.finished() }
            }

        ]
    }

    // internal properties
    QtObject {
        id: internals
        // preliminary theming introduced to ease styling introduction
        property url source: Qt.resolvedUrl("artwork/ActivityIndicator.png")
        property bool smooth: true
        property int animationDuration: 1000
    }

    //TODO: add flickable support - deactivate when out of screen
}
