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
import "ListItems" as ListItem
import Ubuntu.Components 0.1 as Components

/*!
    \qmltype OptionSelector
    \inqmlmodule Components.Components.ListItems 0.1
    \ingroup ubuntu-listitems
    \brief List item displaying single selected value when not expanded,
    where expanding it opens a listing of all the possible values for selection
    with an additional option of always being expanded.

    \b{This component is under heavy development.}

    Examples:
    \qml
        import Components.Components 0.1
        Column {
            width: 250
            OptionSelector {
                text: "Standard"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
            }
            OptionSelector {
                text: "Disabled"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                enabled: false
            }
            OptionSelector {
                text: "Expanded"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                expanded: true
            }
            OptionSelector {
                text: "Icon"
                icon: Qt.resolvedUrl("icon.png")
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                selectedIndex: 2
            }
        }
    \endqml
*/

/*FIXME:
There is a bug with UbuntuShape background which should be fixed once the new UbuntuShape changes land.*/

ListItem.Empty {
    id: optionSelector
    __height: column.height

    /*!
      \preliminary
      The list of values that will be shown under the label text. This is a model.
     */
    property var model

    /*!
      \qmlproperty int selectedIndex
      The index of the currently selected element from the \l values array.
     */
    property alias selectedIndex: list.currentIndex

    /*!
      \preliminary
      Specifies whether the list is always expanded.
     */
    property bool expanded: false

    /*!
      \preliminary
      ListView delegate.
     */
    property Component delegate: OptionSelectorDelegate {}

    /*!
      Called when a ListView delegate is clicked.
     */
    signal listItemClicked()

    /*!
      Called when the optionSelector is either expanded or collapsed.
     */
    signal scroll()

    showDivider: false

    Column {
        id: column

        spacing: units.gu(2)
        anchors {
            left: parent.left
            right: parent.right
        }

        ListItem.LabelVisual {
            text: optionSelector.text
            height: units.gu(2)
            fontSize: "medium"
        }

        StyledItem {
            id: listContainer
            objectName: "listContainer"

            property bool isExpanded: expanded
            property bool isFullyExpanded: expanded
            property int itemHeight: units.gu(5)
            property url chevron: __styleInstance.chevron
            property url tick: __styleInstance.tick
            property color themeColour: Theme.palette.selected.fieldText
            property bool colourComponent: __styleInstance.colourComponent

            anchors {
                left: parent.left
                right: parent.right
            }
            state: optionSelector.expanded ? state = "expanded" : state = "collapsed"
            style: Theme.createStyleComponent("OptionSelectorStyle.qml", listContainer)

            onHeightChanged: scroll()

            states: [ State {
                    name: "expanded"
                    when: listContainer.isExpanded
                    PropertyChanges {
                        target: listContainer
                        height: list.contentHeight
                    }
                }, State {
                    name: "collapsed"
                    when: !listContainer.isExpanded
                    PropertyChanges {
                        target: listContainer
                        height: itemHeight
                    }
                }
            ]

            transitions: [ Transition {
                UbuntuNumberAnimation {
                        properties: "height"
                        duration: Components.UbuntuAnimation.BriskDuration
                    }
                }
            ]

            ListView {
                id: list

                property int previousIndex: -1
                readonly property alias expanded: optionSelector.expanded
                readonly property alias container: listContainer

                interactive: false
                clip: true
                currentIndex: 0
                model: optionSelector.model
                anchors.fill: parent

                delegate: optionSelector.delegate
            }
        }
    }
}
