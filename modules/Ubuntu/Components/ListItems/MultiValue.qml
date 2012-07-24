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

/*!
    \qmlclass MultiValue
    \inqmlmodule UbuntuUIToolkit 0.1
    \brief List item displaying multiple values.
    \b{This component is under heavy development.}

    Examples:
    \qml
        Container {
            MultiValue {
                text: "Label"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                onClicked: selected = !selected
            }
            MultiValue {
                text: "Label"
                iconSource: "icon.png"
                values: ["Value 1", "Value 2", "Value 3", "Value 4"]
                progression: true
                onClicked: print("clicked")
            }
        }
    \endqml
*/
Selectable {
    id: multiValueListItem
    height: 54

    /*!
      \preliminary
      The location of the icon to show in the list item (optional).
      \qmlproperty url iconSource
     */
    property alias iconSource: iconHelper.source

    /*!
      \preliminary
      The text that is shown in the list item as a label.
      \qmlproperty string text
     */
    property alias text: label.text

    /*!
      \preliminary
      Show or hide the progression symbol.
      \qmlproperty bool progression
     */
    property alias progression: progressionHelper.visible

    /*!
      \preliminary
      The list of values that will be shown under the label text
     */
    property variant values

    Row {
        anchors.fill: parent
        IconVisual {
            id: iconHelper;
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
        }
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
                width: parent.width
            }
            LabelVisual {
                id: valueLabel
                selected: multiValueListItem.selected
                anchors {
                    top: label.bottom
                    leftMargin: 5
                    left: parent.left
                }
                width: parent.width
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
            showSplit: false
        }
    }
}
