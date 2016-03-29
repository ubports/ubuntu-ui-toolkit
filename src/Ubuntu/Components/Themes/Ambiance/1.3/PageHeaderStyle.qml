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
    subtitleColor: "pink"
    backgroundColor: theme.palette.normal.background
    dividerColor: theme.palette.normal.base
    property int fontWeight: Font.Light
    property int textSize: Label.Large
    property int subTextSize: internal.landscape ? Label.Small : Label.Medium

    contentHeight: internal.titleAreaHeight + internal.titleBottomSpacing
    implicitHeight: contentHeight + divider.height + internal.extensionHeight

    Component.onCompleted: internal.updateHeights()
    Object {
        id: internal
        property real extensionHeight: styledItem.extension ?
                                         styledItem.extension.height
                                       : styledItem.sections.height

        property bool landscape: Screen.height <= units.gu(50)
        property bool subtitle: styledItem.subtitle && !styledItem.contents
        property bool extension: styledItem.extension

        // the height of the area in which the title and icons are vertically centered
        property real titleAreaHeight
        // add to the contents height for additional space under the title
        property real titleBottomSpacing

        onLandscapeChanged: updateHeights()
        onSubtitleChanged: updateHeights()
        onExtensionChanged: updateHeights()

        function updateHeights() {
            if (landscape) {
                if (subtitle) {
                    titleAreaHeight = units.gu(4);
                    titleBottomSpacing = units.gu(2);
                } else if (extension) {
                    titleAreaHeight = units.gu(4);
                    titleBottomSpacing = units.gu(1);
                } else {
                    titleAreaHeight = units.gu(5)
                    titleBottomSpacing = 0;
                }
            } else { // portrait
                titleAreaHeight = units.gu(6);
                if (subtitle) {
                    titleBottomSpacing = units.gu(1);
                } else {
                    titleBottomSpacing = 0;
                }
            }
        }
    }

    defaultActionDelegate: AbstractButton {
        style: IconButtonStyle { }
        objectName: action.objectName + "_button"
        height: internal.titleAreaHeight
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
                verticalCenterOffset: -internal.titleBottomSpacing/2
            }
            color: pageHeaderStyle.foregroundColor
            font.weight: pageHeaderStyle.fontWeight
            textSize: pageHeaderStyle.textSize
            elide: Text.ElideRight
        }
    }

    subtitleComponent: Item {
        // Item fills the contents height
        Label {
            anchors {
                left: parent.left
                right: parent.right
                baseline: parent.bottom
                baselineOffset: -units.gu(1)
            }
            objectName: "header_subtitle_label"
            text: styledItem.subtitle
            color: pageHeaderStyle.subtitleColor
            visible: text != ""
            textSize: pageHeaderStyle.subTextSize
            font.weight: pageHeaderStyle.fontWeight
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
