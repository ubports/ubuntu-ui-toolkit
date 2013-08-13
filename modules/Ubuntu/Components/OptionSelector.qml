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
            property bool isFullyExpanded: expanded
            property int itemHeight: units.gu(5)
            property url chevron: __styleInstance.chevron
            property url tick: __styleInstance.tick
            property color themeColour: Theme.palette.selected.fieldText
            property bool colourComponent: __styleInstance.colourComponent

            readonly property string fragmentShader:
                "varying highp vec2 qt_TexCoord0;
                 uniform sampler2D source;
                 uniform lowp vec4 colour;
                 uniform lowp float qt_Opacity;

                 void main() {
                    lowp vec4 sourceColour = texture2D(source, qt_TexCoord0);
                    gl_FragColor = colour * sourceColour.a * qt_Opacity;
                }"

            anchors {
                left: parent.left
                right: parent.right
            }
            state: optionSelector.expanded ? state = "expanded" : state = "collapsed"
            style: Theme.createStyleComponent("OptionSelectorStyle.qml", listContainer)

            onStateChanged: scroll(list.contentHeight, state)

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
                    showDivider: listContainer.height !== listContainer.itemHeight && index !== list.count - 1 ? 1 : 0
                    highlightWhenPressed: false
                    selected: ListView.isCurrentItem
                    anchors {
                        left: parent.left
                        leftMargin: units.gu(-2)
                    }
                    onClicked: {
                        if (listContainer.isExpanded) list.currentIndex = index;
                        if (!optionSelector.expanded) listContainer.isExpanded = !listContainer.isExpanded;

                        listItemClicked();
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
                        },
                        //Since we don't want to add states to our divider, we use the exposed alias provided in Empty to access it and alter it's opacity from here.
                        State {
                            name: "dividerExpanded"
                            when: listContainer.state === "expanded" && index === list.currentIndex
                            PropertyChanges {
                                target: option.divider
                                opacity: 1
                            }
                        }, State {
                            name: "dividerCollapsed"
                            when: listContainer.state === "collapsed" && index === list.currentIndex
                            PropertyChanges {
                                target: option.divider
                                opacity: 0
                            }
                        }
                    ]

                    //As with our states, we apply the transition with our divider as the target.
                    transitions: [ Transition {
                            from: "dividerExpanded"
                            to: "dividerCollapsed"
                            UbuntuNumberAnimation {
                                target: option.divider
                                properties: "opacity"
                                duration: Ubuntu.UbuntuAnimation.SlowDuration
                            }
                        }
                    ]

                    resources: [
                        Connections {
                            target: listContainer
                            onIsExpandedChanged: {
                                optionExpansion.stop();
                                imageExpansion.stop();
                                optionCollapse.stop();
                                imageCollapse.stop();

                                if (listContainer.isExpanded === true) {
                                    if (!option.selected) {
                                        optionExpansion.start();

                                        //Ensure a source change. This solves a bug which happens occasionaly when source is switched correctly. Probably related to the image.source binding.
                                        image.source = listContainer.tick
                                    } else {
                                        imageExpansion.start();
                                    }
                                } else {
                                    if (!option.selected) {
                                        optionCollapse.start();

                                        //Ensure a source change. This solves a bug which happens occasionaly when source is switched correctly. Probably related to the image.source binding.
                                        image.source = listContainer.chevron
                                    } else {
                                        imageCollapse.start();
                                    }
                                }
                            }
                        }, SequentialAnimation {
                            id: imageExpansion

                            PropertyAnimation {
                                target: image
                                properties: "opacity"
                                from : 1.0
                                to: 0.0
                                duration: Ubuntu.UbuntuAnimation.FastDuration
                            }
                            PauseAnimation { duration: Ubuntu.UbuntuAnimation.BriskDuration - Ubuntu.UbuntuAnimation.FastDuration }
                            PropertyAction {
                                target: image
                                property: "source"
                                value: listContainer.tick
                            }
                            PropertyAnimation {
                                target: image
                                properties: "opacity"
                                from : 0.0
                                to: 1.0
                                duration: Ubuntu.UbuntuAnimation.FastDuration
                            }
                        }, PropertyAnimation {
                            id: optionExpansion

                            target: option
                            properties: "opacity"
                            from : 0.0
                            to: 1.0
                            duration: Ubuntu.UbuntuAnimation.SlowDuration
                        }, SequentialAnimation {
                            id: imageCollapse

                            PauseAnimation { duration: Ubuntu.UbuntuAnimation.FastDuration }
                            PropertyAnimation {
                                target: image
                                properties: "opacity"
                                from : 1.0
                                to: 0.0
                                duration: Ubuntu.UbuntuAnimation.FastDuration
                            }
                            PropertyAction {
                                target: image
                                property: "source"
                                value: listContainer.chevron
                            }
                            PropertyAnimation {
                                target: image
                                properties: "opacity"
                                from : 0.0
                                to: 1.0
                                duration: Ubuntu.UbuntuAnimation.FastDuration
                            }
                        }, PropertyAnimation {
                                id: optionCollapse
                                target: option
                                properties: "opacity"
                                from : 1.0
                                to: 0.0
                                duration: Ubuntu.UbuntuAnimation.SlowDuration
                        }
                    ]

                    Image {
                        id: image

                        width: units.gu(2)
                        height: units.gu(2)
                        source: optionSelector.expanded ? listContainer.tick : listContainer.chevron
                        opacity: option.selected ? 1.0 : 0.0
                        anchors {
                            right: parent.right
                            rightMargin: units.gu(2)
                            verticalCenter: parent.verticalCenter
                        }

                        Behavior on opacity {
                            enabled: optionSelector.expanded

                            UbuntuNumberAnimation {
                                properties: "opacity"
                                duration: Ubuntu.UbuntuAnimation.SlowDuration
                            }
                        }

                        ShaderEffect {
                            property color colour: listContainer.themeColour
                            property var source: parent

                            width: source.width
                            height: source.height
                            visible: source.status === Image.Ready

                            fragmentShader: fragmentShader
                        }
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
