/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: progressBarStyle

    property color foregroundColor: UbuntuColors.orange
    property color foregroundTextColor: '#FFFFFF'
    property color backgroundColor: theme.palette.normal.base
    property color backgroundTextColor: theme.palette.normal.baseText

    property var progressBar: styledItem

    implicitWidth: units.gu(38)
    implicitHeight: units.gu(4)

    UbuntuShapeOverlay {
        id: background
        anchors.fill: parent
        backgroundColor: progressBarStyle.backgroundColor
        overlayColor: foregroundColor
        overlayRect: Qt.application.layoutDirection == Qt.LeftToRight ?
            Qt.rect(0.0, 0.0, progressBarStyle.progress, 1.0) :
            Qt.rect(1.0 - progressBarStyle.progress, 0.0, 1.0, 1.0)
    }

    property real progress: progressBar.indeterminate ? 0.0
                            : progressBar.value / (progressBar.maximumValue - progressBar.minimumValue)

    Label {
        id: valueLabel
        anchors.centerIn: background
        color: backgroundTextColor
        text: progressBar.indeterminate ? i18n.dtr("ubuntu-ui-toolkit", "In Progress")
              : "%1%".arg(Number(progressBarStyle.progress * 100.0).toFixed(0))
        visible: !progressBar.hasOwnProperty("showProgressPercentage") || progressBar.showProgressPercentage

        SequentialAnimation on opacity {
            loops: Animation.Infinite
            running: progressBar.indeterminate
            UbuntuNumberAnimation {
                to: 0.2; duration: UbuntuAnimation.BriskDuration
            }
            UbuntuNumberAnimation {
                to: 1.0; duration: UbuntuAnimation.BriskDuration
            }
        }
    }

    PartialColorize {
        anchors.fill: valueLabel
        sourceItem: progressBar.indeterminate ? null : valueLabel
        leftColor: foregroundTextColor
        rightColor: backgroundTextColor
        progress: (progressBarStyle.progress * background.width - valueLabel.x) / valueLabel.width
        mirror: Qt.application.layoutDirection == Qt.RightToLeft
        visible: !progressBar.hasOwnProperty("showProgressPercentage") || progressBar.showProgressPercentage
    }
}
