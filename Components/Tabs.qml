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
    \inqmlmodule UbuntuUIToolkit
    \brief The Tabs class provides an environment where multible Tab
    children can be added, and the user is presented with a tab
    bar with tab buttons to select different tab pages.

    Examples:
    \qml
        Tabs {
            Tab {
                text: "tab 1"
                page: Text {
                    anchors.centerIn: parent
                    text: "This is the first tab."
                }
            }
            Tab {
                text: "tab 2"
                page: Text {
                    anchors.centerIn: parent
                    text: "Tab number two."
                }
            }
            Tab {
                text: "tab 3"
                page:  Rectangle {
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

Item {
    id: tabsMain

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

    /*!
      \preliminary
      The height of the button row.
    */
    property real buttonRowHeight: 28

    // minimal width and height of 200 for showing the pages
    width: 200
    height: buttonRowHeight + 200

    /*!
      \preliminary
      The padding on the left and right side of the row of buttons.
    */
    property real buttonRowPadding: 10

    /*!
      \internal
      Keep the Tab items that the user defines separate
      from the other items that we create below.
    */
    default property alias children: tabItems.children
    Item {
        id: tabItems
    }

    // encapsulation
    Item {
        id: tabVisuals
        anchors.fill: parent

        // needed to set the anchors in setSeparator()
        property alias buttonRow: buttonRow
        property alias pages: pages

        Row {
            id: buttonRow
            width: buttonRow.buttonWidth * repeater.count
            height: tabs.buttonRowHeight
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            // maximumButtonWidth is the total space available for all tab buttons
            // divided by the number of buttons.
            // If minimumButtonWidth is larger than maximumButtonWidth, the buttonWidth
            // will be minimumButtonWidth. Otherwise, the buttonWidth is the width the
            // largest button needs to fit all its contents.
            // Scrolling in case the buttons don't fit in the available space is currently
            // not implemented.
            property int minimumButtonWidth: 20
            property int maximumButtonWidth: (tabVisuals.width - 2*tabs.buttonRowPadding) / repeater.count
            property bool needsScrolling: maximumButtonWidth < minimumButtonWidth
            property int widestButtonWidth
            property int buttonWidth
            buttonWidth: {
                if (buttonRow.needsScrolling) return buttonRow.minimumButtonWidth;
                else if (tabsMain.buttonsExpanded) return buttonRow.maximumButtonWidth;
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

                model: tabItems.children
                TabButton {
                    id: tabButton
                    property Item tab: modelData
                    text: tab.text
                    iconSource: tab.iconSource
                    width: buttonRow.buttonWidth
                    selected: (index == tabsMain.selectedTabIndex)
                    onClicked: tabVisuals.selectTab(index)
                }
            }

            Component.onCompleted: buttonRow.updateWidestButtonWidth()
        } // buttonRow

        // This is the item that will be the parent of the currently displayed page.
        Item {
            id: pages
            anchors {
                top: buttonRow.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
        }

        function selectTab(index) {
            if (tabsMain.selectedTabIndex != -1) tabItems.children[tabsMain.selectedTabIndex].selected = false;
            if (index != -1) {
                var tab = tabItems.children[index];
                tab.__setPageParent(pages);
                tab.selected = true;
            }
            tabsMain.selectedTabIndex = index;
        }

        function separatorChanged() {
            if (tabsMain.separator) {
                tabsMain.separator.parent = tabVisuals;
                tabsMain.separator.anchors.top = tabVisuals.buttonRow.bottom;
                tabsMain.separator.anchors.left = tabVisuals.left;
                tabsMain.separator.anchors.right = tabVisuals.right;
                tabVisuals.pages.anchors.top = tabsMain.separator.bottom;
            } else { // no separator
                tabVisuals.pages.anchors.top = tabVisuals.buttonRow.bottom;
            }
        }

        Connections {
            target: tabsMain
            onSelectedTabIndexChanged: if (Component.status == Component.Ready) tabVisuals.selectTab(tabsMain.selectedTabIndex)
            onSeparatorChanged: tabVisuals.separatorChanged()
        }
    } // tabVisuals

    Component.onCompleted: {
        tabVisuals.separatorChanged();
        tabVisuals.selectTab(tabsMain.selectedTabIndex);
    }
}
