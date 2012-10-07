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

/*!
    \qmlclass Tabs
    \inqmlmodule Ubuntu.Components 0.1
    \brief The Tabs class provides an environment where multible Tab
    children can be added, and the user is presented with a tab
    bar with tab buttons to select different tab pages.

    Examples:
    \qml
        Tabs {
            Tab {
                title: "tab 1"
                page: Text {
                    text: "This is the first tab."
                }
            }
            Tab {
                title: "tab 2"
                iconSource: "icon.png"
                page: Rectangle {
                    Text {
                        anchors.centerIn: parent
                        text: "Colorful tab."
                    }
                    color: "lightblue"
                }
            }
            Tab {
                title: "tab 3"
                page: Qt.resolvedUrl("MyCustomPage.qml")
            }
        }
    \endqml

    \b{This component is under heavy development.}
*/

Item {
    id: tabs

    /*!
      \internal
      The children of the Tabs should be instances of Tab.
     */
    default property alias children: contentsContainer.children

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
      as a separator. By default, it is a 1-pixel height white rectangle.
     */
    property Item separator: Rectangle {
            color: "white"
            height: units.dp(1)
    }

    // minimal width of 200
    width: units.dp(200)
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
            height: units.dp(28)
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }

            /*!
              \preliminary
              The padding on the left and right side of the row of buttons.
            */
            property real horizontalPadding: units.dp(10)

            // maximumButtonWidth is the total space available for all tab buttons
            // divided by the number of buttons.
            // If minimumButtonWidth is larger than maximumButtonWidth, the buttonWidth
            // will be minimumButtonWidth. Otherwise, the buttonWidth is the width the
            // largest button needs to fit all its contents.
            // Scrolling in case the buttons don't fit in the available space is currently
            // not implemented.
            property int minimumButtonWidth: units.dp(20)
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
                }
                buttonRow.widestButtonWidth = widest;
            }

            Component.onCompleted: {
                connectToButtonChanges()
                updateWidestButtonWidth()
            }

            function connectToButtonChanges() {
                var button;
                for (var i=0; i < buttonRow.children.length; i++) {
                    button = buttonRow.children[i];
                    button.implicitWidthChanged.disconnect(buttonRow.updateWidestButtonWidth);
                    button.implicitWidthChanged.connect(buttonRow.updateWidestButtonWidth);
                }
            }

            Repeater {
                id: repeater
                onModelChanged: {
                    buttonRow.connectToButtonChanges()
                    buttonRow.updateWidestButtonWidth()
                }
                onCountChanged: {
                    buttonRow.connectToButtonChanges()
                    buttonRow.updateWidestButtonWidth()
                }

                model: tabs.children

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
        }

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
            var tab;
            for (var i = 0; i < tabs.children.length; i++) {
                tab = tabs.children[i];
                if (i == tabs.selectedTabIndex) {
                    tab.__active = true;
                } else {
                    tab.__active = false;
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
            } else {
                // no separator
                visuals.contentsContainer.anchors.top = visuals.buttonRow.bottom;
            }
        }

        Connections {
            target: tabs
            onSelectedTabIndexChanged: visuals.selectedTabChanged()
            onSeparatorChanged: visuals.separatorChanged()
        }
    }

    Component.onCompleted: {
        visuals.separatorChanged();
        visuals.selectedTabChanged();
    }
}
