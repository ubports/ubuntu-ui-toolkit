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

/*!
    \qmltype OptionSelector
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \ingroup ubuntu-listitems
    \brief List item displaying single selected value when not expanded,
    where expanding it opens a listing of all the possible values for selection.

    \b{This component is under heavy development.}

    Examples:
    \qml
        import Ubuntu.Components.ListItems 0.1 as ListItem
        Column {
            width: 250
            ListItem.OptionSelector {
                text: "Standard"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
            }
            ListItem.OptionSelector {
                text: "Disabled"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                enabled: false
            }
            ListItem.OptionSelector {
                text: "Expanded"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                expanded: true
            }
            ListItem.OptionSelector {
                text: "Icon"
                icon: Qt.resolvedUrl("icon.png")
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                selectedIndex: 2
            }
        }
    \endqml
*/
ListItem.Empty {
    id: selector
    __height: column.height

    /*!
      \internal
      \deprecated
      Width of the icon to be displayed
    */
    property real __iconWidth

    /*!
      \internal
      \deprecated
      Height of the icon to be displayed
    */
    property real __iconHeight

    /*!
      \internal
      \deprecated
      The margins on the left side of the icon.
     */
    property real __leftIconMargin

    /*!
      \internal
      \deprecated
      The margins on the right side of the icon.
     */
    property real __rightIconMargin

    /*!
      \preliminary
      The list of values that will be shown under the label text
     */
    property variant values

    /*!
      \preliminary
      The index of the currently selected element from the \l values array.
     */
    property alias selectedIndex: list.currentIndex

    /*!
      \preliminary
      Specifies whether the list is always expanded.
     */
    property bool expanded: false

    showDivider: false
    style: Theme.createStyleComponent("OptionSelectorStyle.qml", selector)

    Column {
        id: column

        anchors {
            left: parent.left
            right: parent.right
        }

        ListItem.Header {
            text: selector.text
        }

        ListView {
            id: list

            property bool isExpanded: expanded
            property int itemHeight: units.gu(5)

            interactive: false
            clip: true
            anchors {
                left: parent.left
                right: parent.right
            }

            states: [ State {
                    name: "expanded"
                    when: list.isExpanded
                    PropertyChanges {
                        target: list
                        height: list.contentHeight
                    }
                }, State {
                    name: "closed"
                    when: !list.isExpanded
                    PropertyChanges {
                        target: list
                        height: itemHeight
                    }
                }
            ]

            Behavior on height {
                UbuntuNumberAnimation {
                    duration: UbuntuAnimation.SnapDuration
                }
            }

            model: selector.values
            currentIndex: 0

            delegate:
            Base {
                id: valueBase

                width: parent.width + units.gu(4)
                height: list.itemHeight
                showDivider: true
                highlightWhenPressed: false
                anchors {
                    left: parent.left
                    leftMargin: units.gu(-2)
                }
                onClicked: {
                    if (list.isExpanded) list.currentIndex = index
                    if (!selector.expanded) list.isExpanded = !list.isExpanded
                }

                selected: ListView.isCurrentItem

                Image {
                    id: tickIcon

                    property string downChevron: "artwork/chevron_down_gradient@30.png"
                    property string tick: "artwork/tick_light@30.png"

                    width: units.gu(2)
                    height: units.gu(2)
                    opacity: enabled ? 1.0 : 0.5
                    visible: valueBase.selected
                    anchors {
                        right: parent.right
                        rightMargin: units.gu(2)
                        verticalCenter: parent.verticalCenter
                    }
                    source: {
                        if (!selector.expanded) {
                            if (!list.isExpanded) {
                                downChevron
                            } else {
                                tick
                            }
                        } else {
                            tick
                        }
                    }
                }

                LabelVisual {
                    text: modelData
                    anchors {
                    left: parent.left
                        leftMargin: units.gu(1)
                        verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }
}
