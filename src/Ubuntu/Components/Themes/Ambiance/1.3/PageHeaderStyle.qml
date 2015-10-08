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
import Ubuntu.Components.Styles 1.3 as Style

Style.PageHeaderStyle {
    id: pageHeaderStyle

    foregroundColor: UbuntuColors.orange
    backgroundColor: "white"
    dividerColor: Qt.rgba(0, 0, 0, 0.1)
//    objectName: "PageHeadStyle" // used in unit tests
//    contentHeight: units.gu(6)
//    fontWeight: Font.Light
//    textSize: Label.Large
//    textLeftMargin: units.gu(2)
//    maximumNumberOfActions: 3

    property real contentHeight: units.gu(6)
    implicitHeight: contentHeight + divider.height // + sectionsItem.height

    defaultActionDelegate: AbstractButton {
        style: IconButtonStyle { }
        objectName: action.objectName + "_action_button"
        height: parent ? parent.height : undefined
        action: modelData
        StyleHints {
            foregroundColor: pageHeaderStyle.foregroundColor
        }
    }

    defaultContentsDelegate: Label {
        id: titleLabel
        text: styledItem.title

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            leftMargin: units.gu(2)
        }
        color: header.__styleInstance.foregroundColor
    }

//    defaultContents: Label {
//        text: styledItem.title
//        anchors {
//            left: parent ? parent.left : undefined
//            right: parent ? parent.right : undefined
//            verticalCenter: parent ? parent.verticalCenter : undefined
//        }
//        color: header.__styleInstance.foregroundColor
//    }

    Rectangle {
        anchors.fill: parent
        color: pageHeaderStyle.backgroundColor
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
        color: pageHeaderStyle.dividerColor
    }
}
