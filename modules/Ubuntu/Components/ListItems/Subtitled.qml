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
import Ubuntu.Components 0.1

/*!
    \qmlclass MultiValue
    \inqmlmodule UbuntuUIToolkit 0.1
    \brief List item with multiple values.
    \b{This component is under heavy development.}

    Examples:
    \qml
        Container {
            Subtitled {
                text: "Default"
                subText: "Secondary label"
            }
            Subtitled {
                text: "Disabled"
                enabled: false
                subText: "Secondary label"
            }
            Subtitled {
                text: "Selected"
                selected: true
                subText: "Secondary label"
            }
            Subtitled {
                text: "Progression"
                subText: "Secondary label"
                progression: true
            }
            Subtitled {
                text: "Icon"
                subText: "Secondary label"
                iconSource: "avatar_contacts_list.png"
            }
            Subtitled {
                text: "Multiple lines"
                subText: "This is a multi-line subText.\nUp to 5 lines are supported."
            }
            Subtitled {
                text: "Multiple lines"
                subText: "It also works well with icons and progression."
                iconSource: "avatar_contacts_list.png"
                progression: true
            }
        }
    \endqml
*/
Selectable {
    id: subtitledListItem
    height: Math.max(middleVisuals.height, 54)

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
     */
    property bool progression: false

    /*!
      \preliminary
      The list of strings that will be shown under the label text
      \qmlproperty string subText
     */
    property alias subText: subLabel.text

    Row {
        width: parent.width
        height: parent.height

        Item {
            width: iconHelper.width
            height: iconHelper.height
            IconVisual { id: iconHelper; height: 46 }
        }
        Column  {
            id: middleVisuals
            width: parent.width - iconHelper.width - progressionHelper.width
            height: spacing.height + label.height + subLabel.height
            Item {
                id: topSpacing
                width: parent.width
                height: 7
            }
            LabelVisual {
                id: label
                selected: subtitledListItem.selected
                anchors {
                    leftMargin: 5
                    left: parent.left
                }
                width: parent.width - 5
            }
            TextCustom {
                id: subLabel
                property bool selected: subtitledListItem.selected
                anchors {
                    leftMargin: 5
                    left: parent.left
                }

                fontSize: "medium"
                elide: Text.ElideRight
                color: selected ? "#f37505" : Qt.rgba(0.4, 0.4, 0.4, 1.0)
                style: Text.Raised
                styleColor: "white"
                opacity: label.enabled ? 1.0 : 0.5
                width: parent.width - 5
                wrapMode: Text.Wrap
                maximumLineCount: 5
            }
            Item {
                id: bottomSpacing
                width: parent.width
                height: 8
            }
        } // middleVisuals

        ProgressionVisual {
            id: progressionHelper
            visible: subtitledListItem.progression
            showSplit: true
        }
    }
}
