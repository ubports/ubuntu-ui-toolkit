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

/*!
    \qmltype Palette
    \inqmlmodule Ubuntu.Components.Themes 1.0
    \ingroup theme-module
    \brief Provides the palette of colors from the theme that widgets use to draw themselves.

    Palette provides access to colors defined by the current theme. The actual
    color palette to use depends on the state of the widget being drawn.

    A color palette (\l PaletteValues) has various roles to choose from depending
    on what part of the widget is being drawn (the base of the widget, the
    foreground, etc.).

    For each color there is a corresponding 'text' color that is guaranteed to
    provide good legibility for text or icons placed on top of a background
    of the corresponding color.

    Example of a Text on top of a background Rectangle:
    \qml
    Item {
        Rectangle {
            color: theme.palette.normal.base
        }

        Text {
            color: theme.palette.normal.baseText
        }
    }
    \endqml
*/
QtObject {
    /*!
       Color palette to use when the widget is in the default state.
    */
    property PaletteValues normal: PaletteValues{}

    /*!
      Color palette to use when the widget is in the disabled (enabled: false)
      state.
      \since Ubuntu.Components.Themes 1.3
    */
    property PaletteValues disabled: PaletteValues{}

    /*!
       Color palette to use when the widget is focused.
    */
    property PaletteValues focused: PaletteValues{}

    /*!
       Color palette to use when the widget is selected, for example when
       a tab is the current one.
    */
    property PaletteValues selected: PaletteValues{}

    /*!
       Color palette to use when the widget is selected and disabled.
      \since Ubuntu.Components.Themes 1.3
    */
    property PaletteValues selectedDisabled: PaletteValues{}

    /*!
      Color palette values to use when the widget is highlighted.
      \since Ubuntu.Components.Themes 1.3
      */
    property PaletteValues highlighted: PaletteValues{}
}
