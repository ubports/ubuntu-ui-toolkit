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

/*!
    \qmltype PageHeadConfiguration
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \since Ubuntu.Components 1.1
    \brief Page.head is used to configure the header for a \l Page.

    For examples how to use Page.head, see \l Page.
 */
Object {
    // To be used inside a Page only.
    id: headerConfig

    /*!
      List of actions to show in the header.

      Example:
      \qml
        Page {
            title: "Custom header actions"
            head.actions: [
                Action {
                    iconName: "save"
                    text: i18n.tr("Save")
                },
                Action {
                    iconName: "add"
                    text: i18n.tr("Add")
                }
            ]
        }
      \endqml
     */
    property list<Action> actions

    /*!
      Overrides the default \l PageStack back button and the
      \l Tabs drawer button in the header.

      Example:
      \qml
        Page {
            title: "Back Action Page"
            head.backAction: Action {
                iconName: "close"
                onTriggered: {
                    console.log("Run custom back action")
                }
            }
        }
     \endqml
     */
    property Action backAction: null

    /*!
      Set this property to show this Item in the header instead of
      the title. Use a \l TextField here for implementing search in header.

      The parent of this Item will be binded while the \l Page is active.
      The header contents will automatically be anchored to the left and
      vertically centered inside the header.

      Example:
      \qml
        Page {
            title: "Invisible title"
            head.contents: Rectangle {
                color: UbuntuColors.orange
                height: units.gu(5)
                width: parent ? parent.width - units.gu(2) : undefined
            }
        }
      \endqml

      See \l PageHeadState for an example that shows how search mode can
      be implemented.
     */
    property Item contents: null

    /*!
      \qmlproperty PageHeadSections sections
      Defines the sections in the page header divider.
     */
    readonly property alias sections: headSections
    PageHeadSections {
        id: headSections
    }

    /*!
      The color of the text and icons.
     */
    property color foregroundColor: Theme.palette.selected.backgroundText
}
