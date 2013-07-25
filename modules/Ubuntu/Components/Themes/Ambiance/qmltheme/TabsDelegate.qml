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

Item {
    id: visuals
    /*!
      Setting this property to true will cause the tab buttons to expand
      their widths equally to fill the Tabs bar.
      If the value is false, instead each button will use the width that
      is required by the largest tab button.
     */
    property bool buttonsExpanded

    /*!
      If this optional property is specified, it will be positioned
      between the bar with tab buttons, and the tab pages to act
      as a separator.
     */
    property Item separator

    /*!
      The padding on the left and right side of the row of buttons.
    */
    property real horizontalPadding

    /*!
      The minimum width of the tab buttons
     */
    property real minimumButtonWidth

    anchors.fill: parent

    property alias contentItem: tabsContainer

    Row {
        id: buttonRow
        width: buttonRow.buttonWidth * repeater.count
        height: units.gu(4)
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
        property real maximumButtonWidth: (visuals.width - 2*horizontalPadding) / repeater.count
        property bool needsScrolling: maximumButtonWidth < minimumButtonWidth
        property real widestButtonWidth
        property real buttonWidth
        buttonWidth: {
            if (buttonRow.needsScrolling) return minimumButtonWidth;
            else if (buttonsExpanded) return buttonRow.maximumButtonWidth;
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

            model: tabsContainer.children

            TabButton {
                id: tabButton
                property Item page: modelData
                text: modelData.title
                __isFirst: index === 0
                __isLast: index === (repeater.count - 1)
                iconSource: modelData.iconSource
                width: buttonRow.buttonWidth
                selected: (index === item.selectedTabIndex)
                onClicked: item.selectedTabIndex = index
            }
        }
    }

    // This is the item that will be the parent of the currently displayed page.
    Item {
        id: tabsContainer
        anchors {
            top: buttonRow.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        onChildrenChanged: visuals.selectedTabChanged()
    }

    function selectedTabChanged() {
        var tab;
        for (var i = 0; i < tabsContainer.children.length; i++) {
            tab = tabsContainer.children[i];
            if (i === item.selectedTabIndex) {
                tab.__active = true;
            } else {
                tab.__active = false;
            }
        }
    }

    function updateSeparator() {
        if (separator) {
            separator.parent = visuals;
            separator.anchors.top = buttonRow.bottom;
            separator.anchors.left = visuals.left;
            separator.anchors.right = visuals.right;
            tabsContainer.anchors.top = separator.bottom;
        } else {
            // no separator
            tabsContainer.anchors.top = buttonRow.bottom;
        }
    }

    Connections {
        target: item
        onSelectedTabIndexChanged: visuals.selectedTabChanged()
    }

    Component.onCompleted: {
        visuals.updateSeparator();
    }
}
