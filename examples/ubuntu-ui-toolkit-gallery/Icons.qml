/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Qt.labs.folderlistmodel 2.1

Template {
    objectName: "iconsTemplate"

    TemplateSection {
        className: "Icon"

        TemplateRow {
            title: i18n.tr("Scaling")
            spacing: units.gu(2)

            Icon {
                name: "call-start"
                width: units.gu(2)
                height: width
                anchors.verticalCenter: parent.verticalCenter
            }

            Icon {
                name: "call-start"
                width: units.gu(6)
                height: width
                anchors.verticalCenter: parent.verticalCenter
            }

            Icon {
                name: "call-start"
                width: units.gu(12)
                height: width
            }
        }

        TemplateRow {
            title: i18n.tr("Color")
            spacing: units.gu(2)

            Icon {
                name: "stock_alarm-clock"
                width: units.gu(3)
                height: width
            }

            Icon {
                name: "stock_alarm-clock"
                color: UbuntuColors.orange
                width: units.gu(3)
                height: width
            }

            Icon {
                name: "stock_alarm-clock"
                color: UbuntuColors.lightAubergine
                width: units.gu(3)
                height: width
            }
        }

        TemplateRow {
            title: i18n.tr("Theme")
            spacing: units.gu(2)
            height: iconFlow.height

            Flow {
                id: iconFlow
                width: parent.width
                spacing: units.gu(2)

                Repeater {
                    model: FolderListModel {
                        folder: "/usr/share/icons/suru/actions/scalable"
                        showDirs: false
                        showOnlyReadable : true
                        sortField: FolderListModel.Name
                        nameFilters: [ "*.svg" ]
                    }
                    Icon {
                        id: themedIcon
                        name: fileBaseName || ""
                        width: units.gu(3)
                        height: width
                        MouseArea {
                            anchors.fill: parent
                            onClicked: PopupUtils.open(iconPop, themedIcon, { 'iconName': themedIcon.name })
                        }
                        Component {
                            id: iconPop
                            Popover {
                                id: iconPopover
                                property string iconName: "N/A"
                                Item {
                                    anchors {
                                        left: parent.left
                                        right: parent.right
                                    }
                                    height: units.gu(6)
                                    UbuntuShape {
                                        id: iconShape
                                        anchors {
                                            left: parent.left
                                            top: parent.top
                                            bottom: parent.bottom
                                        }
                                        width: units.gu(6)
                                        Icon {
                                            anchors.fill: parent
                                            name: iconPopover.iconName
                                        }
                                    }

                                    Label {
                                        anchors {
                                            verticalCenter: parent.verticalCenter
                                            left: iconShape.right
                                            leftMargin: units.gu(1)
                                        }
                                        text: iconPopover.iconName
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
