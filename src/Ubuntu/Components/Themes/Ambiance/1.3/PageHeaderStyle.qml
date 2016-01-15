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
import QtQuick.Window 2.2
import Ubuntu.Components 1.3
import Ubuntu.Components.Styles 1.3 as Style

Style.PageHeaderStyle {
    id: pageHeaderStyle

    foregroundColor: theme.palette.normal.backgroundText
    backgroundColor: theme.palette.normal.background
    dividerColor: theme.palette.normal.base
    property int fontWeight: Font.Light
    property int textSize: Label.Large

    // reduced content height on a phone in landscape mode
    contentHeight: Screen.height > units.gu(50) ? units.gu(6) : units.gu(5)
    implicitHeight: contentHeight + divider.height + internal.extensionHeight

    Object {
        id: internal
        property real extensionHeight: styledItem.extension ?
                                         styledItem.extension.height
                                       : styledItem.sections.height
    }

    defaultActionDelegate: AbstractButton {
        style: IconButtonStyle { }
        objectName: action.objectName + "_button"
        height: parent ? parent.height : undefined
        action: modelData
        StyleHints {
            // FIXME: introduce inactiveForegroundColor to PageHeaderStyle
            foregroundColor: pageHeaderStyle.foregroundColor
        }
    }

    titleComponent: Item {
        // The wrapping inside the extra Item is needed to
        //  be able to position the Label vertically. Without it,
        //  the Loader resizes the Label to span the full height.
        Label {
            id: titleLabel
            objectName: "header_title_label"
            text: styledItem.title

            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            color: pageHeaderStyle.foregroundColor
            font.weight: pageHeaderStyle.fontWeight
            textSize: pageHeaderStyle.textSize
            elide: Text.ElideRight
        }
    }

    Rectangle {
        objectName: "header_background"
        anchors.fill: parent
        color: pageHeaderStyle.backgroundColor
    }

    Rectangle {
        id: divider
        objectName: "header_divider"
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: units.dp(1)
        color: pageHeaderStyle.dividerColor
    }
}
