/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.PerformanceMetrics 0.1 as PerformanceMetrics

Item {
    id: overlay

    anchors.fill: parent

    Component {
        id: overlayContent

        Item {
            MouseArea {
                anchors.fill: content
                drag.target: content
                onClicked: overlay.active = false
            }

            Item {
                id: content
                x: parent.width - width - units.gu(2)
                y: units.gu(10)
                width: units.gu(38)
                height: childrenRect.height

                PerformanceMetrics.RenderingTimes {
                    id: renderingTimes
                    period: 2000
                    samples: renderingTimeGraph.width
                }

                PerformanceMetrics.CpuUsage {
                    id: cpuUsage
                    period: 2000
                    samplingInterval: 100
                }

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: units.gu(1)

                    BarGraph {
                        id: renderingTimeGraph
                        anchors.left: parent.left
                        anchors.right: parent.right
                        model: renderingTimes.graphModel
                        maximumValue: 20
                        threshold: 16
                        labels: [{"color": "darkorange", "value": 10, "label": "10 ms"},
                                {"color": "red", "value": 16, "label": "16 ms"}]
                    }

                    BarGraph {
                        id: cpuUsageGraph
                        anchors.left: parent.left
                        anchors.right: parent.right
                        model: cpuUsage.graphModel
                        maximumValue: 100
                        threshold: 75
                        labels: [{"color": "green", "value": 25, "label": "25%"},
                                 {"color": "darkorange", "value": 50, "label": "50%"},
                                 {"color": "red", "value": 75, "label": "75%"}]
                    }
                }
            }

        }
    }

    property bool active: true
    property int delayBetweenPresses: 200

    MouseArea {
        id: leftArea

        property var timeLastPress

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: units.gu(10)
        height: units.gu(10)
        onPressed: {
            mouse.accepted = false;
            timeLastPress = new Date().getTime();
            if (timeLastPress - rightArea.timeLastPress <= delayBetweenPresses) {
                overlay.active = true;
            }
        }
    }

    MouseArea {
        id: rightArea

        property var timeLastPress

        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: units.gu(10)
        height: units.gu(10)
        onPressed: {
            mouse.accepted = false;
            timeLastPress = new Date().getTime();
            if (timeLastPress - leftArea.timeLastPress <= delayBetweenPresses) {
                overlay.active = true;
            }
        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        active: overlay.active
        asynchronous: true
        sourceComponent: overlayContent
    }
}
