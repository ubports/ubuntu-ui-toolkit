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
    \qmlclass TabGroup
    \inqmlmodule UbuntuUIToolkit
    \brief The TabGroup groups together Items, of which only one
            is active and visible to the user. TabButtons are used
            to control the currently active Item.

    \b{This component is under heavy development.}
*/

Item {
    id: tabGroup
    property Item currentTab

    width: parent ? parent.width : 0
    height: parent ? parent.height : 0
}
