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

    implicitWidth: sectionsRow.width
    implicitHeight: units.gu(4)

    enabled: styledItem.enabled
    opacity: enabled ? 1.0 : 0.5

    /*!
      The foreground color of unselected sections.
     */
    property color sectionColor: theme.palette.selected.backgroundText

    /*!
      The foreground color of the selected section.
     */
    property color selectedSectionColor: UbuntuColors.orange

    /*!
      The background color for the pressed section button.
     */
    property color pressedBackgroundColor: theme.palette.selected.background

    /*!
      The font size for the text in the buttons.
     */
    property int textSize: Label.Small

    /*!
      The spacing on the left and right sides of the label
      inside a section button.
     */
    property real horizontalLabelSpacing: units.gu(2)

    /*!
      The height of the bar underlining the sections.
     */
    property real underlineHeight: units.dp(2)

    Row {
        id: sectionsRow
        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        width: childrenRect.width

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
                width: label.width + 2 * sectionsStyle.horizontalLabelSpacing
                height: sectionsRow.height
                property bool selected: index === styledItem.selectedIndex
                onClicked: styledItem.selectedIndex = index

                // Background pressed highlight
                Rectangle {
                    visible: parent.pressed
                    anchors.fill: parent
                    color: sectionsStyle.pressedBackgroundColor
                }

                // Section title
                Label {
                    id: label
                    objectName: "section_button_label_" + index
                    // modelData may be either a string, or an Action
                    text: modelData.hasOwnProperty("text") ? modelData.text : modelData
                    textSize: sectionsStyle.textSize
                    anchors.centerIn: parent
                    color: sectionButton.selected ?
                               sectionsStyle.selectedSectionColor :
                               sectionsStyle.sectionColor
                }

                // Section title underline
                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }
                    height: sectionsStyle.underlineHeight
                    color: sectionButton.selected ?
                               sectionsStyle.selectedSectionColor :
                               sectionsStyle.sectionColor
                }
            }
        }
    }
}
