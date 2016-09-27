/*
 * Copyright 2016 Canonical Ltd.
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
import Ubuntu.Components 1.0

MainView {
    width: units.gu(50)
    height: units.gu(50)
    objectName: "mainView"

    // Make sure that for these tests the toolbar starts closed.
    Component.onCompleted: {
        __propagated.toolbar.close();
    }

    Page {

        Label {
            id: "label"
            objectName: "clicked_label"
            anchors.centerIn: parent
            text: "Button not clicked."
        }

        tools: ToolbarItems {
            ToolbarButton {
                objectName: "buttonName"
                action: Action {
                    text: "buttonText"
                    onTriggered: label.text = "Button clicked."
                }
            }
        }
    }
}
