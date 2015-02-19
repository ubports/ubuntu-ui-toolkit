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

import QtQuick 2.0
import Ubuntu.Components 1.1

MainView {
    id: root
    width: units.gu(48)
    height: units.gu(60)
    useDeprecatedToolbar: false

    Page {
        title: "Header"
        head.contents: TextField {
            id: searchTextField
            objectName: "textfield"
            placeholderText: "Header"
            inputMethodHints: Qt.ImhNoPredictiveText
            hasClearButton: false

            text: "Then two bears came out of the woods and mauled forty-two of the youths."

            anchors {
                fill: parent
                leftMargin: units.gu(1)
                topMargin: units.gu(1)
                bottomMargin: units.gu(1)
                rightMargin: root.width > units.gu(60) ? root.width - units.gu(40) : units.gu(1)
            }

            secondaryItem: AbstractButton {
                height: searchTextField.height
                width: height
                enabled: searchTextField.text.length > 0
                Image {
                    objectName: "clearIcon"
                    anchors.fill: parent
                    anchors.margins: units.gu(.75)
                    source: "image://theme/clear"
                    opacity: searchTextField.text.length > 0
                    visible: opacity > 0
                    Behavior on opacity {
                        UbuntuNumberAnimation { duration: UbuntuAnimation.FastDuration }
                    }
                }
            }
        }

        Column {
            Label {
                text: "Above is a text field in the header"
            }
        }
    }
}
