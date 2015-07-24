/*
 * Copyright 2014-2015 Canonical Ltd.
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

/*!
    \qmltype PageHeadSections
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \since Ubuntu.Components 1.1
    \brief PageHeadSections is used to configure the sections for a \l Page.

    These sections will be shown in the bottom part of the header. This component does not
    need to be instantiated by the developer, it is automatically part of \l PageHeadConfiguration.
 */
QtObject {
    // To be used inside PageHeadConfiguration
    id: sections

    /*!
      Set this property to false to disable user interaction to change the selected section.
      Default value: true
     */
    property bool enabled: true

    /*!
      List of strings that represent section names. Example:
      \qml
        import Ubuntu.Components 1.2
        import QtQuick 2.4

        MainView {
            width: units.gu(50)
            height: units.gu(80)

            Page {
                id: page
                title: "Sections"
                head {
                    sections {
                        model: ["one", "two", "three"]
                    }
                }

                Label {
                    anchors.centerIn: parent
                    text: "Section " + page.head.sections.selectedIndex
                }
            }
        }
     \endqml
     It is strongly recommended to limit the number of sections to two or three.
     */
    property var model
    onModelChanged: {
        if (model && model.length > 3) {
            console.warn("It is not recommended or supported to use more than three sections in Page.head.sections.model.");
        }
    }

    /*!
      The index of the currently selected section in \l model.
     */
    property int selectedIndex: model ? 0 : -1
}
