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
    \qmlclass SingleValue
    \inqmlmodule UbuntuUIToolkit
    \brief The SingleValue class is DOCME

    \b{This component is under heavy development.}

*/
Selectable {
    id: multiValueListItem
    height: 54

    property alias iconSource: iconHelper.source
    property alias text: label.text
    property alias progression: progressionHelper.visible
//    property alias value: valueLabel.text
    property variant values

    BackgroundVisual {
        anchors.fill: parent
        showTopSeparator: multiValueListItem.__isFirst
        showBottomSeparator: true
    }

    Row {
        anchors.fill: parent
        IconVisual { id: iconHelper; height: parent.height }
        Item {
            id: middleVisuals
            width: parent.width - iconHelper.width - progressionHelper.width
            height: label.height + valueLabel.height
            anchors.verticalCenter: parent.verticalCenter

            LabelVisual {
                id: label
                selected: multiValueListItem.selected
                anchors {
                    top: parent.top
                    leftMargin: 5
                    left: parent.left
                }
                width: parent.width - 5
                height: implicitHeight
            }
            LabelVisual {
                id: valueLabel
                selected: multiValueListItem.selected
                anchors {
                    top: label.bottom
                    leftMargin: 5
                    left: parent.left
                }
                width: parent.width - 5
                fontSize: "medium"
                text: {
                    var n = multiValueListItem.values.length;
                    var result;
                    if (n > 0) {
                        result = multiValueListItem.values[0];
                        for (var i = 1; i < n; i++) {
                            result = result + ", " + multiValueListItem.values[i];
                        }
                    } else {
                        result = "void";
                    }
                    return result;
                }
            } // valueLabel
        } // middleVisuals
        ProgressionVisual {
            id: progressionHelper
            showSplit: true
        }
    }
}
