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

Template {
    objectName: "listItemLayoutTemplate"

    TemplateSection {
        className: "ListItemLayout"
        title: "Examples of core apps list items"
        // no spacing between the list items in the Column
        spacing: 0

        ListItem {
            id: addressbookListItem
            height: addressbookLayout.height + (divider.visible ? divider.height : 0)

            ListItemLayout {
                id: addressbookLayout

                title.color: UbuntuColors.lightAubergine
                title.text: "Address book list item: Name goes here"

                Rectangle {
                    SlotsLayout.position: SlotsLayout.Leading
                    color: "pink"
                    height: units.gu(6)
                    width: height
                    visible: width > 0
                }
            }
        }

        ListItem {
            id: addressbookDetailsListItem
            height: addressbookDetailsLayout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: addressbookDetailsLayout

                title.color: UbuntuColors.lightAubergine
                title.text: "+12 3456789012"
                subtitle.text: "Mobile"

                Icon {
                    name: "message"
                    height: units.gu(2)
                }

                Icon {
                    name: "call-start"
                    height: units.gu(2)
                }
            }

        }


        ListItem {
            id: telegramContactsListItem

            height: telegramContactsLayout.height + (divider.visible ? divider.height : 0)

            ListItemLayout {
                id: telegramContactsLayout
                title.text: "Telegram Name goes here"
                subtitle.text: "last seen 12:02"
                summary.text: "Oh wow! What a nice layout!"

                Rectangle {
                    id: imageShape
                    SlotsLayout.position: SlotsLayout.Leading
                    height: units.gu(6)
                    width: height
                    color: "red"
                }
            }
        }

        ListItem {
            id: systemSettings1
            height: systemSettings1_layout.height + (divider.visible ? divider.height : 0)

            ListItemLayout {
                id: systemSettings1_layout
                title.text: "Call forwarding"

                ProgressionSlot {}

                Text {
                    font.pixelSize: FontUtils.sizeToPixels("medium")
                    text: "Off"
                    color: "#525252"
                }
            }
        }

        ListItem {
            id: systemSettings2
            height: systemSettings2_layout.height + (divider.visible ? divider.height : 0)

            ListItemLayout {
                id: systemSettings2_layout
                title.text: "Flight mode"

                //see http://bazaar.launchpad.net/~ubuntu-branches/ubuntu/wily/ubuntu-system-settings/wily-proposed/view/head:/src/qml/EntryComponent.qml
                Icon {
                    SlotsLayout.position: SlotsLayout.Leading
                    width: units.gu(6);
                    height: units.gu(6);
                    name: "airplane-mode"
                }
                Switch { }
            }
        }

        ListItem {
            id: systemSettings3
            height: systemSettings3_layout.height + (divider.visible ? divider.height : 0)

            ListItemLayout {
                id: systemSettings3_layout
                title.text: "الفيسبوك"
                subtitle.text: "الفيسبوك"

                LayoutMirroring.enabled: true
                LayoutMirroring.childrenInherit: true

                ProgressionSlot {}

                Icon {
                    SlotsLayout.position: SlotsLayout.Leading
                    width: units.gu(6);
                    height: units.gu(6);
                    name: "facebook"
                }
            }
        }
    }

    TemplateSection {
        className: "ListItemLayout"
        title: "Custom labels properties"
        spacing: 0

        ListItem {
            height: customLabelsLayout.height + (divider.visible ? divider.height : 0)

            ListItemLayout {
                id: customLabelsLayout
                title.text: "Lorem ipsum..."
                title.color: "blue"
                title.horizontalAlignment: Text.AlignHCenter

                subtitle.text: "..dolor sit amet, consectetur adipiscing elit..."
                subtitle.color: "red"
                subtitle.horizontalAlignment: Text.AlignHCenter

                summary.text: "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua." +
                    "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi" +
                    "ut aliquip ex ea commodo consequat."
                summary.color: "green"
                summary.horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    TemplateSection {
        className: "ListItemLayout"
        title: "With swiping actions"
        // no spacing between the list items in the Column
        spacing: 0

        ListItemActions {
            id: exampleLeadingActions
            actions: [
                Action {
                    iconName: "delete"
                }
            ]
        }
        ListItemActions {
            id: exampleTrailingActions
            actions: [
                Action {
                    iconName: "edit"
                },
                Action {
                    iconName: "save-to"
                }
            ]
        }

        ListItemWithLabel {
            title.text: i18n.tr("Leading and trailing actions")
            leadingActions: exampleLeadingActions
            trailingActions: exampleTrailingActions
        }

    }

    TemplateSection {
        className: "ListItemLayout"
        title: "Custom vertical positioning of slots"
        // no spacing between the list items in the Column
        spacing: 0

        ListItem {
            id: dialerHistoryListItem
            height: dialerHistoryLayout.height + (divider.visible ? divider.height : 0)

            ListItemLayout {
                id: dialerHistoryLayout

                title.text: "+12 3456789012 (3)"
                title.color: UbuntuColors.lightAubergine
                subtitle.text: "Mobile"

                Rectangle {
                    SlotsLayout.position: SlotsLayout.Leading
                    color: "pink"
                    height: units.gu(6)
                    width: height
                    visible: width > 0
                }

                Item {
                    id: slot
                    width: label2.width
                    height: parent.height
                    //as we want to position labels to align with title and subtitle
                    SlotsLayout.overrideVerticalPositioning: true

                    Label {
                        id: lab
                        anchors.right: label2.right
                        text: "19:17"
                        fontSize: "small"
                        y: dialerHistoryLayout.mainSlot.y + dialerHistoryLayout.title.y
                           + dialerHistoryLayout.title.baselineOffset - baselineOffset
                    }

                    Label {
                        id: label2
                        text: "Outgoing"
                        fontSize: "small"
                        y: dialerHistoryLayout.mainSlot.y + dialerHistoryLayout.subtitle.y
                           + dialerHistoryLayout.subtitle.baselineOffset - baselineOffset
                    }
                }
            }
        }
    }
}
