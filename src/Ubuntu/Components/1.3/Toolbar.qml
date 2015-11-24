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
import Ubuntu.Components 1.3

/*!
    \qmltype Toolbar
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    TODO
*/
StyledItem {
    id: bar
    styleName: "ToolbarStyle"

    /*!
      The actions to display in the bar.
      If more actions are specified than there are slots, an overflow
      button will be show which opens a popover with the actions that
      did not fit in the bar directly.
     */
    property list<Action> actions

    /*!
      Set this to override the default delegate button that shows
      the actions in the ActionBar. Uses the defaultDelegate of
      ToolbarStyle by default.
     */
    property Component delegate: __styleInstance.defaultDelegate

    /*!
      A single fixed action displayed on the left side of the toolbar.
     */
    property Action fixedAction
}
