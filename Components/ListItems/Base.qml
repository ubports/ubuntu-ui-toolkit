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

import QtQuick 1.1
import ".."

/*!
    \qmlclass Default
    \inqmlmodule UbuntuUIToolkit
    \brief The Default list item class.

    Examples:
    \qml
        Container {
            ListItem.Header { text: "Actions" }
            ListItem.Base {
                height: 50
                Button {
                    anchors.centerIn: parent
                    width: parent.width - 20
                    text:"click me"
                }
            }
            ListItem.Base {
                height: 100
                Tabs {
                    height: parent.height - 20
                    width: parent.width - 20
                    anchors.centerIn: parent
                    Tab {
                        text: "Tab 1"
                        page: TextCustom {
                            anchors.centerIn: parent
                            text: "Page one"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                    Tab {
                        text: "Tab 2"
                        page: TextCustom {
                            anchors.centerIn: parent
                            text: "Page two"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                    Tab {
                        text: "Tab 3"
                        page: TextCustom {
                            anchors.centerIn: parent
                            text: "Page three"
                            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
                        }
                    }
                }
            }
        }
    \endqml

    See the documentation of the derived classes of Base for more examples.
    \b{This component is under heavy development.}
*/
Item {
    id: baseListItem
    width: 250

    signal clicked

    AbstractButton {
        anchors.fill: parent
        enabled: baseListItem.enabled
        onClicked: baseListItem.clicked()
    }

    /*!
      \internal
     */
    property bool __showTopSeparator: false

    /*!
      \internal
     */
    property bool __showBottomSeparator: false

    /*!
      \internal
      Override in divider classes that should never
      draw the one-pixel divider at the top or bottom
      of the list item.
     */
    property bool __isDivider: false

    Image {
        id: topSeparatorLine
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: visible ? 2 : 0
        source: "artwork/divider_Horizontal.png"
        visible: baseListItem.__showTopSeparator
    }
    Image {
        id: bottomSeparatorLine
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: visible ? 2 : 0
        source: "artwork/divider_Horizontal.png"
        visible: baseListItem.__showBottomSeparator
    }
}
