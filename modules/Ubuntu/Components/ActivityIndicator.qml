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

    /*!
       \preliminary
       Presents whether there is activity to be visualized or not. The default value is false.
       When activated (set to true), an animation is shown indicating an ongoing activity, and
       the started() signal is emitted. Upon deactivation the finished() signal is emitted.
    */
    property bool running: false

    // use one of the image size for the implicit size
    implicitWidth: center.sourceSize.width
    implicitHeight: center.sourceSize.height
    // embedd visuals
    Image {
        id: center
        anchors.fill: parent

        fillMode: Image.PreserveAspectFit
        source: internals.centralSource
        smooth: internals.smooth
        visible: internals.active

        Image {
            id: animation
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            source: internals.movingSource
            smooth: internals.smooth
            visible: internals.active
            NumberAnimation on rotation {
                running: internals.active & indicator.onScreen
                from: 0; to: 360; loops: Animation.Infinite
                duration: internals.animationDuration
            }
        }
    }

    // internal properties
    QtObject {
        id: internals
        property bool active: indicator.running && indicator.enabled
        // preliminary theming introduced to ease styling introduction
        property url centralSource: Qt.resolvedUrl("artwork/ActivityIndicatorCentre.png")
        property url movingSource: Qt.resolvedUrl("artwork/ActivityIndicatorMoving.png")
        property bool smooth: true
        property int animationDuration: 1000
    }
}
