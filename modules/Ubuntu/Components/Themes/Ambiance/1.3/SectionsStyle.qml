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
import Ubuntu.Components 1.3

Item {
    id: sectionsStyle
    // TODO: Total width of the contents
    //    implicitWidth: actionsContainer.implicitWidth

    implicitWidth: sectionsRow.width
    implicitHeight: units.gu(4)

    // TODO TIM: Should the divider go here, or outside?
    Rectangle {
        id: divider
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: units.dp(1)
        color: "red"
    }

    Row {
        id: sectionsRow
        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        width: childrenRect.width
        //        enabled: sectionsItem.sections.enabled
        //        visible: sectionsItem.sections.model !== undefined
        opacity: enabled ? 1.0 : 0.5

        Repeater {
            id: sectionsRepeater
            model: styledItem.model
            objectName: "sections_repeater"
            AbstractButton {
                id: sectionButton
                anchors {
                    top: parent ? parent.top : undefined
                    bottom: parent ? parent.bottom : undefined
                }
                objectName: "section_button_" + index
                enabled: sectionsRow.enabled
                width: label.width + units.gu(4) // FIXME: expose spacing as style property
                height: sectionsRow.height //+ units.gu(2) // TODO TIM: check this
                property bool selected: index === styledItem.selectedIndex
                onClicked: styledItem.selectedIndex = index;

                // TODO
                //                Rectangle {
                //                    id: highlight
                //                    visible: parent.pressed
                //                    anchors {
                //                        verticalCenter: parent.verticalCenter
                //                        left: parent.left
                //                        right: parent.right
                //                        rightMargin: verticalDividerLine.width

                //                    }
                //                    height: sectionsRow.height
                //                color: headerStyle.sectionHighlightColor
                //                }

                Label {
                    id: label
                    text: modelData
                    fontSize: "small"
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    color: sectionButton.selected ?
                               "red" :
                               "blue"
                    //                               headerStyle.selectedSectionColor :
                    //                               headerStyle.sectionColor
                }

                Rectangle {
                    id: sectionLine
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }
                    height: units.dp(2) // FIXME: Expose as style property
                    color: sectionButton.selected ?
                               //                               headerStyle.selectedSectionColor :
                               //                               styledItem.dividerColor
                               "red" :
                               "blue"
                }
            }
        }
    }
}
