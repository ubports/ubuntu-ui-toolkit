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
            onClicked: print("whee")
        }
        divider.visible: false
    }

    defaultNumberOfSlots: 3

    Item {
        id: listViewContainer
        anchors.fill: parent
//        clip: true

        property real listViewMargins: units.gu(2)

        property var visibleActions: getReversedVisibleActions(styledItem.actions)
        function getReversedVisibleActions(actions) {
            var visibleActionList = [];
//            for (var i = actions.length - 1; i >= 0 ; i--) {
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
            //            width: Math.min(listViewContainer.width - 2*listViewContainer.listViewMargins,
            //                            contentWidth)
            width: listViewContainer.width //Math.min(listViewContainer.width, contentWidth)
//            Component.onCompleted: positionViewAtEnd()
            layoutDirection: Qt.RightToLeft

            // FIXME TIM: No need for margins because the button is not visible when scrolled to the edge.
//            leftMargin: units.gu(4)
//            rightMargin: units.gu(4)

            clip: true
            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds
            delegate: styledItem.delegate
            model: listViewContainer.visibleActions

            onCurrentIndexChanged: print("current index = "+currentIndex)
onContentXChanged: print("contentX = "+contentX+", origniX = "+originX  )
            SmoothedAnimation {
                objectName: "actions_scroll_animation"
                id: contentXAnim
                target: actionsListView
                property: "contentX"
//                duration: UbuntuAnimation.FastDuration
                velocity: units.gu(20)
                onRunningChanged: print("running = "+running)
            }

//            function scrollToIndex(index) {
//                contentXAnim.running = false;
//                var oldPos = actionsListView.contentX;
//                var newPos;
//                // FIXME: TIM probably pass the 2nd parameter too
//                actionsListView.positionViewAtIndex(index, ListView.Beginning);
//                newPos = actionsListView.contentX;
//                contentXAnim.from = oldPos;
//                contentXAnim.to = newPos;
//                print("oldPos = "+oldPos+", newPos = "+newPos)
//                contentXAnim.running = true;
//            }

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
//                            0.0, // estimation of originX is some times wrong when scrolling towards the beginning
                            originX,
                            actionsListView.originX + actionsListView.contentWidth - actionsListView.width);
                print("newContentX = "+newContentX+", to = "+contentXAnim.to);
                contentXAnim.start();
//                positionViewAtBeginning();
//                scrollToIndex(0);
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
//                color: "yellow"
//                opacity: 0.5
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
