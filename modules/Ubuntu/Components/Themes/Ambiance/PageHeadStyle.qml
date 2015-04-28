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
import QtQuick 2.4
import Ubuntu.Components 1.2
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Styles 1.2 as Style

Style.PageHeadStyle {
    id: headerStyle
    objectName: "PageHeadStyle" // used in unit tests
    contentHeight: units.gu(7)
    fontWeight: Font.Light
    fontSize: "x-large"
    textLeftMargin: units.gu(2)
    maximumNumberOfActions: 3

    /*!
      The color of the buttons in the header.
     */
    property color buttonColor: styledItem.config.foregroundColor

    /*!
      The color of the title text.
     */
    property color titleColor: styledItem.config.foregroundColor

    /*!
      The background color of the tabs panel and the actions overflow panel.
     */
    property color panelBackgroundColor: styledItem.panelColor

    /*!
      The background color of the tapped item in the panel.
     */
    property color panelHighlightColor: Theme.palette.selected.background

    /*!
      The foreground color (icon and text) of actions in the panel.
     */
    property color panelForegroundColor: Theme.palette.selected.backgroundText

    /*!
      The text color of unselected sections and the section divider.
     */
    property color sectionColor: Theme.palette.selected.backgroundText

    /*!
      The text color of the selected section.
     */
    property color selectedSectionColor: UbuntuColors.orange

    /*!
      The background color of the pressed section.
     */
    property color sectionHighlightColor: Theme.palette.selected.background

    implicitHeight: headerStyle.contentHeight + divider.height

    // FIXME: Workaround to get sectionsRepeater.count in autopilot tests,
    //  see also FIXME in AppHeader where this property is used.
    property alias __sections_repeater_for_autopilot: sectionsRepeater

    // Used by unit tests and autopilot tests to wait for animations to finish
    readonly property bool animating: headerStyle.state == "OUT"
                                      || leftAnchor.anchors.leftMargin < 0

    // for Unity8
    // FIXME: Remove this property when we introduce a header preset that does not
    //  have a separator.
    property alias __separator_visible: divider.visible

    StyledItem {
        id: divider
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        height: sectionsRow.visible ? units.gu(3) : units.gu(2)

        // separatorSource and separatorBottomSource are needed for the deprecated
        // HeadSeparatorImageStyle.
        property url separatorSource: headerStyle.separatorSource
        property url separatorBottomSource: headerStyle.separatorBottomSource

        // backgroundColor is used in the new HeadDividerStyle
        property color backgroundColor: styledItem.dividerColor

        style: Theme.createStyleComponent("HeadDividerStyle.qml", divider)

        property PageHeadSections sections: styledItem.config.sections

        Row {
            id: sectionsRow
            anchors.centerIn: parent
            width: childrenRect.width
            height: parent.height
            enabled: divider.sections.enabled
            visible: divider.sections.model !== undefined
            opacity: enabled ? 1.0 : 0.5

            Repeater {
                id: sectionsRepeater
                model: divider.sections.model
                objectName: "page_head_sections_repeater"
                AbstractButton {
                    id: sectionButton
                    anchors.verticalCenter: parent ? parent.verticalCenter : undefined
                    objectName: "section_button_" + index
                    enabled: sectionsRow.enabled
                    width: label.width + units.gu(4)
                    height: sectionsRow.height + units.gu(2)
                    property bool selected: index === divider.sections.selectedIndex
                    onClicked: divider.sections.selectedIndex = index;

                    Rectangle {
                        visible: parent.pressed
                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                            right: parent.right
                            rightMargin: verticalDividerLine.width
                        }
                        height: sectionsRow.height
                        color: headerStyle.sectionHighlightColor
                    }

                    Label {
                        id: label
                        text: modelData
                        fontSize: "small"
                        anchors.centerIn: sectionButton
                        horizontalAlignment: Text.AlignHCenter
                        color: sectionButton.selected ?
                                   headerStyle.selectedSectionColor :
                                   headerStyle.sectionColor
                    }

                    // vertical divider line
                    Rectangle {
                        id: verticalDividerLine
                        anchors {
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                        }
                        height: units.dp(10)
                        width: units.dp(1)
                        visible: index < sectionsRepeater.model.length - 1
                        color: headerStyle.sectionColor
                        opacity: 0.2
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "IN"
            PropertyChanges {
                target: allContents
                opacity: 1.0
            }
        },
        State {
            name: "OUT"
            PropertyChanges {
                target: allContents
                opacity: 0.0
            }
        }
    ]

    function animateOut() {
        state = "OUT";
    }
    function animateIn() {
        state = "IN";
    }

    signal animateOutFinished()
    signal animateInFinished()

    transitions: [
        Transition {
            id: transitionOut
            from: "IN"
            to: "OUT"
            SequentialAnimation {
                ParallelAnimation {
                    UbuntuNumberAnimation {
                        target: allContents
                        property: "opacity"
                        from: 1.0
                        to: 0.0
                    }
                    UbuntuNumberAnimation {
                        target: leftAnchor
                        properties: "anchors.leftMargin"
                        from: 0.0
                        to: -units.gu(5)
                    }
                    UbuntuNumberAnimation {
                        target: rightAnchor
                        properties: "anchors.rightMargin"
                        from: 0
                        to: -units.gu(5)
                    }
                }
                ScriptAction {
                    script: headerStyle.animateOutFinished()
                }
            }
        },
        Transition {
            id: transitionIn
            from: "OUT"
            to: "IN"
            SequentialAnimation {
                ParallelAnimation {
                    UbuntuNumberAnimation {
                        target: allContents
                        property: "opacity"
                        from: 0.0
                        to: 1.0
                    }
                    UbuntuNumberAnimation {
                        target: leftAnchor
                        properties: "anchors.leftMargin"
                        from: -units.gu(5)
                        to: 0
                    }
                    UbuntuNumberAnimation {
                        target: rightAnchor
                        properties: "anchors.rightMargin"
                        from: -units.gu(5)
                        to: 0
                    }
                }
                ScriptAction {
                    script: headerStyle.animateInFinished()
                }
            }
        }
    ]

    Item {
        id: allContents
        anchors.fill: parent

        Item {
            id: leftAnchor
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                leftMargin: 0
            }
            width: 0
        }
        Item {
            id: rightAnchor
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
                rightMargin: 0
            }
            width: 0
        }

        Item {
            id: leftButtonContainer
            anchors {
                left: leftAnchor.right
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
                color: styledItem.config.foregroundColor
            }

            PageHeadButton {
                id: backButton
                objectName: "backButton"

                iconName: "back"
                visible: styledItem.pageStack !== null &&
                         styledItem.pageStack !== undefined &&
                         styledItem.pageStack.depth > 1 &&
                         !styledItem.config.backAction

                text: "back"
                color: styledItem.config.foregroundColor

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
                color: headerStyle.buttonColor

                onTriggered: PopupUtils.open(tabsPopoverComponent, tabsButton)

                Component {
                    id: tabsPopoverComponent

                    OverflowPanel {
                        id: tabsPopover
                        objectName: "tabsPopover"
                        callerMargin: -units.gu(1) + units.dp(4)
                        contentWidth: units.gu(20)

                        Binding {
                            target: tabsPopover.__foreground.__styleInstance
                            property: "color"
                            value: headerStyle.panelBackgroundColor
                            when: tabsPopover.__foreground &&
                                  tabsPopover.__foreground.__styleInstance
                        }

                        Column {
                            anchors {
                                left: parent.left
                                top: parent.top
                                right: parent.right
                            }
                            Repeater {
                                model: styledItem.tabsModel
                                AbstractButton {
                                    objectName: "tabButton" + index
                                    onClicked: {
                                        styledItem.tabsModel.selectedIndex = index;
                                        tabsPopover.hide();
                                    }
                                    implicitHeight: units.gu(6) + bottomDividerLine.height
                                    width: parent ? parent.width : units.gu(31)

                                    Rectangle {
                                        visible: parent.pressed
                                        anchors {
                                            left: parent.left
                                            right: parent.right
                                            top: parent.top
                                        }
                                        height: parent.height - bottomDividerLine.height
                                        color: headerStyle.panelHighlightColor
                                    }

                                    Label {
                                        anchors {
                                            verticalCenter: parent.verticalCenter
                                            verticalCenterOffset: units.dp(-1)
                                            left: parent.left
                                            leftMargin: units.gu(2)
                                            right: parent.right
                                        }
                                        fontSize: "medium"
                                        elide: Text.ElideRight
                                        text: tab.title // FIXME: only "title" doesn't work with i18n.tr(). Why not?
                                        color: headerStyle.panelForegroundColor
                                    }

                                    ListItem.ThinDivider {
                                        id: bottomDividerLine
                                        anchors.bottom: parent.bottom
                                        visible: index < styledItem.tabsModel.count - 1
                                    }
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
                top: parent.top
                // don't keep a margin if there is already a button with spacing
                leftMargin: leftButtonContainer.width > 0 ? 0 : headerStyle.textLeftMargin
            }
            width: parent.width - anchors.leftMargin
                   - leftButtonContainer.anchors.leftMargin - leftButtonContainer.width
                   - actionsContainer.anchors.rightMargin - actionsContainer.width
            height: headerStyle.contentHeight

            Label {
                objectName: "header_title_label"
                LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft
                visible: !contentsContainer.visible && styledItem.config.preset === ""
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                text: styledItem.title
                font.weight: headerStyle.fontWeight
                fontSize: headerStyle.fontSize
                color: headerStyle.titleColor
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
                    if (action && action.hasOwnProperty("visible") && action.visible) {
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
                right: rightAnchor.left
                rightMargin: actionsContainer.width > 0 ? units.gu(1) : 0
            }
            width: childrenRect.width
            height: headerStyle.contentHeight

            Repeater {
                model: numberOfSlots.used
                PageHeadButton {
                    id: actionButton
                    objectName: action.objectName + "_header_button"
                    action: actionsContainer.visibleActions[index]
                    color: headerStyle.buttonColor
                    state: styledItem.config.preset === "select" ?
                               "IconAndLabel" : ""
                }
            }

            PageHeadButton {
                id: actionsOverflowButton
                objectName: "actions_overflow_button"
                visible: numberOfSlots.requested > numberOfSlots.right
                // Ensure resetting of X when this button is not visible to avoid
                // miscalculation of actionsContainer.width. Fixes bug #1408481.
                onVisibleChanged: if (!visible) x = 0
                iconName: "contextual-menu"
                color: headerStyle.buttonColor
                height: actionsContainer.height
                onTriggered: PopupUtils.open(actionsOverflowPopoverComponent, actionsOverflowButton)

                Component {
                    id: actionsOverflowPopoverComponent

                    OverflowPanel {
                        id: actionsOverflowPopover
                        objectName: "actions_overflow_popover"
                        callerMargin: -units.gu(1) + units.dp(4)
                        contentWidth: units.gu(20)

                        Binding {
                            target: actionsOverflowPopover.__foreground.__styleInstance
                            property: "color"
                            value: headerStyle.panelBackgroundColor
                            when: actionsOverflowPopover.__foreground &&
                                  actionsOverflowPopover.__foreground.__styleInstance
                        }

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
                                id: overflowRepeater
                                model: numberOfSlots.requested - numberOfSlots.used
                                AbstractButton {
                                    action: actionsContainer.visibleActions[numberOfSlots.used + index]
                                    objectName: action.objectName + "_header_overflow_button"
                                    onClicked: actionsOverflowPopover.hide()
                                    implicitHeight: units.gu(6) + bottomDividerLine.height
                                    width: parent ? parent.width : units.gu(31)

                                    Rectangle {
                                        visible: parent.pressed
                                        anchors {
                                            left: parent.left
                                            right: parent.right
                                            top: parent.top
                                        }
                                        height: parent.height - bottomDividerLine.height
                                        color: headerStyle.panelHighlightColor
                                    }

                                    Icon {
                                        id: actionIcon
                                        source: action.iconSource
                                        color: headerStyle.panelForegroundColor
                                        anchors {
                                            verticalCenter: parent.verticalCenter
                                            verticalCenterOffset: units.dp(-1)
                                            left: parent.left
                                            leftMargin: units.gu(2)
                                        }
                                        width: units.gu(2)
                                        height: units.gu(2)
                                        opacity: action.enabled ? 1.0 : 0.5
                                    }

                                    Label {
                                        anchors {
                                            verticalCenter: parent.verticalCenter
                                            verticalCenterOffset: units.dp(-1)
                                            left: actionIcon.right
                                            leftMargin: units.gu(2)
                                            right: parent.right
                                        }
                                        fontSize: "small"
                                        elide: Text.ElideRight
                                        text: action.text
                                        color: headerStyle.panelForegroundColor
                                        opacity: action.enabled ? 1.0 : 0.5
                                    }

                                    ListItem.ThinDivider {
                                        id: bottomDividerLine
                                        anchors.bottom: parent.bottom
                                        visible: index !== overflowRepeater.count - 1
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
