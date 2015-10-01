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

/*!
    \qmltype AnimatedItem
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief The AnimatedItem drives the animated components behavior inside a Flickable.
    Reports whether the component whos parent is a Flickable is in the visible area or not,
    so derived components can pause animations while off-screen.

*/

import QtQuick 2.4
import Ubuntu.Components 1.3

StyledItem {
    id: root
    /*!
      \deprecated
      Specifies whether the component is on the visible area of the Flickable or not.
    */
    property bool onScreen: true
}
