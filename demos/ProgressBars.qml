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
import Ubuntu.Components 0.1

Template {
    title: "Progress Bar"
    objectName: "ProgressBarsTemplate"

    Column {
        spacing: 30

        TemplateRow {
            title: "Known %"

            Button {
                text: "Reset"
                onClicked: progress.value = progress.minimumValue
            }

            ProgressBar {
                id: progress
                objectName: "DeterminateProgressBar"
                value: progress.minimumValue
                minimumValue: -10.0
                maximumValue: 1.0

                NumberAnimation on value {
                    from: progress.minimumValue
                    to: progress.maximumValue
                    running: progress.value < progress.maximumValue
                    duration: 1000
                }
            }
        }

        TemplateRow {
            title: "Unknown %"

            ProgressBar {
                objectName: "IndeterminateProgressBar"
                indeterminate: true
            }
        }
    }
}
