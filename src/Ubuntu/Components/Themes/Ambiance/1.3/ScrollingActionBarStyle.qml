/*
 * Copyright 2016 Canonical Ltd.
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
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.Styles 1.3 as Style
import QtGraphicalEffects 1.0

Style.ActionBarStyle {
    id: actionBarStyle
    implicitHeight: units.gu(5)
    //        implicitWidth: units.gu(4) * styledItem.numberOfItems
    implicitWidth: units.gu(36) // 9 * defaultDelegate.width

    overflowIconName: "contextual-menu"

    // Unused with the standard action icon buttons, but may be used with a custom delegate.
    overflowText: "More"

    /*!
      The default action delegate if the styled item does
      not provide a delegate.
     */
    // FIXME: This ListItem { AbstractButton { } } construction can be avoided
    //  when StyledItem supports cursor keys navigation, see bug #1573616
    defaultDelegate: ListItem {
        width: units.gu(4)
        height: actionBarStyle.height
        onClicked: button.trigger()
        AbstractButton {
            id: button
            anchors.fill: parent
            style: IconButtonStyle { }
            objectName: action.objectName + "_button"
            height: parent ? parent.height : undefined
            action: modelData
                    activeFocusOnTab: false
        }
        divider.visible: false
    }

    // FIXME TIM: remove this.
    defaultNumberOfSlots: 3

    property color backgroundColor: "white"

//    Item {
    Rectangle {
        color: actionBarStyle.backgroundColor
        id: listViewContainer
        anchors.fill: parent
//        clip: true

        property real listViewMargins: units.gu(2)

        property var visibleActions: getVisibleActions(styledItem.actions)
        function getVisibleActions(actions) {
            var visibleActionList = [];
            for (var i = 0; i < actions.length; i++) {
                var action = actions[i];
                if (action && action.hasOwnProperty("visible") && action.visible) {
                    visibleActionList.push(action);
                }
            }
            return visibleActionList;
        }

        ListView {
            id: actionsListView
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            width: listViewContainer.width //Math.min(listViewContainer.width, contentWidth)
            layoutDirection: Qt.RightToLeft

            clip: true
            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds
            delegate: styledItem.delegate
            model: listViewContainer.visibleActions

            onCurrentIndexChanged: print("current index = "+currentIndex)
            SmoothedAnimation {
                objectName: "actions_scroll_animation"
                id: contentXAnim
                target: actionsListView
                property: "contentX"
//                duration: UbuntuAnimation.FastDuration
                velocity: units.gu(20)
                onRunningChanged: print("running = "+running)
            }

            // direction == 1 to show more icons on the left
            // direction == -1 to show more icons on the right
            function scroll(direction) {
                if (contentXAnim.running) contentXAnim.stop();
                print("direction = "+direction)
                var newContentX = actionsListView.contentX - (actionsListView.width * direction);
                contentXAnim.from = actionsListView.contentX;
                // make sure we don't overshoot bounds
                contentXAnim.to = MathUtils.clamp(
                            newContentX,
                            originX,
                            actionsListView.originX + actionsListView.contentWidth - actionsListView.width);
                contentXAnim.start();
            }
        }
        AbstractButton {
            id: leftButton
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            width: units.gu(4)
            enabled: opacity === 1.0
            onClicked: actionsListView.scroll(1);
            opacity: actionsListView.atXBeginning ? 0.0 : 1.0
            Behavior on opacity {
                UbuntuNumberAnimation {
                    duration: 1000//UbuntuAnimation.FastDuration
                }
            }
            Rectangle {
                // FIXME TIM: Use the background color of the actionbar.
                anchors.fill: parent
                color: "white"
            }
            Icon {
                id: leftIcon
                objectName: "left_scroll_icon"
                anchors.centerIn: parent
                width: units.gu(1)
                height: units.gu(1)
                rotation: 180
                name: "chevron"
            }
        }
        AbstractButton {
            id: rightButton
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            width: units.gu(4)
            enabled: opacity === 1.0
            onClicked: actionsListView.scroll(-1);
            opacity: actionsListView.atXEnd ? 0.0 : 1.0
            Behavior on opacity {
                UbuntuNumberAnimation {
                    duration: 1000//UbuntuAnimation.FastDuration
                }
            }
            Rectangle {
                anchors.fill: parent
                color: "white"
            }
            Icon {
                id: rightIcon
                objectName: "right_scroll_icon"
                anchors.centerIn: parent
                width: units.gu(1)
                height: units.gu(1)
                name: "chevron"
            }
        }
    }
}
