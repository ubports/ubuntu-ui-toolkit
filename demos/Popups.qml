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

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Template {
    objectName: "Popups"
    title: i18n.tr("Popovers, Sheets & Dialogues")

    Item {
        Component {
            id: closeButtonSheet
            DefaultSheet {
                id: sheet
                title: "Default sheet with close button"
                Label {
                    anchors.fill: parent
                    text: "The Default Sheet allows an application to insert a content view over the focused view without disrupting the navigation pattern (tabs state or drill-down path are maintained. When the sheet is dismissed the user continues the journey from the point he left it)."
                    wrapMode: Text.WordWrap
                }
            }
        }

        Component {
            id: doneButtonSheet
            DefaultSheet {
                id: sheet
                title: "Default sheet with done button"
                doneButton: true
                Label {
                    anchors.fill: parent
                    text: "The Default Sheet allows an application to insert a content view over the focused view without disrupting the navigation pattern (tabs state or drill-down path are maintained. When the sheet is dismissed the user continues the journey from the point he left it)."
                    wrapMode: Text.WordWrap
                }
            }
        }

        Component {
            id: composerSheet
            ComposerSheet {
                id: sheet
                title: "Composer sheet"
                Label {
                    text: "A composer sheet has cancel and confirm buttons."
                }
            }
        }

        Component {
            id: dialog
            Dialog {
                id: dialogue

                title: "Save file"
                text: "Are you sure that you want to save this file?"

                Button {
                    text: "cancel"
                    onClicked: PopupUtils.close(dialogue)
                }
                Button {
                    text: "overwrite previous version"
                    color: "#5da357"
                    onClicked: PopupUtils.close(dialogue)
                }
                Button {
                    text: "save a copy"
                    color: "#5da357"
                    onClicked: PopupUtils.close(dialogue)
                }
            }
        }

        Component {
            id: popoverWithFlickable

            Popover {
                id: popover

                Flickable {
                    id: flickable
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }
                    height: units.gu(30)

                    contentHeight: image.sourceSize.height
                    contentWidth: image.sourceSize.width
                    clip: true
                    Image {
                        id: image
                        source: Qt.resolvedUrl("demo_image.jpg")
                    }
                }
            }
        }

        Component {
            id: popoverWithListView

            Popover {
                id: popover

                ListView {
                    clip: true
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }
                    model: 20
                    height: units.gu(50)
                    delegate: ListItem.Standard {
                        icon: Qt.resolvedUrl("avatar_contacts_list.png")
                        text: "Item "+index
                        onClicked: print("Clicked item "+index)
                    }
                }
            }
        }


        Column {
            spacing: units.gu(4)

            TemplateRow {
                title: i18n.tr("Popover")

                Button {
                    id: leftPopoverButton
                    text: i18n.tr("automatic..")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(Qt.resolvedUrl("MyCustomPopover.qml"), leftPopoverButton, {"leftMargin": 20, "contentWidth": 400})
                }

                Button {
                    id: rightPopoverButton
                    text: i18n.tr("..positioning")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(Qt.resolvedUrl("MyCustomPopover.qml"), rightPopoverButton)
                }

                TextField {
                    id: textItem
                    placeholderText: "press to activate"
                    primaryItem: Button {
                        id: overlay
                        ItemStyle.class: "transparent-button"
                        iconSource: Qt.resolvedUrl("call_icon.png")
                        width: units.gu(4)
                        height: parent.height
                        onClicked: {
                            PopupUtils.open(Qt.resolvedUrl("MyCustomPopover.qml"), textItem,
                                            {
                                                "pointerTarget": overlay,
                                                "contentWidth": textItem.width
                                            })
                        }
                    }
                }
            }

            TemplateRow {
                title: i18n.tr("Contents")

                Button {
                    id: popoverWithFlickableButton
                    text: i18n.tr("flickable")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(popoverWithFlickable, popoverWithFlickableButton)
                }

                Button {
                    id: popoverWithListViewButton
                    text: i18n.tr("list view")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(popoverWithListView, popoverWithListViewButton)
                }
            }

            TemplateRow {
                title: i18n.tr("Sheets")

                Button {
                    text: i18n.tr("close button")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(closeButtonSheet)
                }

                Button {
                    text: i18n.tr("done button")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(doneButtonSheet)
                }

                Button {
                    text: i18n.tr("composer")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(composerSheet)
                }
            }

            TemplateRow {
                title: i18n.tr("Dialog")

                Button {
                    id: saveButton
                    text: i18n.tr("save")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(dialog, saveButton)
                }
            }
        }
    }
}
