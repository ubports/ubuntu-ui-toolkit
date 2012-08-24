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
import Qt.labs.shaders 1.0


/*!
    \qmlclass ProgressBar
    \inqmlmodule Ubuntu.Components 0.1
    \brief The ProgressBar component visually indicates the progress of a task of
    unknown duration, e.g. download progress, or any determinate activity progress.
    It can also indicate activities when the actual status of the progress is unknown.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        ProgressBar {
            id: indeterminateBar
            indeterminate: true
        }

        ProgressBar {
            id: determinateBar
            minimumValue: -3.0
            maximumValue: 15
        }
    }
    \endqml
*/
AnimatedItem {
    id: progressBar

    width: 200
    height: frame.sourceSize.height

    /*!
      \preliminary
      Specifies whether the progress duration is unknown. When set, altering
      other properties do not have any effect on the component's behavior.
      By default the property is set to false.
    */
    property bool indeterminate: false

    /*!
      \preliminary
      Specifies the minimum value of the progress.
    */
    property real minimumValue: 0.0

    /*!
      \preliminary
      Specifies the maximum value of the progress.
    */
    property real maximumValue: 1.0

    /*!
      \preliminary
      Specifies the current value of the progress
    */
    property real value: 0.5

    // TODO: use proper images/styling
    BorderImage {
        id: frame
        anchors.fill: parent
        source: internals.barSource
        border.left: 2; border.top: 2
        border.right: 2; border.bottom: 2
        smooth: true
        clip: true
        Image {
            id: trackerIndeterminate
            visible: progressBar.indeterminate
            width: progressBar.width - 2 * internals.trackerOffset + 4 * sourceSize.width
            height: progressBar.height - 2 * internals.trackerOffset

            source: internals.unknownTrackerSource
            smooth: true
            fillMode: Image.Tile

            property real xTemp
            x: internals.trackerOffset + Math.round(trackerIndeterminate.xTemp)
            NumberAnimation on xTemp {
                running: progressBar.indeterminate && progressBar.visible && progressBar.onScreen
                loops: Animation.Infinite
                from: -trackerIndeterminate.sourceSize.width
                to: 0
                duration: (300 * trackerIndeterminate.sourceSize.width / 10)
            }
        }

        BorderImage {
            id: tracker
            visible: !progressBar.indeterminate
            anchors.fill: frame
            anchors.margins: internals.trackerOffset

            source: internals.knownTrackerSource
            anchors {
                leftMargin: internals.trackerOffset
                topMargin: internals.trackerOffset
                bottomMargin: internals.trackerOffset
                rightMargin: internals.trackerOffset + internals.progress()
            }
            smooth: true
        }
    }

    QtObject {
        id: internals
        // style properties, preparation for theming
        property url barSource: Qt.resolvedUrl("artwork/ProgressBarBackground.png")
        property url knownTrackerSource: Qt.resolvedUrl("artwork/ProgressBarTracker.png")
        property url unknownTrackerSource: Qt.resolvedUrl("artwork/ProgressBarTrackerIndeterminate.png")
        property int trackerOffset: 1

        function progress()
        {
            if (progressBar.indeterminate)
                return 0
            var normalValue = progressBar.value
            if (normalValue < progressBar.minimumValue)
                normalValue = progressBar.minimumValue
            if (normalValue > progressBar.maximumValue)
                normalValue = progressBar.maximumValue
            var area = Math.abs(progressBar.maximumValue - progressBar.minimumValue)
            var progress = Math.abs((normalValue + Math.abs(progressBar.minimumValue)) / area)
            var trackerW = progressBar.width - 2 * trackerOffset
            return trackerW - trackerW * progress
        }
    }
}
