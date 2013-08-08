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
import Ubuntu.Components 0.1 as Ubuntu

/*!
    \qmltype OptionSelector
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \ingroup ubuntu-listitems
    \brief List item displaying single selected value when not expanded,
    \where expanding it opens a listing of all the possible values for selection
    \with an additional option of always being expanded.

    \b{This component is under heavy development.}

    Examples:
    \qml
        import Ubuntu.Components 0.1
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
    property var values

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
      Called when a ListView delegate is clicked.
     */
    signal listItemClicked()

    /*!
      Called when the optionSelector is either expanded or collapsed.
     */
    signal scroll(real selectorHeight, string currentState)

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
            property int itemHeight: units.gu(5)
            property url chevron: __styleInstance.chevron
            property url tick: __styleInstance.tick
            property color themeColour: Theme.palette.selected.fieldText
            property bool colourComponent: __styleInstance.colourComponent

            anchors {
                left: parent.left
                right: parent.right
            }
            style: Theme.createStyleComponent("OptionSelectorStyle.qml", listContainer)

            onStateChanged: scroll(list.contentHeight, state)

            UbuntuNumberAnimation {
                id: opacity
            }

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
                        duration: Ubuntu.UbuntuAnimation.BriskDuration
                        }
                    }
                ]

            ListView {
                id: list

                interactive: false
                clip: true
                currentIndex: 0
                model: optionSelector.values
                anchors.fill: parent

                delegate:
                ListItem.Standard {
                    id: option

                    property bool currentItem: ListView.isCurrentItem

                    width: parent.width + units.gu(2)
                    height: listContainer.itemHeight
                    dividerOpacity: listContainer.isExpanded && index !== list.count - 1 ? 100 : 0
                    highlightWhenPressed: false
                    selected: ListView.isCurrentItem
                    anchors {
                        left: parent.left
                        leftMargin: units.gu(-2)
                    }
                    onClicked: {
                        listItemClicked();
                        if (listContainer.isExpanded) list.currentIndex = index;
                        if (!optionSelector.expanded) listContainer.isExpanded = !listContainer.isExpanded;
                    }

                    states: [ State {
                            name: "expanded"
                            when: listContainer.state === "expanded" && index !== list.currentIndex
                            PropertyChanges {
                                target: option
                                opacity: 1
                            }
                        }, State {
                            name: "collapsed"
                            when: listContainer.state === "collapsed" && index !== list.currentIndex
                            PropertyChanges {
                                target: option
                                opacity: 0
                            }
                        }
                    ]

                    transitions: [ Transition {
                            UbuntuNumberAnimation {
                                properties: "opacity"
                                duration: Ubuntu.UbuntuAnimation.FastDuration
                            }
                        }
                    ]

                    CrossFadeImage {
                        id: image

                        width: units.gu(2)
                        height: units.gu(2)
                        colour: listContainer.themeColour
                        colourise: true
                        opacity: option.selected ? 1.0 : 0.0
                        delayDuration: Ubuntu.UbuntuAnimation.FastDuration
                        fadeDuration: Ubuntu.UbuntuAnimation.FastDuration * 2
                        anchors {
                            right: parent.right
                            rightMargin: units.gu(2)
                            verticalCenter: parent.verticalCenter
                        }

                        Behavior on opacity {
                            UbuntuNumberAnimation {
                                properties: "opacity"
                                duration: Ubuntu.UbuntuAnimation.FastDuration
                            }
                        }

                        states: [ State {
                                name: "tick"
                                when: listContainer.isExpanded
                                PropertyChanges {
                                    target: image
                                    source: listContainer.tick
                                }
                            }, State {
                                name: "chevron"
                                when: !listContainer.isExpanded
                                PropertyChanges {
                                    target: image
                                    source: listContainer.chevron
                                }
                            }
                        ]
                    }

                    ListItem.LabelVisual {
                        text: modelData
                        anchors {
                        left: parent.left
                            leftMargin: units.gu(3)
                            verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }
    }
}
