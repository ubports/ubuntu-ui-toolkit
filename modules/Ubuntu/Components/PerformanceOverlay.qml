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

/*!
    \qmltype PerformanceOverlay
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief DOCME

*/

import QtQuick 2.0
import Ubuntu.Components 0.1 as Ubuntu

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

                Ubuntu.RenderingTimes {
                    id: metrics
                    period: 2000
                    samples: renderingTimeGraph.width
                }

                BarGraph {
                    id: renderingTimeGraph
                    anchors.left: parent.left
                    anchors.right: parent.right
                    model: metrics.graphModel
                    maximumValue: 20
                    threshold: 16
                    labels: [{"color": "darkorange", "value": 10, "label": "10 ms"},
                            {"color": "red", "value": 16, "label": "16 ms"}]
                }
            }

        }
    }

    property bool active: false
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
