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
    \brief The ActivityIndicator component visually indicates that a task of
    unknown duration is in progress, e.g. busy indication, connection in progress
    indication, etc.

    Note: for time consuming Java script operations use WorkerScript, otherwise no
    UI interaction will be possible and the ActicityIndicator amination will freeze.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        ActivityIndicator {
            id: activity
        }

        Button {
            id: toggleActive
            text: (activity.running) ? "Deactivate" : "Activate"
            onClicked: activity.running = !activity.running
        }
    }
    \endqml
*/
AnimatedItem {
    id: indicator

    width: animation.sourceSize.width
    height: animation.sourceSize.height

    /*!
       \preliminary
       Presents whether there is activity to be visualized or not. The default value is false.
       When activated (set to true), an animation is shown indicating an ongoing activity, and
       the started() signal is emitted. Upon deactivation the finished() signal is emitted.
    */
    property bool running: false

    /*!
       \preliminary
       Signal emitted when the indicator gets activated.
    */
    signal started

    /*!
       \preliminary
       Signal emitted when the indicator gets deactivated.
    */
    signal finished

    // Yet we'll do the indication with image rotation animation
    // till we get the proper graphics and description from UX
    Image {
        id: animation
        anchors.fill: parent
        source: internals.source
        smooth: internals.smooth
        visible: indicator.running && indicator.enabled
        NumberAnimation on rotation {
            running: animation.visible & indicator.onScreen
            from: 0; to: 360; loops: Animation.Infinite
            duration: internals.animationDuration
        }

        states: [
            State {
                name: "on"
                when: indicator.running
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
        // TODO: image is not the final one, however might change with theming
        property url source: Qt.resolvedUrl("artwork/ActivityIndicator.png")
        property bool smooth: true
        property int animationDuration: 1000
    }
}
