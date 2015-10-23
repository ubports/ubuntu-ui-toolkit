/*
 * Copyright (C) 2015 Canonical Ltd.
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
import Ubuntu.Test 1.0

Item {
    // Wrap the root Item to work around bug #1504755 which
    //  causes the OverflowPanel to open behind the PageHeader
    //  without this wrapper Item.
    id: wrapper
    width: units.gu(50)
    height: units.gu(70)

    Item {
        anchors.fill: parent
        id: root

        property real initialHeaderHeight: units.gu(6)

        property list<Action> sectionActions: [
            Action { text: "first" },
            Action { text: "second" },
            Action { text: "third" }
        ]

        Rectangle {
            id: alternativeContents
            visible: header.contents === alternativeContents
            objectName: "alternative_contents"

            anchors.fill: parent
            color: UbuntuColors.red

            Label {
                anchors.centerIn: parent
                text: "Custom header contents"
                color: "white"
            }
        }

        property list<Action> actionList: [
            Action {
                iconName: "settings"
                text: "first"
                onTriggered: print("Trigger first action")
                objectName: "action1"
            },
            Action {
                iconName: "info"
                text: "second"
                onTriggered: print("Trigger second action")
            },
            Action {
                iconName: "search"
                text: "third"
                onTriggered: print("Trigger third action")
            },
            Action {
                iconName: "appointment"
                text: "fourth"
                onTriggered: print("Trigger fourth action")
            }
        ]

        PageHeader {
            id: header
            flickable: flickable
            z:1

            title: "Default title"
            contents: contentsSwitch.checked ? alternativeContents : null
            sections.actions: sectionsSwitch.checked ? root.sectionActions : []
            trailingActionBar.actions: trailingActionsSwitch.checked ?
                                           root.actionList : []
            navigationActions: leadingActionsSwitch.checked ?
                                          root.actionList : []
        }

        Flickable {
            id: flickable
            anchors {
                top: header.flickable ? parent.top : header.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            contentHeight: root.height * 2

            Grid {
                id: switchGrid
                columns: 2
                spacing: units.gu(1)
                anchors {
                    top: parent.top
                    left: parent.left
                    leftMargin: units.gu(5)
                    topMargin: root.initialHeaderHeight
                }

                Switch {
                    id: lockedSwitch
                    checked: null === header.flickable
                    function trigger() {
                        if (header.flickable) {
                            header.flickable = null;
                        } else {
                            header.flickable = flickable;
                        }
                    }
                }
                Label {
                    text: "header locked"
                }

                Switch {
                    id: hiddenSwitch
                    checked: header.exposed
                    function trigger() {
                        header.exposed = !header.exposed;
                    }
                }
                Label {
                    text: "header exposed"
                }

                Switch {
                    id: leadingActionsSwitch
                    checked: false
                }
                Label {
                    text: "leading actions"
                }

                Switch {
                    id: trailingActionsSwitch
                    checked: true
                }
                Label {
                    text: "trailing actions"
                }

                Switch {
                    id: contentsSwitch
                    checked: false
                }
                Label {
                    text: "replace title by contents"
                }

                Switch {
                    id: sectionsSwitch
                    checked: false
                }
                Label {
                    text: "show sections"
                }
            }

            PageHeader {
                id: defaultHeader
                visible: false
            }
        }

        UbuntuTestCase {
            name: "PageHeader"
            when: windowShown
            id: testCase

            property var style;
            function initTestCase() {
                style = header.__styleInstance;
            }

            function scroll(dy) {
                var p = centerOf(flickable);
                // Use mouseWheel to scroll because mouseDrag is very unreliable
                // and does not properly handle negative values for dy.
                mouseWheel(flickable, p.x, p.y, 0, dy);
            }

            function scroll_down() {
                scroll(-2.0*header.height);
            }

            function scroll_up() {
                scroll(header.height);
            }

            function wait_for_exposed(exposed, errorMessage) {
                tryCompare(header, "exposed", exposed, 5000, errorMessage);
                // wait for the animation to finish:
                tryCompare(header, "moving", false, 5000, "Header still moving?");
                if (exposed) {
                    compare(header.y, 0, errorMessage +
                            " y-value/exposed mismatch for exposed header!");
                } else {
                    compare(header.y, -header.height, errorMessage +
                            " y-value/exposed mismatch for hidden header!");
                }
            }

            // Use this function to prevent copying the color by reference.
            function color_by_value(color) {
                return Qt.rgba(color.r, color.g, color.b, color.a);
            }

            function test_height() {
                var divider = findChild(style, "header_divider");
                compare(header.height, style.contentHeight + divider.height,
                        "Incorrect initial header height.");
                var initialHeight = header.height;

                var sections = header.sections;
                compare(header.sections.height, 0,
                        "Empty sections has non-0 height.");

                sections.actions = root.sectionActions;
                compare(sections.height > 0, true,
                        "Sections with actions has non-positive height.");
                compare(header.height, style.contentHeight + divider.height + sections.height,
                        "Header with sections has incorrect total height.");

                sections.actions = [];
                compare(header.height, initialHeight,
                        "Unsetting sections does not revert the header height.");
            }

            function test_background_color() {
                var background = findChild(style, "header_background");
                compare(background.color, style.backgroundColor,
                        "Incorrect initial background color.");

                var initialColor = color_by_value(style.backgroundColor);
                var otherColor = "#CCFDAA"; // a random color.
                style.backgroundColor = otherColor;
                compare(Qt.colorEqual(background.color, otherColor), true,
                        "Updating style background color does not change the background color.");

                style.backgroundColor = initialColor;
                compare(Qt.colorEqual(background.color, initialColor), true,
                        "Reverting the background color failed.");
            }

            function test_foreground_color() {
                var color1 = color_by_value(style.foregroundColor);
                var bar = header.trailingActionBar;
                var iconButton = findChild(bar, "action1_button");
                var buttonStyle = iconButton.__styleInstance;
                compare(Qt.colorEqual(buttonStyle.foregroundColor, color1), true,
                        "Button foreground color does not match header foreground color.");

                var label = findChild(header, "header_title_label");
                compare(Qt.colorEqual(label.color, color1), true,
                        "Title color does not match header foreground color.");

                var color2 = "#FF1ABC"; // a random color.
                style.foregroundColor = color2;
                compare(Qt.colorEqual(buttonStyle.foregroundColor, color2), true,
                        "Button foreground color does not match updated header foreground color.");
                compare(Qt.colorEqual(label.color, color2), true,
                        "Title color does not match updated header foreground color.");

                style.foregroundColor = color1;

                // revert to the original color.
                compare(Qt.colorEqual(buttonStyle.foregroundColor, color1), true,
                        "Button foreground color does not match reverted header foreground color.");
                compare(Qt.colorEqual(label.color, color1), true,
                        "Title color does not match reverted header foreground color.");
            }

            function test_divider_color() {
                var color1 = color_by_value(style.dividerColor);
                var divider = findChild(style, "header_divider");
                compare(Qt.colorEqual(divider.color, color1), true,
                        "Incorrect divider color.");

                var color2 = "#ACDC12"; // a random color.
                style.dividerColor = color2;
                compare(Qt.colorEqual(divider.color, color2), true,
                        "Incorrect updated divider color.");

                style.dividerColor = color1;
                compare(Qt.colorEqual(divider.color, color1), true,
                        "Incorrect reverted divider color.");
            }

            function test_title() {
                compare(defaultHeader.title, "", "Header has a title by default.");
                var oldTitle = header.title;
                var titleLabel = findChild(header, "header_title_label");
                compare(titleLabel.text, oldTitle, "Incorrect title text.");

                var newTitle = "Updated title text";
                header.title = newTitle;
                compare(titleLabel.text, newTitle, "Incorrect updated title text.");

                header.title = oldTitle;
                compare(titleLabel.text, oldTitle, "Incorrect reverted title text.");
            }

            function test_contents() {
                compare(defaultHeader.contents, null, "Default header contents is not null.");
                compare(header.contents, null, "Header has contents initially.");

                var titleLabel = findChild(header, "header_title_label");
                compare(titleLabel !== null, true, "No title component loaded.");
                compare(titleLabel.visible, true, "Title is not visible.");

                var altParent = alternativeContents.parent;
                header.contents = alternativeContents;
                titleLabel = findChild(header, "header_title_label");
                compare(titleLabel, null, "Setting contents does not unload title.");
                var headerContents = findChild(header, "alternative_contents");
                compare(headerContents, alternativeContents,
                        "New contents was not re-parented to the header.");
                compare(altParent !== headerContents.parent, true,
                        "Contents parent was not changed.");

                header.contents = null;
                titleLabel = findChild(header, "header_title_label");
                compare(titleLabel !== null, true, "No title component loaded after unsetting contents.");
                compare(titleLabel.visible, true, "Title label invisible after unsetting contents.");
                headerContents = findChild(header, "alternative_contents");
                compare(headerContents, null,
                        "Previous header contents is not removed as a child of header.");
                compare(alternativeContents.parent, altParent,
                        "Contents parent was not reverted.");
            }

            function test_navigationActions() {
                header.navigationActions = [];
                compare(header.leadingActionBar.actions, header.navigationActions,
                        "Leading action bar actions does not equal navigationActions initially.");
                header.navigationActions = root.actionList;
                compare(header.leadingActionBar.actions, header.navigationActions,
                        "Updating navigationActions does not update leading actions.");
                header.navigationActions = [];
                compare(header.leadingActionBar.actions, header.navigationActions,
                        "Reverting navigationActions does not revert leading actions.");
                header.leadingActionBar.actions = root.actionList;
                compare(header.navigationActions.length, 0,
                        "Setting leading actions changes navigationActions.");
                header.leadingActionBar.actions = [];
                compare(header.navigationActions.length, 0,
                        "Reverting leading actions changes navigationActions.");
            }

            // The properties of header.sections, header.leadingActionBar and
            //  header.trailingActionBar are tested in tst_sections.qml and tst_actionbar.qml.
        }
    }
}
