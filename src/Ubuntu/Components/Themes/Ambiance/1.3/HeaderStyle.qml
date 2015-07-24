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
    id: headerStyle
    /*!
      The height of the headercontents, which is the full height of
      the header minus the separators shown at the bottom of it.
     */
    property real contentHeight: units.gu(7.5)

    /*!
      The source of the image that separates the header from the contents of a \l MainView.
      The separator will be drawn over the contents.
     */
    property url separatorSource: Qt.resolvedUrl("../artwork/PageHeaderBaseDividerLight.sci")

    /*!
      The source of an additional image attached to the bottom of the separator. The contents
      of the \l MainView will be drawn on top of the separator bottom image.
     */
    property url separatorBottomSource: Qt.resolvedUrl("../artwork/PageHeaderBaseDividerBottom.png")

    property int fontWeight: Font.Light
    property string fontSize: "x-large"
    property color textColor: theme.palette.selected.backgroundText
    property real textLeftMargin: units.gu(2)

    implicitHeight: headerStyle.contentHeight + separator.height + separatorBottom.height

    /*!
      \internal
      Tabs needs to call sync of the TabBar
     */
    property TabBar __tabBar: tabBarLoader.sourceComponent ? tabBarLoader.item : null

    BorderImage {
        id: separator
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        source: headerStyle.separatorSource
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
        id: foreground
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: headerStyle.contentHeight

        Label {
            LayoutMirroring.enabled: Qt.application.layoutDirection == Qt.RightToLeft

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                leftMargin: headerStyle.textLeftMargin
            }
            text: styledItem.title
            font.weight: headerStyle.fontWeight
            visible: !styledItem.tabsModel && !styledItem.contents
            fontSize: headerStyle.fontSize
            color: headerStyle.textColor
        }

        Binding {
            target: styledItem.contents
            property: "anchors.fill"
            value: foreground
            when: styledItem.contents
        }
        Binding {
            target: styledItem.contents
            property: "parent"
            value: foreground
            when: styledItem.contents
        }

        Loader {
            id: tabBarLoader
            sourceComponent: styledItem.tabsModel && !styledItem.contents ? tabBarComponent : null
            anchors.fill: parent
        }

        Component {
            id: tabBarComponent
            TabBar {
                id: tabBar
                anchors.fill: parent
                model: styledItem.tabsModel
                animate: styledItem.animate
            }
        }
    }
}
