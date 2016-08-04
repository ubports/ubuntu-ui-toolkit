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

    // FIXME: These 3 properties are not used in this style.
//    overflowIconName: "contextual-menu" // FIXME: unused
//    overflowText: "More" // FIXME: unused
//    defaultNumberOfSlots: 3

    backgroundColor: theme.palette.normal.background
    buttons {
        foregroundColor: theme.palette.normal.backgroundText
        pressedForegroundColor: buttons.foregroundColor
        disabledForegroundColor: theme.palette.disabled.backgroundText
        backgroundColor: "transparent" // action bar background is already colored
        pressedBackgroundColor: theme.palette.highlighted.background
        disabledBackgroundColor: buttons.backgroundColor
    }

    property QtObject scrollButtons: Style.ActionItemProperties {
        foregroundColor: actionBarStyle.buttons.foregroundColor
        pressedForegroundColor: actionBarStyle.buttons.pressedForegroundColor
        backgroundColor: actionBarStyle.backgroundColor // must be opaque to hide the icon buttons
        pressedBackgroundColor: actionBarStyle.buttons.pressedBackgroundColor
    }

    /*!
      The default action delegate if the styled item does
      not provide a delegate.
     */
    // FIXME: This ListItem { AbstractButton { } } construction can be avoided
    //  when StyledItem supports cursor keys navigation, see bug #1573616
    defaultDelegate: ListItem {
        width: units.gu(4)
        height: actionBarStyle.height
        enabled: modelData.enabled
        onClicked: button.trigger()
        AbstractButton {
            id: button
            anchors.fill: parent
            style: IconButtonStyle {
                foregroundColor: button.pressed ?
                                     actionBarStyle.buttons.pressedForegroundColor :
                                     button.enabled ?
                                         actionBarStyle.buttons.foregroundColor :
                                         actionBarStyle.buttons.disabledForegroundColor
                backgroundColor: button.pressed ?
                                     actionBarStyle.buttons.pressedBackgroundColor :
                                     button.enabled ?
                                         actionBarStyle.buttons.backgroundColor :
                                         actionBarStyle.buttons.disabledBackgroundColor
            }
            objectName: action.objectName + "_button"
            height: parent ? parent.height : undefined
            action: modelData
            activeFocusOnTab: false
        }
        divider.visible: false
    }

    // The duration of the fade-in/out of the scroll buttons.
    property int scrollButtonFadeDuration: UbuntuAnimation.FastDuration

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
            objectName: "actions_listview"
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

//            onCurrentIndexChanged: print("current index = "+currentIndex)
            SmoothedAnimation {
                objectName: "actions_scroll_animation"
                id: contentXAnim
                target: actionsListView
                property: "contentX"
                duration: UbuntuAnimation.FastDuration
//                velocity: units.gu(20)
//                onRunningChanged: print("running = "+running)
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

        Component {
            id: scrollButtonComponent
            AbstractButton {
                id: scrollButton
                width: units.gu(4)
                enabled: opacity === 1.0
                onClicked: actionsListView.scroll(scrollDirection);
                opacity: buttonOpacity
                objectName: buttonName
                Behavior on opacity {
                    UbuntuNumberAnimation {
                        duration: actionBarStyle.scrollButtonFadeDuration
                    }
                }
                Rectangle {
                    anchors.fill: parent
                    color: scrollButton.pressed ? actionBarStyle.scrollButtons.pressedBackgroundColor
                                                : actionBarStyle.scrollButtons.backgroundColor
                }
                Icon {
                    // FIXME TIM: Use new theme icon from
                    //  https://code.launchpad.net/~tiheum/ubuntu-themes/toolkit-arrows/+merge/298609
                    //  after it lands in the theme.
                    anchors.centerIn: parent
                    width: units.gu(1)
                    height: units.gu(1)
                    rotation: iconRotation
                    name: "chevron"
                    color: scrollButton.pressed ? actionBarStyle.scrollButtons.pressedForegroundColor
                                                : actionBarStyle.scrollButtons.foregroundColor
                }
            }
        }
        Loader {
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            sourceComponent: scrollButtonComponent
            property int iconRotation: 180
            property real buttonOpacity: actionsListView.atXBeginning ? 0.0 : 1.0
            property int scrollDirection: 1
            property string buttonName: "leading_scroll_button"
        }
        Loader {
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            sourceComponent: scrollButtonComponent
            property int iconRotation: 0
            property real buttonOpacity: actionsListView.atXEnd ? 0.0 : 1.0
            property int scrollDirection: -1
            property string buttonName: "trailing_scroll_button"
        }
    }
}
