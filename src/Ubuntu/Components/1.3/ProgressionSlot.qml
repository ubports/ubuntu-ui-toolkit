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
    \qmltype ProgressionSlot
    \inqmlmodule Ubuntu.Components 1.3
    \inherits Icon
    \ingroup ubuntu

    \brief ProgressionSlot holds an icon representing
    the progression symbol.

    ProgressionSlot is designed to provide an easy way for developers to
    add a progression symbol to the list item created using \l ListItemLayout.

    There is no need to specify any margin or position, \l ListItemLayout will
    take care of that and it will also make sure that the progression symbol
    is always displayed as the last trailing slot inside the layout. For more
    details, see \l ListItemLayout documentation.

    The following is an example of how easy it is to implement list items using
    \l ListItem with \l ListItemLayout and ProgressionSlot:

    \qml
        ListItem {
            height: layout.height
            onClicked: pushPageOnStack()
            ListItemLayout {
                id: layout
                title.text: "Push a new page on the PageStack"
                ProgressionSlot {}
            }
        }
    \endqml
    \sa ListItemLayout
*/

Icon {
    height: units.gu(2.5)
    name: "chevron"
    SlotsLayout.position: SlotsLayout.Last
}
