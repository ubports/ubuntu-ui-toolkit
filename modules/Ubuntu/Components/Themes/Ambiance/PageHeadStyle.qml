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
import QtQuick 2.2
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Styles 1.1 as Style

Style.PageHeadStyle {
    id: headerStyle
    contentHeight: units.gu(7.5)
    separatorSource: "artwork/PageHeaderBaseDividerLight.sci"
    separatorBottomSource: "artwork/PageHeaderBaseDividerBottom.png"
    fontWeight: Font.Light
    fontSize: "x-large"
    textColor: Theme.palette.selected.backgroundText
    textLeftMargin: units.gu(2)
    maximumNumberOfActions: 3

    implicitHeight: headerStyle.contentHeight + separator.height + separatorBottom.height

    // FIXME: Workaround to get sectionsRepeater.count in autopilot tests,
    //  see also FIXME in AppHeader where this property is used.
    property alias __sections_repeater_for_autopilot: sectionsRepeater

    BorderImage {
        id: separator
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        source: headerStyle.separatorSource

        property PageHeadSections sections: styledItem.config.sections

        Row {
            id: sectionsRow
            anchors.centerIn: parent
            width: childrenRect.width
            height: parent.height
            enabled: separator.sections.enabled
            visible: separator.sections.model !== undefined
            opacity: enabled ? 1.0 : 0.5

            Repeater {
                id: sectionsRepeater
                model: separator.sections.model
                objectName: "page_head_sections_repeater"
                AbstractButton {
                    id: sectionButton
                    anchors.verticalCenter: parent.verticalCenter
                    objectName: "section_button_" + index
                    enabled: sectionsRow.enabled
                    width: label.width + units.gu(4)
                    height: sectionsRow.height
                    property bool selected: index === separator.sections.selectedIndex
                    onClicked: separator.sections.selectedIndex = index;

                    Label {
                        id: label
                        text: modelData
                        fontSize: "small"
                        anchors.centerIn: sectionButton
                        horizontalAlignment: Text.AlignHCenter
                        color: sectionButton.selected ?
                                   UbuntuColors.orange :
                                   UbuntuColors.darkGrey
                    }
                }
            }
        }
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
            leftMargin: width > 0 ? units.gu(1) : 0
        }
        width: childrenRect.width
        height: headerStyle.contentHeight

        PageHeadButton {
            id: customBackButton
            objectName: "customBackButton"
            action: styledItem.config.backAction
            visible: null !== styledItem.config.backAction &&
                     styledItem.config.backAction.visible
        }

        PageHeadButton {
            id: backButton
            objectName: "backButton"

            iconName: "back"
            visible: styledItem.pageStack !== null &&
                     styledItem.pageStack !== undefined &&
                     styledItem.pageStack.depth > 1 &&
                     !customBackButton.visible

            text: "back"

            onTriggered: {
                styledItem.pageStack.pop();
            }
        }

        PageHeadButton {
            id: tabsButton
            objectName: "tabsButton"

            iconName: "navigation-menu"
            visible: styledItem.tabsModel !== null &&
                     styledItem.tabsModel !== undefined &&
                     !backButton.visible &&
                     !customBackButton.visible
            text: visible ? styledItem.tabsModel.count + " tabs" : ""

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
            objectName: "header_title_label"
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
            visible: styledItem.contents || styledItem.config.contents
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
            target: styledItem.config.contents
            property: "parent"
            value: contentsContainer
            when: styledItem.config.contents && !styledItem.contents
        }
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
            rightMargin: units.gu(1)
        }
        width: childrenRect.width
        height: headerStyle.contentHeight

        Repeater {
            model: numberOfSlots.used
            PageHeadButton {
                id: actionButton
                objectName: action.objectName + "_header_button"
                action: actionsContainer.visibleActions[index]
            }
        }

        PageHeadButton {
            id: actionsOverflowButton
            objectName: "actions_overflow_button"
            visible: numberOfSlots.requested > numberOfSlots.right
            iconName: "contextual-menu"
            width: visible ? units.gu(5) : 0
            height: actionsContainer.height
            onTriggered: actionsOverflowPopover.show()

            Popover {
                id: actionsOverflowPopover
                objectName: "actions_overflow_popover"
                parent: QuickUtils.rootItem(actionsOverflowPopover)
                caller: actionsOverflowButton

                // Ensure the popover closes when actions change and
                // the list item below may be destroyed before its
                // onClicked is executed. See bug
                // https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1326963
                Connections {
                    target: styledItem.config
                    onActionsChanged: {
                        actionsOverflowPopover.hide();
                    }
                }
                Connections {
                    target: styledItem
                    onConfigChanged: {
                        actionsOverflowPopover.hide();
                    }
                }

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
