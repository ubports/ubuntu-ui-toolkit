/*
 * Copyright 2012 Canonical Ltd.
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
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

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
    property url separatorSource: "artwork/PageHeaderBaseDividerLight.sci"

    /*!
      The source of an additional image attached to the bottom of the separator. The contents
      of the \l MainView will be drawn on top of the separator bottom image.
     */
    property url separatorBottomSource: "artwork/PageHeaderBaseDividerBottom.png"

    property int fontWeight: Font.Light
    property string fontSize: "x-large"
    property color textColor: Theme.palette.selected.backgroundText
    property real textLeftMargin: units.gu(2)

    implicitHeight: headerStyle.contentHeight + separator.height + separatorBottom.height

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
        id: leftButtonContainer
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: childrenRect.width

        AbstractButton {
            id: tabsButton
            objectName: "tabsButton"
            height: parent ? parent.height : undefined
            width: visible ? units.gu(5) : 0

            iconName: "navigation-menu"
            visible: styledItem.tabsModel !== null
            text: visible ? styledItem.tabsModel.count + " tabs" : ""

            // XXX: We currently use an AbstractButton with ToolbarButtonStyle because
            //  a ToolbarButton does not has its own MouseArea to handle interaction,
            //  that was done in the Toolbar.
            style: Theme.createStyleComponent("ToolbarButtonStyle.qml", tabsButton)

            onTriggered: {
                tabsPopover.show();
            }

            Popover {
                id: tabsPopover
                objectName: "tabsPopover"
                parent: QuickUtils.rootItem(tabsPopover)
                caller: tabsButton
                Column {
                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }
                    Repeater {
                        model: styledItem.tabsModel
                        ListItem.Standard {
                            visible: index !== styledItem.tabsModel.selectedIndex
                            text: tab.title // XXX: only "title" doesn't work with i18n.tr(). Why not?
                            objectName: "tabButton" + index
                            onClicked: {
                                styledItem.tabsModel.selectedIndex = index;
                                tabsPopover.hide();
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
            fontSize: headerStyle.fontSize
            color: headerStyle.textColor
        }
    }
}
