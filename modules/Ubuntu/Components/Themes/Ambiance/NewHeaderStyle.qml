/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.0
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem

Item {
    id: headerStyle
    /*!
      The height of the headercontents, which is the full height of
      the header minus the separators shown at the bottom of it.
     */
    property real contentHeight: units.gu(7.5)

    /*!
      The source of the image that separates the header from the contents of a \l MainView.
      The separator will be drawn over the contents.
     */
    property url separatorSource: "artwork/PageHeaderBaseDividerLight.sci"

    /*!
      The source of an additional image attached to the bottom of the separator. The contents
      of the \l MainView will be drawn on top of the separator bottom image.
     */
    property url separatorBottomSource: "artwork/PageHeaderBaseDividerBottom.png"

    property int fontWeight: Font.Light
    property string fontSize: "x-large"
    property color textColor: Theme.palette.selected.backgroundText
    property real textLeftMargin: units.gu(2)

    /*!
      The number of slots for actions in the header, including the optional
      (custom or automatic) back button in the left side of the header.
      If the number of actions defined is larger than the numer of actions
      specified here, extra actions are put into an overflow.
     */
    property int maximumNumberOfActions: 3

    implicitHeight: headerStyle.contentHeight + separator.height + separatorBottom.height

    BorderImage {
        id: separator
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        source: headerStyle.separatorSource
    }
    Image {
        id: separatorBottom
        anchors {
            top: separator.bottom
            left: parent.left
            right: parent.right
        }
        source: headerStyle.separatorBottomSource
    }

    Item {
        id: leftButtonContainer
        anchors {
            left: parent.left
            top: parent.top
        }
        width: childrenRect.width
        height: headerStyle.contentHeight

        AbstractButton {
            id: customBackButton
            objectName: "customBackButton"
            height: parent ? parent.height : undefined
            width: visible ? units.gu(5) : 0
            action: styledItem.config.backAction
            visible: null !== styledItem.config.backAction &&
                     styledItem.config.backAction.visible
            style: Theme.createStyleComponent("HeaderButtonStyle.qml", backButton)
        }

        AbstractButton {
            id: backButton
            objectName: "backButton"
            height: parent ? parent.height : undefined
            width: visible ? units.gu(5) : 0

            iconName: "back"
            visible: styledItem.pageStack !== null &&
                     styledItem.pageStack.depth > 1 &&
                     !customBackButton.visible

            text: "back"
            style: Theme.createStyleComponent("HeaderButtonStyle.qml", backButton)

            onTriggered: {
                styledItem.pageStack.pop();
            }
        }

        AbstractButton {
            id: tabsButton
            objectName: "tabsButton"
            height: parent ? parent.height : undefined
            width: visible ? units.gu(5) : 0

            iconName: "navigation-menu"
            visible: styledItem.tabsModel !== null && !backButton.visible &&
                     !customBackButton.visible
            text: visible ? styledItem.tabsModel.count + " tabs" : ""
            style: Theme.createStyleComponent("HeaderButtonStyle.qml", tabsButton)

            onTriggered: {
                tabsPopover.show();
            }

            Popover {
                id: tabsPopover
                objectName: "tabsPopover"
                parent: QuickUtils.rootItem(tabsPopover)
                caller: tabsButton
                Column {
                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }
                    Repeater {
                        model: styledItem.tabsModel
                        ListItem.Standard {
                            visible: index !== styledItem.tabsModel.selectedIndex
                            text: tab.title // FIXME: only "title" doesn't work with i18n.tr(). Why not?
                            objectName: "tabButton" + index
                            onClicked: {
                                styledItem.tabsModel.selectedIndex = index;
                                tabsPopover.hide();
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        id: foreground
        anchors {
            left: leftButtonContainer.right
            right: actionsContainer.left
            top: parent.top
            // don't keep a margin if there is already a button with spacing
            leftMargin: leftButtonContainer.width > 0 ? 0 : headerStyle.textLeftMargin
        }
        height: headerStyle.contentHeight

        Label {
            LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
            visible: !contentsContainer.visible
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            text: styledItem.title
            font.weight: headerStyle.fontWeight
            fontSize: headerStyle.fontSize
            color: headerStyle.textColor
            elide: Text.ElideRight
        }
        Item {
            // This Item is used to make the custom header item invisible
            // when styledItem.contents is unset and its parent is not updated
            // when the bindings below is no longer active
            id: contentsContainer
            anchors.fill: parent
            visible: styledItem.contents || styledItem.config.input
        }
        Binding {
            target: styledItem.contents
            property: "anchors.fill"
            value: contentsContainer
            when: styledItem.contents
        }
        Binding {
            target: styledItem.contents
            property: "parent"
            value: contentsContainer
            when: styledItem.contents
        }
        Binding {
            target: styledItem.config.input
            property: "parent"
            value: contentsContainer
            when: styledItem.config.input && !styledItem.contents
        }
        Binding {
            target: styledItem.config.input
            property: "anchors.verticalCenter"
            value: contentsContainer.verticalCenter
            when: styledItem.config.input &&
                  styledItem.config.input.parent === contentsContainer
        }
        Binding {
            target: styledItem.config.input
            property: "anchors.left"
            value: contentsContainer.left
            when: styledItem.config.input &&
                  styledItem.config.input.parent === contentsContainer
        }
//        Binding {
//            target: styledItem.config.input
//            property: "anchors.left"
//            value: contentsContainer.left
//            when: styledItem.config.input //&& !styledItem.contents
//        }
//        Binding {
//            target: styledItem.config.input
//            property: "anchors.verticalCenter"
//            value: styledItem.config.input.parent.verticalCenter //contentsContainer.verticalCenter
//            when: styledItem.config.input !== null //&& !styledItem.contents
//        }
    }

    Row {
        id: actionsContainer

        property var visibleActions: getVisibleActions(styledItem.config.actions)
        function getVisibleActions(actions) {
            var visibleActionList = [];
            for (var i in actions) {
                var action = actions[i];
                if (action.visible) {
                    visibleActionList.push(action);
                }
            }
            return visibleActionList;
        }

        QtObject {
            id: numberOfSlots
            property int requested: actionsContainer.visibleActions.length
            property int left: tabsButton.visible || backButton.visible ||
                               customBackButton.visible ? 1 : 0
            property int right: headerStyle.maximumNumberOfActions - left
            property int overflow: actionsOverflowButton.visible ? 1 : 0
            property int used: Math.min(right - overflow, requested)
        }

        anchors {
            top: parent.top
            right: parent.right
        }
        width: childrenRect.width
        height: headerStyle.contentHeight

        Repeater {
            model: numberOfSlots.used
            AbstractButton {
                id: actionButton
                objectName: action.objectName + "_header_button"
                action: actionsContainer.visibleActions[index]
                style: Theme.createStyleComponent("HeaderButtonStyle.qml", actionButton)
                width: units.gu(5)
                height: actionsContainer.height
            }
        }

        AbstractButton {
            id: actionsOverflowButton
            objectName: "actions_overflow_button"
            visible: numberOfSlots.requested > numberOfSlots.right
            iconName: "dropdown-menu"
            width: visible ? units.gu(5) : 0
            style: Theme.createStyleComponent("HeaderButtonStyle.qml", actionsOverflowButton)
            height: actionsContainer.height
            onTriggered: actionsOverflowPopover.show()

            Popover {
                id: actionsOverflowPopover
                objectName: "actionsOverflowPopover"
                parent: QuickUtils.rootItem(actionsOverflowPopover)
                caller: actionsOverflowButton
                Column {
                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }
                    Repeater {
                        model: numberOfSlots.requested - numberOfSlots.used
                        ListItem.Standard {
                            action: actionsContainer.visibleActions[numberOfSlots.used + index]
                            objectName: action.objectName + "_header_overflow_button"
                            onClicked: actionsOverflowPopover.hide()
                        }
                    }
                }
            }
        }
    }
}
