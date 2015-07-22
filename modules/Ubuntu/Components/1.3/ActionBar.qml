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
    \qmltype ActionBar
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \brief Show a row of buttons that trigger actions. When the
        number of visible actions specified is larger than
        \l numberOfSlots, an overflow button will be shown which
        triggers an overflow panel that shows the remaining actions.

    Example:

    \qml
        ActionBar {
            numberOfSlots: 2
            actions: [
                Action {
                    iconName: "share"
                    text: "Share"
                },
                Action {
                    iconName: "starred"
                    text: "Favorite"
                },
                Action {
                    iconName: "lock"
                    text: "Lock"
                }
            ]
        }
    \endqml
*/
StyledItem {
    id: bar
    styleName: "ActionBarStyle"

    /*!
      The actions to display in the bar.
      If more actions are specified than there are slots, an overflow
      button will be show which opens a popover with the actions that
      did not fit in the bar directly.
     */
    property list<Action> actions
    
    /*!
      The number of actions to be shown in the bar directly.
      If the actions don't fit, an overflow button (using one slot)
      will be shown which opens a popover with the remaining actions.

      Setting numberOfSlots to 0 will always show the overflow button
      and no other action buttons. Default value is 3.
     */
    property int numberOfSlots: 3
}
