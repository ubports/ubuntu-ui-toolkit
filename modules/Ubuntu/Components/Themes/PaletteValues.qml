/*
 * Copyright 2013 Canonical Ltd.
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

import QtQuick 2.0

/*!
    \qmltype PaletteValues
    \inqmlmodule Ubuntu.Components.Themes 0.1
    \ingroup theming
    \brief Palette of standard colors used by the widgets to draw themselves.
*/
QtObject {
    property color window
    property color windowText
    property color background
    property color backgroundText
    property color foreground
    property color foregroundText
    property color tooltip
    property color tooltipText
    property color field
    property color fieldText
}
