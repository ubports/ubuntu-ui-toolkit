/*
 * Copyright 2016 Canonical Ltd.
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
    \qmltype SectionsStyle
    \inqmlmodule Ubuntu.Components.Styles 1.3
    \ingroup style-api
    \brief Style API for Sections.

    The component defines the style API for the \l Sections component.
  */
Item {
    /*!
      The foreground color of unselected sections.
     */
    property color sectionColor

    /*!
      The foreground color of underline of unselected sections.
     */
    property color underlineColor

    /*!
      The foreground color of the selected section.
     */
    property color selectedSectionColor

    /*!
      The background color for the pressed section button.
     */
    property color pressedBackgroundColor

    /*!
      The size of text in the buttons.
     */
    property int textSize
}
