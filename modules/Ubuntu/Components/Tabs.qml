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

import QtQuick 1.1

/*!
    \qmlclass Tabs
    \inqmlmodule Ubuntu.Components 0.1
    \brief The Tabs class provides an environment where multible Tab
    children can be added, and the user is presented with a tab
    bar with tab buttons to select different tab pages.

    Examples:
    \qml
        Tabs {
            Page {
                title: "tab 1"
                contents: Text {
                    anchors.centerIn: parent
                    text: "This is the first tab."
                }
            }
            Page {
                title: "tab 2"
                contents: Text {
                    anchors.centerIn: parent
                    text: "Tab number two."
                }
            }
            Page {
                title: "tab 3"
                contents:  Rectangle {
                    id: tab3
                    anchors.fill: parent
                    Text {
                        anchors.centerIn: parent
                        text: "Colorful tab 3"
                    }
                    color: "lightblue"
                }
            }
        }
    \endqml

    \b{This component is under heavy development.}
*/

TabContainer {
    id: tabs

    /*!
      \preliminary
      Setting this property to true will make the tabs buttons use
      the maximum available space for the buttons.
      If the value is false, each button will use the width that
      is required by the largest button.
     */
    property bool buttonsExpanded: false

    /*!
      \preliminary
      The index of the currently selected tab.
      The first tab is 0, and -1 means that no tab is selected.
     */
    property int selectedTabIndex: 0

    /*!
      \preliminary
      If this optional property is specified, it will be positioned
      between the bar with tab buttons, and the tab pages to act
      as a separator.
     */
    property Item separator: Rectangle {
            color: "white"
            height: 1
    }

    // minimal width of 200
    width: 200
    height: buttonRow.height

    // encapsulation
    Item {
        id: visuals
        anchors.fill: parent

        // needed to set the anchors in setSeparator()
        property alias buttonRow: buttonRow
        property alias contentsContainer: contentsContainer

        Row {
            id: buttonRow
            width: buttonRow.buttonWidth * repeater.count
            height: 28
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }

            /*!
              \preliminary
              The padding on the left and right side of the row of buttons.
            */
            property real horizontalPadding: 10

            // maximumButtonWidth is the total space available for all tab buttons
            // divided by the number of buttons.
            // If minimumButtonWidth is larger than maximumButtonWidth, the buttonWidth
            // will be minimumButtonWidth. Otherwise, the buttonWidth is the width the
            // largest button needs to fit all its contents.
            // Scrolling in case the buttons don't fit in the available space is currently
            // not implemented.
            property int minimumButtonWidth: 20
            property int maximumButtonWidth: (visuals.width - 2*buttonRow.horizontalPadding) / repeater.count
            property bool needsScrolling: maximumButtonWidth < minimumButtonWidth
            property int widestButtonWidth
            property int buttonWidth
            buttonWidth: {
                if (buttonRow.needsScrolling) return buttonRow.minimumButtonWidth;
                else if (tabs.buttonsExpanded) return buttonRow.maximumButtonWidth;
                else return Math.min(buttonRow.maximumButtonWidth, buttonRow.widestButtonWidth);
            }

            function updateWidestButtonWidth() {
                var button;
                var widest = 0;
                for (var i=0; i < buttonRow.children.length; i++) {
                    button = buttonRow.children[i];
                    if (button === repeater) continue;
                    if (button.implicitWidth > widest) {
                        widest = button.implicitWidth;
                    }
                } // for i
                buttonRow.widestButtonWidth = widest;
            }

            Repeater {
                id: repeater
                onModelChanged: buttonRow.updateWidestButtonWidth()
                onCountChanged: buttonRow.updateWidestButtonWidth()

                model: tabs.tabs
                TabButton {
                    id: tabButton
                    property Item page: modelData
                    text: modelData.title
                    __isFirst: index === 0
                    __isLast: index === (repeater.count - 1)
                    iconSource: modelData.iconSource
                    width: buttonRow.buttonWidth
                    selected: (index === tabs.selectedTabIndex)
                    onClicked: tabs.selectedTabIndex = index
                }
            }
            Component.onCompleted: buttonRow.updateWidestButtonWidth()
        } // buttonRow

        // This is the item that will be the parent of the currently displayed page.
        Item {
            id: contentsContainer
            anchors {
                top: buttonRow.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
        }

        function selectedTabChanged() {
            var page;
            for (var i = 0; i < tabs.tabs.length; i++) {
                page = tabs.tabs[i].__pageObject
                if (i == tabs.selectedTabIndex) {
                    page.contentsParent = contentsContainer;
                    page.active = true;
                } else {
                    page.active = false;
                }
            }
        }

        function separatorChanged() {
            if (tabs.separator) {
                tabs.separator.parent = visuals;
                tabs.separator.anchors.top = visuals.buttonRow.bottom;
                tabs.separator.anchors.left = visuals.left;
                tabs.separator.anchors.right = visuals.right;
                visuals.contentsContainer.anchors.top = tabs.separator.bottom;
            } else { // no separator
                visuals.contentsContainer.anchors.top = visuals.buttonRow.bottom;
            }
        }

        Connections {
            target: tabs
            onSelectedTabIndexChanged: visuals.selectedTabChanged()
            onSeparatorChanged: visuals.separatorChanged()
        }
    } // visuals

    Component.onCompleted: {
        visuals.separatorChanged();
        visuals.selectedTabChanged();
    }
}
