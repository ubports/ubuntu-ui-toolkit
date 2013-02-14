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

    property string longText: "<i>Lorem ipsum dolor sit amet, consectetur adipiscing elit.</i> "+
                              "<b>Nunc pretium iaculis risus, sed vehicula odio varius ac.</b> "+
                              "<u>Etiam orci lectus, bibendum in vulputate ac, tristique quis dui.</u> "+
                              "Suspendisse sed nulla in arcu dapibus vehicula ac eu tellus. "+
                              "Mauris malesuada nisl vitae neque auctor placerat. Ut vitae "+
                              "luctus libero. Praesent tempus nisl faucibus tellus semper "+
                              "eget iaculis purus congue. Donec elit tellus, malesuada non "+
                              "vehicula a, sagittis vel purus. Ut elementum vehicula accumsan. "+
                              "Pellentesque habitant morbi tristique senectus et netus et malesuada "+
                              "fames ac turpis egestas. Suspendisse ut rutrum sapien. Nullam id lobortis mauris"+
                              "Maecenas sollicitudin ipsum eu mauris vehicula rutrum. Proin aliquet "+
                              "justo ut mi luctus eu lobortis risus elementum. Cras sit amet "+
                              "sollicitudin magna. Quisque facilisis sodales tincidunt. Maecenas "+
                              "ligula tortor, luctus id elementum vel, varius vel augue. "+
                              "Nunc porta mattis bibendum. Nam vitae sapien ipsum, non viverra neque."

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
                text: "Are you sure you want to save this file?"

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
                callerMargin: units.gu(5)

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
                        text: "Item " + modelData
                        onClicked: print("Clicked item " + modelData)
                    }
                }
            }
        }

        Component {
            id: actionSelectionPopover
            ActionSelectionPopover {
                actions: ActionList {
                    Action {
                        text: i18n.tr("Selection #1")
                        onTriggered: print(text)
                    }
                    Action {
                        text: i18n.tr("Selection #2")
                        onTriggered: print(text)
                    }
                    Action {
                        text: i18n.tr("Selection #3")
                        onTriggered: print(text)
                    }
                    Action {
                        text: i18n.tr("Selection #4")
                        onTriggered: print(text)
                    }
                    Action {
                        text: i18n.tr("Selection #5")
                        onTriggered: print(text)
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
                                                "contentWidth": textItem.width,
                                                "callerMargin": units.gu(2)
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

                Button {
                    id: actionSelectionPopoverButton
                    text: i18n.tr("action list")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(actionSelectionPopover, actionSelectionPopoverButton)
                }
            }

            TemplateRow {
                title: i18n.tr("Sheets")

                Button {
                    id: sheet1
                    text: i18n.tr("close button")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(closeButtonSheet, sheet1)
                }

                Button {
                    id: sheet2
                    text: i18n.tr("done button")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(doneButtonSheet, sheet2)
                }

                Button {
                    id: sheet3
                    text: i18n.tr("composer")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(composerSheet, sheet3)
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

            TemplateRow {
                title: i18n.tr("Non-modal")

                Button {
                    id: nonModalPopover
                    text: i18n.tr("Popover")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(Qt.resolvedUrl("MyCustomPopover.qml"), nonModalPopover, {"autoClose": false})
                }

                Button {
                    id: nonModalSheet
                    text: i18n.tr("Sheet")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(composerSheet, nonModalSheet, {"modal": false, "contentWidth": units.gu(20)})
                }

                Button {
                    id: nonModalDialog
                    text: i18n.tr("save")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(dialog, nonModalDialog, {"modal": false})
                }
            }
        }
    }
}
