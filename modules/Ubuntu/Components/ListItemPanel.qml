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

import QtQuick 2.2
import Ubuntu.Components 1.1

/*
  This component is the holder of the ListItem options.
  */
Rectangle {
    id: panel
    width: optionsRow.childrenRect.width + 2 * optionsRow.spacing
    height: parent ? parent.height : 10

    /*
      Specifies whether the panel is used to visualize leading or trailing options.
      */
    property bool leadingPanel: false
    /*
      The delegate to be used to visualize the options
      */
    property Component delegate

    /*
      Options
      */
    property var optionList

    /*
      Signal emitted when an option has been triggered.
      */
    signal selected()

    color: leadingPanel ? UbuntuColors.red : UbuntuColors.green

    anchors {
        left: (leadingPanel) ? undefined : (parent ? parent.right : undefined)
        right: (leadingPanel) ? (parent != null ? parent.left : undefined) : undefined
        top: parent ? parent.top : undefined
        bottom: parent ? parent.bottom : undefined
    }

    Row {
        id: optionsRow
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: spacing
        }

        Repeater {
            model: panel.optionList
            Loader {
                width: Math.max(height, item ? item.width : 0)
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                sourceComponent: panel.delegate ? panel.delegate : defaultDelegate
                onItemChanged: {
                    if (item) {
                        item.objectName = "list_option_" + index
                        item.action = modelData;
                        item.triggered.connect(selected);
                    }
                }
            }
        }
    }

    Component {
        id: defaultDelegate
        AbstractButton {
            id: optionItem
            height: parent.height
            width: height
            anchors.verticalCenter: parent.verticalCenter

            Icon {
                width: units.gu(2.5)
                height: width
                anchors.centerIn: parent
                name: optionItem.iconName
                color: "white"
                keyColor: "#808080"
            }
        }
    }
}
