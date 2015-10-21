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
    objectName: "dragGestureTemplate"

    TemplateSection {
        className: "DragGesture"

        TemplateRow {
            title: i18n.tr("Upwards")
            Rectangle {
                width: units.gu(40)
                height: units.gu(20)
                color: theme.palette.normal.foreground

                DragGesture {
                    direction: DragGesture.Upwards
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }
                    height: units.gu(5)
                    Label {
                        text: i18n.tr("Upwards")
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
            title: i18n.tr("Downwards")
            Rectangle {
                width: units.gu(40)
                height: units.gu(20)
                color: theme.palette.normal.foreground

                DragGesture {
                    direction: DragGesture.Downwards
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
                width: units.gu(40)
                height: units.gu(20)
                color: theme.palette.normal.foreground

                DragGesture {
                    direction: DragGesture.Rightwards
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
                width: units.gu(40)
                height: units.gu(20)
                color: theme.palette.normal.foreground

                DragGesture {
                    direction: DragGesture.Leftwards
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
                            horizontalCenterOffset: parent.dragging ? parent.distance : 0
                        }
                    }
                }
            }
        }
    }
}
