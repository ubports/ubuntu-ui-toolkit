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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components 1.3
import Ubuntu.Layouts 1.0

Item {
    id: buttonGrid
    Layouts.item: "buttons"

    property real spacing: units.gu(0.5)
    property real buttonHeight: (buttonGrid.height - 5 * buttonGrid.spacing) / 6
    property real buttonWidth: (buttonGrid.width - 3 * buttonGrid.spacing) / 4

    property var operations

    property var __model: [
        {"label": "mc", "color": "darkgray", "width" : buttonWidth, "height": buttonHeight},
        {"label": "m+", "color": "darkgray", "width" : buttonWidth, "height": buttonHeight},
        {"label": "m-", "color": "darkgray", "width" : buttonWidth, "height": buttonHeight},
        {"label": "mr", "color": "darkgray", "width" : buttonWidth, "height": buttonHeight},

        {"label": "c", "color": "brown", "width" : buttonWidth, "height": buttonHeight},
        {"label": "+/-", "color": "brown", "width" : buttonWidth, "height": buttonHeight},
        {"label": "/", "color": "brown", "width" : buttonWidth, "height": buttonHeight},
        {"label": "*", "color": "brown", "width" : buttonWidth, "height": buttonHeight},

        {"label": "7", "color": "#5f5f5f", "width" : buttonWidth, "height": buttonHeight},
        {"label": "8", "color": "#5f5f5f", "width" : buttonWidth, "height": buttonHeight},
        {"label": "9", "color": "#5f5f5f", "width" : buttonWidth, "height": buttonHeight},
        {"label": "-", "color": "brown", "width" : buttonWidth, "height": buttonHeight},

        {"label": "4", "color": "#5f5f5f", "width" : buttonWidth, "height": buttonHeight},
        {"label": "5", "color": "#5f5f5f", "width" : buttonWidth, "height": buttonHeight},
        {"label": "6", "color": "#5f5f5f", "width" : buttonWidth, "height": buttonHeight},
        {"label": "+", "color": "brown", "width" : buttonWidth, "height": buttonHeight},

        {"label": "1", "color": "#5f5f5f", "width" : buttonWidth, "height": buttonHeight},
        {"label": "2", "color": "#5f5f5f", "width" : buttonWidth, "height": buttonHeight},
        {"label": "3", "color": "#5f5f5f", "width" : buttonWidth, "height": buttonHeight},
        {"label": "=", "color": "orange", "width" : buttonWidth, "height": buttonHeight * 2 + spacing},
    ]
    property var __lastRow: [
        {"label": "0", "color": "#5f5f5f", "width" : 2 * buttonWidth + spacing, "height": buttonHeight},
        {"label": ".", "color": "#5f5f5f", "width" : buttonWidth, "height": buttonHeight},
    ]
    Grid {
        id: grid
        spacing: buttonGrid.spacing
        anchors.fill: parent

        // memory
        Repeater {
            model: __model
            delegate: Button {
                text: modelData.label
                color: modelData.color
                width: modelData.width
                height: modelData.height
                onClicked: operations.execute(modelData.label)
            }
        }
    }
    Row {
        anchors{
            top: grid.bottom
            topMargin: -buttonHeight
        }

        spacing: buttonGrid.spacing

        // memory
        Repeater {
            model: __lastRow
            delegate: Button {
                text: modelData.label
                color: modelData.color
                width: modelData.width
                height: modelData.height
                onClicked: operations.execute(modelData.label)
            }
        }
    }
}
