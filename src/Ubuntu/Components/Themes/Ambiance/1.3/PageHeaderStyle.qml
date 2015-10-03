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
//import Ubuntu.Components.Styles 1.3 as Style

Item {
    id: pageHeaderStyle
//    objectName: "PageHeadStyle" // used in unit tests
//    contentHeight: units.gu(6)
//    fontWeight: Font.Light
//    textSize: Label.Large
//    textLeftMargin: units.gu(2)
//    maximumNumberOfActions: 3

    /*!
      The color of the buttons in the header.
     */
//    property color buttonColor: headerStyle.config.foregroundColor

    /*!
      The color of the title text.
     */
//    property color titleColor: headerStyle.config.foregroundColor

//    implicitHeight: headerStyle.contentHeight + divider.height + sectionsItem.height
    implicitHeight: units.gu(6) + divider.height


    Rectangle {
        anchors.fill: parent
        color: UbuntuColors.blue
    }

    // for Unity8
    // FIXME: Remove this property when we introduce a header preset that does not
    //  have a separator.
//    property alias __separator_visible: divider.visible

    Rectangle {
        id: divider
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: units.dp(1)
//        color: styledItem.dividerColor
        color: "black"
    }


//        ActionBar {
//            id: actionsContainer
//            objectName: "headerActionBar"
//            anchors {
//                top: parent.top
//                right: rightAnchor.left
//                rightMargin: actionsContainer.width > 0 ? units.gu(1) : 0
//            }
//            height: headerStyle.contentHeight

//            actions: headerStyle.config.actions
//            numberOfSlots: 3
//        }
}
