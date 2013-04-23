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

import QtQuick 2.0
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

// internal class, used inside subclasses of PopupBase
Item {
    id: pointer
    Theming.ItemStyle.class: "pointer"

    // Using Item.Rotation does not play well with the
    //  translation that would be needed after rotating.
    property real longAxis
    property real shortAxis

    // up, down, left or right, or none to hide the pointer
    property string direction: "down"

    // rotate pointer 90 degrees
    property bool rotate: (direction === "left" || direction === "right")

    width: rotate ? shortAxis : longAxis
    height: rotate ? longAxis : shortAxis
}
