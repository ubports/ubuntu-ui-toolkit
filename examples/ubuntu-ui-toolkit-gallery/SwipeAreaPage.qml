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

Template {
    objectName: "SwipeAreaTemplate"
    scrollable: !(upwards.dragging | downwards.dragging |
                rightwards.dragging | leftwards.dragging)

    TemplateSection {
        className: "SwipeArea"

        TemplateRow {
            title: i18n.tr("Upwards")
            Rectangle {
                width: parent.width / 1.5
                height: width / 2
                color: theme.palette.normal.foreground

                SwipeArea {
                    id: upwards
                    direction: SwipeArea.Upwards
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }
                    height: units.gu(4)
                    Label {
                        text: i18n.tr("Upwards")
                        color: theme.palette.normal.foregroundText
                        anchors {
                            centerIn: parent
                            verticalCenterOffset: parent.dragging ? -parent.distance : 0
                        }
                    }
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Downwards")
            Rectangle {
                width: parent.width / 1.5
                height: width / 2
                color: theme.palette.normal.foreground

                SwipeArea {
                    id: downwards
                    direction: SwipeArea.Downwards
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }
                    height: units.gu(5)
                    Label {
                        text: i18n.tr("Downwards")
                        color: theme.palette.normal.foregroundText
                        anchors {
                            centerIn: parent
                            verticalCenterOffset: parent.dragging ? parent.distance : 0
                        }
                    }
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Rightwards")
            Rectangle {
                width: parent.width / 1.5
                height: width / 2
                color: theme.palette.normal.foreground

                SwipeArea {
                    id: rightwards
                    direction: SwipeArea.Rightwards
                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: units.gu(5)

                    Label {
                        text: i18n.tr("Rightwards")
                        rotation: -90
                        color: theme.palette.normal.foregroundText
                        anchors {
                            centerIn: parent
                            horizontalCenterOffset: parent.dragging ? parent.distance : 0
                        }
                    }
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Leftwards")
            Rectangle {
                width: parent.width / 1.5
                height: width / 2
                color: theme.palette.normal.foreground

                SwipeArea {
                    id: leftwards
                    direction: SwipeArea.Leftwards
                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: units.gu(5)

                    Label {
                        text: i18n.tr("Leftwards")
                        rotation: 90
                        color: theme.palette.normal.foregroundText
                        anchors {
                            centerIn: parent
                            horizontalCenterOffset: parent.dragging ? -parent.distance : 0
                        }
                    }
                }
            }
        }
    }
}
