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

QtObject {
    /*!
      Setting this property to true will cause the tab buttons to expand
      their widths equally to fill the Tabs bar.
      If the value is false, instead each button will use the width that
      is required by the largest tab button.
     */
    property bool buttonsExpanded: false

    /*!
      If this optional property is specified, it will be positioned
      between the bar with tab buttons, and the tab pages to act
      as a separator.
     */
    property Item separator: Rectangle { height: units.dp(1); color: "white" }

    /*!
      The padding on the left and right side of the row of buttons.
    */
    property real horizontalPadding: units.gu(1)

    /*!
      The minimum width of the tab buttons
     */
    property real minimumButtonWidth: units.gu(3)
}
