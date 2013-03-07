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
import Ubuntu.Components.ListItems 0.1 as ListItem

Template {
    title: i18n.tr("Header")
    objectName: "Header"

    Item {
        MainView {
            id: mainView
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            width: units.gu(40)
            height: units.gu(55)

            Page {
                anchors.fill: parent
                title: i18n.tr("Title")

                tools: ToolbarActions {
                    Action {
                        text: "example"
                        iconSource: Qt.resolvedUrl("call_icon.png")
                    }
                }

                Flickable {
                    id: flickable
                    anchors.fill: parent
                    contentHeight: column.height
                    contentWidth: parent.width

                    Column {
                        id: column
                        width: parent.width
                        height: childrenRect.height

                        Label {
                            text: "\n\n\n\n\n\n\n\n\n\n" +
                                  i18n.tr("This is a page with a flickable.") +
                                  "\n\n\n\n\n\n\n\n\n\n(" +
                                  i18n.tr("scroll down") + ")\n\n\n"
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Repeater {
                            model: 20
                            Label {
                                text: "Lorem ipsum dolor sit amet, platea est tincidunt nunc, commodo odio elit."
                                width: parent.width
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                        Label {
                            text: "\n\n\n" + i18n.tr("The end.")
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }
    }
}
