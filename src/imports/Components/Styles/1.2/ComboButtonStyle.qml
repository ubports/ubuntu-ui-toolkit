/*
 * Copyright 2014 Canonical Ltd.
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
    \qmltype ComboButtonStyle
    \inqmlmodule Ubuntu.Components.Styles 1.1
    \ingroup style-api
    \brief Style API for ComboButton component.

    The component defines the style API for the ComboButton component.
  */
Item {
    /*!
      Width of the drop down button.
      */
    property real dropDownWidth

    /*!
      Width of the dropdown button separator.
      */
    property real dropDownSeparatorWidth

    /*!
      Distance between the combo list and the main button.
      */
    property real comboListMargin

    /*!
      The item which will hold the combo list data. Implementations can point
      both this and \l comboListPanel to the same component, however separate
      items should be used if a gap between the panel and the content is required.
      See Ambiance theme.
      */
    property Item comboListHolder

    /*!
      The item pointing to the panel holding the combo list and additional design
      artifacts. It is used by the component to drive the expansion size.
      */
    property Item comboListPanel

    /*!
      Default color for the main button.
      */
    property color defaultColor

    /*!
      Default gradient for the main button.
      */
    property Gradient defaultGradient

    /*!
      Default color for the dropdown button and the combo list both when expanded
      and collapsed. Implementations can define different color for expanded/collapsed
      using the \a styledItem.expanded property.
      */
    property color defaultDropdownColor

    /*!
      Default font used in the main button.
      */
    property font defaultFont
}
