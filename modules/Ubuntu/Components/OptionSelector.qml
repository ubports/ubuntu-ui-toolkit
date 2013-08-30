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
import Ubuntu.Components 0.1 as Toolkit

/*!
    \qmltype OptionSelector
    \inqmlmodule Components.Components 0.1
    \ingroup ubuntu-components
    \brief Component displaying a single selected value with and optional image and subtext when not expanded, where expanding
    it opens a listing of all the possible values for selection with an additional option of always being expanded.

    \b{This component is under heavy development.}

    Examples:
    \qml
        import Components.Components 0.1
        Column {
            width: 250
            OptionSelector {
                text: "Standard"
                model: ["Value 1", "Value 2", "Value 3", "Value 4"]
            }
            OptionSelector {
                text: "Disabled"
                model: ["Value 1", "Value 2", "Value 3", "Value 4"]
                enabled: false
            }
            OptionSelector {
                text: "Expanded"
                model: ["Value 1", "Value 2", "Value 3", "Value 4"]
                expanded: true
            }
            OptionSelector {
                text: "Icon"
                icon: Qt.resolvedUrl("icon.png")
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                selectedIndex: 2
            }
            OptionSelector {
                text: i18n.tr("Label")
                model: customModel
                expanded: true
                colourImage: true
                delegate: OptionSelectorDelegate { text: name; subText: description; icon: image }
            }
            ListModel {
                id: customModel
                ListElement { name: "Name 1"; description: "Description 1"; image: "images.png" }
                ListElement { name: "Name 2"; description: "Description 2"; image: "images.png" }
                ListElement { name: "Name 3"; description: "Description 3"; image: "images.png" }
                ListElement { name: "Name 4"; description: "Description 4"; image: "images.png" }
            }
        }
    \endqml
*/

ListItem.Empty {
    id: optionSelector
    __height: column.height

    /*!
      \preliminary
      The list of values that will be shown under the label text. This is a model.
     */
    property var model

    /*!
      \preliminary
      Specifies whether the list is always expanded.
     */
    property bool expanded: false

    /*!
      \preliminary
      Colours image according to the fieldText colour of the theme, otherwise source colour is maintained.
     */
    property bool colourImage: false

    /*!
      \preliminary
      ListView delegate.
     */
    property Component delegate: OptionSelectorDelegate {}

    /*!
      \preliminary
      Custom height for list container which allows scrolling inside the selector.
     */
    property real containerHeight: list.contentHeight

    /*!
      \qmlproperty int selectedIndex
      The index of the currently selected element in our list.
     */
    property alias selectedIndex: list.currentIndex

    /*!
      Called when the optionSelector is either expanded or collapsed.
     */
    signal scroll()

    /*!
      Called when delegate is clicked.
     */
    signal delegateClicked(int index)

    showDivider: false

    Column {
        id: column

        spacing: units.gu(2)
        anchors {
            left: parent.left
            right: parent.right
        }

        Label {
            text: optionSelector.text
            height: units.gu(2)
        }

        StyledItem {
            id: listContainer
            objectName: "listContainer"

            readonly property url chevron: __styleInstance.chevron
            readonly property url tick: __styleInstance.tick
            readonly property color themeColour: Theme.palette.selected.fieldText
            readonly property bool isFullyExpanded: expanded
            readonly property bool colourImage: optionSelector.colourImage
            property bool isExpanded: expanded

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
                        height: containerHeight
                    }
                }, State {
                    name: "collapsed"
                    when: !listContainer.isExpanded
                    PropertyChanges {
                        target: listContainer
                        height: list.itemHeight
                    }
                }
            ]

            transitions: [ Transition {
                Toolkit.UbuntuNumberAnimation {
                        properties: "height"
                        duration: Toolkit.UbuntuAnimation.BriskDuration
                    }
                }
            ]

            ListView {
                id: list

                property int previousIndex: -1
                readonly property alias expanded: optionSelector.expanded
                readonly property alias container: listContainer
                property real itemHeight
                signal delegateClicked(int index)

                onDelegateClicked: optionSelector.delegateClicked();
                boundsBehavior: Flickable.StopAtBounds
                interactive: listContainer.height !== list.contentHeight && listContainer.isExpanded ? true : false
                clip: true
                currentIndex: 0
                model: optionSelector.model
                anchors.fill: parent

                delegate: optionSelector.delegate
            }
        }
    }
}
