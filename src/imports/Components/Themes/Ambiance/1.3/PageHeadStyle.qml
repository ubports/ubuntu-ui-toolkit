/*
 * Copyright 2015 Canonical Ltd.
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
import QtQuick.Window 2.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.Styles 1.3 as Style

Style.PageHeadStyle {
    id: headerStyle
    objectName: "PageHeadStyle" // used in unit tests
    contentHeight: Screen.height > units.gu(50) ? units.gu(6) : units.gu(5)
    fontWeight: Font.Light
    textSize: Label.Large
    textLeftMargin: units.gu(2)
    maximumNumberOfActions: 3

    PageHeadConfiguration {
        id: defaultConfig
    }

    property PageHeadConfiguration config: styledItem.config ?
                                               styledItem.config :
                                               defaultConfig
    /*!
      The color of the buttons in the header.
     */
    property color buttonColor: headerStyle.config.foregroundColor

    /*!
      The color of the title text.
     */
    property color titleColor: headerStyle.config.foregroundColor

    /*!
      The background color of the header.
     */
    property color backgroundColor: styledItem.backgroundColor
    Rectangle {
        id: background
        anchors.fill: parent
        color: headerStyle.backgroundColor

        // The border is shown to warn the developer that the AppHeader
        //  is deprecated. See bug #1583636
        property bool showBorder: styledItem.hasOwnProperty("showDeprecatedWarning") &&
                                  styledItem.showDeprecatedWarning
        border {
            width: background.showBorder ? units.gu(0.5) : 0
            color: background.showBorder ? UbuntuColors.red : "transparent"
        }
    }

    // FIXME: When the three panel color properties below are removed,
    //  update unity8/Dash/PageHeader to use the new theming (currently
    //  in progress) to set these colors.
    /*!
      \deprecated
      The background color of the tabs panel and the actions overflow panel.
     */
    property color panelBackgroundColor: styledItem.panelColor

    /*!
       \deprecated
       The background color of the tapped item in the panel.
      */
    property color panelHighlightColor: theme.palette.highlighted.background

    /*!
       \deprecated
       The foreground color (icon and text) of actions in the panel.
      */
    property color panelForegroundColor: styledItem.enabled
                                            ? theme.palette.normal.backgroundText
                                            : theme.palette.disabled.backgroundText

    /*!
      The text color of unselected sections and the section divider.
     */
    property color sectionColor: styledItem.enabled
                                    ? theme.palette.normal.backgroundText
                                    : theme.palette.disabled.backgroundText

    /*!
      The text color of the selected section.
     */
    property color selectedSectionColor: styledItem.enabled
                                            ? theme.palette.selected.backgroundText
                                            : theme.palette.selectedDisabled.backgroundText

    /*!
      The background color of the pressed section.
     */
    property color sectionHighlightColor: theme.palette.highlighted.background

    implicitHeight: headerStyle.contentHeight + divider.height + sectionsItem.height

    // Used by unit tests and autopilot tests to wait for animations to finish
    readonly property bool animating: headerStyle.state == "OUT"
                                      || leftAnchor.anchors.leftMargin < 0

    // for Unity8
    // FIXME: Remove this property when we introduce a header preset that does not
    //  have a separator.
    property alias __separator_visible: divider.visible

    Rectangle {
        id: divider
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: units.dp(1)
        color: styledItem.dividerColor
    }

    SectionsForPageHead {
        id: sectionsItem
        objectName: "headerSectionsItem"
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
            bottom: divider.top
        }
        visible: model && model.length > 0
        enabled: sections.enabled
        height: visible ? implicitHeight : 0

        property PageHeadSections sections: headerStyle.config.sections
        model: sections ? sections.model : null

        onSelectedIndexChanged: {
            if (sections) {
                sections.selectedIndex = sectionsItem.selectedIndex;
            }
        }

        Binding {
            target: sectionsItem
            when: sectionsItem.sections
            property: "selectedIndex"
            value: sectionsItem.sections.selectedIndex
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
                action: headerStyle.config.backAction
                visible: null !== headerStyle.config.backAction &&
                         headerStyle.config.backAction.visible
                color: headerStyle.config.foregroundColor
            }

            PageHeadButton {
                id: backButton
                objectName: "backButton"

                iconName: "back"
                property bool stackBack: styledItem.pageStack !== null &&
                                         styledItem.pageStack !== undefined &&
                                         styledItem.pageStack.depth > 1

                // MultiColumnView adds the following properties: multiColumn, page, showBackButton.
                property bool treeBack: styledItem.hasOwnProperty("multiColumn") &&
                                        styledItem.showBackButton

                visible: !headerStyle.config.backAction && (stackBack || treeBack)

                text: "back"
                color: headerStyle.config.foregroundColor

                onTriggered: {
                    if (stackBack) {
                        styledItem.pageStack.pop();
                    } else {
                        // treeBack
                        styledItem.multiColumn.removePages(styledItem.page);
                    }
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
                        actions: actionsFromTabs(styledItem.tabsModel)

                        function getActionFromTab(tab) {
                            return tab.__protected.action;
                        }

                        function actionsFromTabs(tabsList) {
                            var result = [];
                            var tab;
                            for (var i=0; i < tabsList.count; i++) {
                                tab = tabsList.get(i).tab;
                                result.push(getActionFromTab(tab));
                            }
                            return result;
                        }
                        backgroundColor: headerStyle.panelBackgroundColor
                        foregroundColor: headerStyle.panelForegroundColor
                        highlightColor: headerStyle.panelHighlightColor
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
                visible: !contentsContainer.visible && headerStyle.config.preset === ""
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                text: headerStyle.config.title
                font.weight: headerStyle.fontWeight
                textSize: headerStyle.textSize
                color: headerStyle.titleColor
                elide: Text.ElideRight
            }

            Item {
                // This Item is used to make the custom header item invisible
                // when styledItem.contents is unset and its parent is not updated
                // when the bindings below is no longer active
                id: contentsContainer
                anchors.fill: parent
                visible: styledItem.contents || headerStyle.config.contents
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
                target: headerStyle.config.contents
                property: "parent"
                value: contentsContainer
                when: headerStyle.config.contents && !styledItem.contents
            }
        }

        ActionBar {
            id: actionsContainer
            objectName: "headerActionBar"
            anchors {
                top: parent.top
                right: rightAnchor.left
                rightMargin: actionsContainer.width > 0 ? units.gu(1) : 0
            }
            height: headerStyle.contentHeight

            actions: headerStyle.config.actions
            numberOfSlots: 3

            delegate: AbstractButton {
                style: IconButtonStyle { }
                objectName: action.objectName + "_button"
                height: parent ? parent.height : undefined
                action: modelData
                StyleHints {
                    foregroundColor: headerStyle.buttonColor
                }
            }
        }
    }
}
