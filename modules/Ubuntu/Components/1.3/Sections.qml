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

/*!
    \qmltype Sections
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \since Ubuntu.Components 1.3
    \brief Display a list of sections that the user can select.
 */
StyledItem {
    id: sections
    style: theme.createStyleComponent("SectionsStyle.qml", sections)

    /*!
      List of strings that represent section names. Example:
      \qml
        Sections {
            model: [
                "first",
                "second",
                "third"
            ]
        }
     \endqml
     It is strongly recommended to limit the number of sections to two or three.
     */
    property var model
    onModelChanged: {
        if (model && model.length > 3) {
            // FIXME: Make the Sections scrollable for more than 3 sections.
            console.warn("It is not YET recommended or supported to use more than three sections.");
        }
    }

    /*!
      The index of the currently selected section in \l model.
     */
    property int selectedIndex: model ? 0 : -1
}
