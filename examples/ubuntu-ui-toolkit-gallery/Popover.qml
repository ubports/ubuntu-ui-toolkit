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
 */

import QtQuick 2.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Template {
    objectName: "popoversTemplate"

    tools: ToolbarItems {
        ToolbarButton {
            id: actionsButton
            text: "Actions"
            iconSource: "call_icon.png"
            onTriggered: PopupUtils.open(actionSelectionPopover, actionsButton)
            visible: true
        }
        locked: true
        opened: true
    }

    TemplateSection {
        className: "Popover"
        documentation: "qml-ubuntu-components-popups-%1.html".arg(className.toLowerCase())

        TemplateRow {
            title: i18n.tr("Flickable")

            Button {
                id: popoverWithFlickableButton
                text: i18n.tr("Open")
                width: units.gu(16)
                onClicked: PopupUtils.open(popoverWithFlickable, popoverWithFlickableButton)
            }
        }
    }

    Item {
        Component {
            id: popoverWithFlickable

            Popover {
                Flickable {
                    width: parent.width
                    height: units.gu(35)
                    contentWidth: image.sourceSize.width
                    contentHeight: image.sourceSize.height

                    Image {
                        id: image
                        source: Qt.resolvedUrl("demo_image.jpg")
                    }
                }
            }
        }

        Component {
            id: actionSelectionPopover

            ActionSelectionPopover {
                actions: ActionList {
                    Action {
                        text: i18n.tr("Action #1")
                        onTriggered: print(text)
                    }
                    Action {
                        text: i18n.tr("Action #2")
                        onTriggered: print(text)
                    }
                    Action {
                        text: i18n.tr("Action #3")
                        onTriggered: print(text)
                    }
                    Action {
                        text: i18n.tr("Action #4")
                        onTriggered: print(text)
                    }
                    Action {
                        text: i18n.tr("Action #5")
                        onTriggered: print(text)
                    }
                }
            }
        }
    }
}
