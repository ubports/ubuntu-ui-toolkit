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
    \qmltype PageHeaderStyle
    \inqmlmodule Ubuntu.Components.Styles 1.3
    \ingroup style-api
    \brief Style API for page header.

    The component defines the style API for the \l PageHeader component.
  */
Item {
    /*!
      The color of the title and the action buttons in the header.
     */
    property color foregroundColor

    /*!
      \deprecated
      The color of disabled action buttons in the header.
      Replaced by buttons.disabledForegroundColor.
     */
    property color disabledForegroundColor

    /*!
      The color of the (optional) subtitle.
     */
    property color subtitleColor

    /*!
      The color of the background of the header.
     */
    property color backgroundColor

    /*!
      Configuration of the colors of the action buttons in the header.
     */
    readonly property ActionItemProperties buttons: ActionItemProperties { }

    /*!
      The color of the divider at the bottom of the header.
     */
    property color dividerColor

    /*!
      The delegate used for the buttons in the leading and
      trailing \l ActionBar in the header.
     */
    property Component defaultActionDelegate

    /*!
      The default contents that will be loaded when no contents Item was set.
     */
    property Component titleComponent

    /*!
      The default component to display the subtitle.
     */
    property Component subtitleComponent

    /*!
      The height of the header excluding the divider and extension.
     */
    property real contentHeight
}
