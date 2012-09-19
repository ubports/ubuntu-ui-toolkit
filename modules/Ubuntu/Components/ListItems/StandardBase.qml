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

import QtQuick 1.1
import Ubuntu.Components 0.1

/*!
    \internal
    \qmlclass StandardBase
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \brief Parent class of various list item classes that can have
        an icon and a progression symbol.

    Examples: See subclasses
    \b{This component is under heavy development.}
*/
Base {
    id: listItem
    height: 48

    /*!
      \preliminary
      The location of the icon to show in the list item (optional).
      \qmlproperty url iconSource
     */
    property alias iconSource: iconHelper.source

    /*!
      \preliminary
      Show or hide the progression symbol.
     */
    property bool progression: false

    /*!
      \preliminary
      Show or hide the frame around the icon
     */
    property alias iconFrame: iconHelper.hasFrame

    /*!
      \preliminary
      Show a split between the progessions symbol and the
      left part of the list item.
     */
    property alias showSplit: progressionHelper.showSplit

    IconVisual {
        id: iconHelper
    }
    property alias iconHelper: iconHelper
    property alias progressionHelper: progressionHelper

    /*!
      \internal
      Anchor for the left-side of the contents of the list item.
      Used in subclasses of StandardBase.
     */
    property alias __leftAnchor: iconHelper.right

    /*!
      \internal
      Anchor for the right-side of the contents of the list item.
      Used in subclasses of StandardBase.
     */
    property alias __rightAnchor: progressionHelper.left

    /*!
      \internal
      Width of the list item minus the width of the icon and progression symbol.
      Used in subclasses of StandardBase.
     */
    property real __middleWidth: listItem.width - iconHelper.width - progressionHelper.width

    ProgressionVisual {
        id: progressionHelper
        visible: listItem.progression
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
    }
}
