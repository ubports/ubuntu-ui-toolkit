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
import Ubuntu.Components 0.1

Item {
    id: root

    property string themeError: Theme.error
    property string themeFile: Theme.currentTheme

    Item {
        // fake style to enable styling of the item
        ItemStyle.class: "button"
        // test item reflecting button properties styled
        property color color: otherColor
        property url iconSource
        property string text
        property string iconPosition

        property color otherColor: "green"
    }
}
