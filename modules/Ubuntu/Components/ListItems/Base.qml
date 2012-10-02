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
    \qmlclass Base
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \brief Parent class of various list item classes that can have
        an icon and a progression symbol.

    Examples: See subclasses
    \b{This component is under heavy development.}
*/
Empty {
    id: baseListItem
    height: 48

    /*!
      \preliminary
      The location of the icon to show in the list item (optional), or an Item that is
      shown on the left side inside the list item. The icon will automatically be
      anchored to the left side of the list item, and if its height is undefined, to the top
      and bottom of the list item.
      \qmlproperty variant icon
    */
    property variant icon

    /*!
      \preliminary
      The location of the icon to show in the list item if iconSource failed to load (optional).
      \qmlproperty url fallbackIconSource
     */
    property alias fallbackIconSource: iconHelper.fallbackSource

    /*!
      \preliminary
      Show or hide the progression symbol.
     */
    property bool progression: false

    /*!
      \preliminary
      Show or hide the frame around the icon
      \qmlproperty bool iconFrame
     */
    property alias iconFrame: iconHelper.hasFrame

    /*!
      \internal
      The margin on the left side of the icon.
      \qmlproperty real leftIconMargin
     */
    // FIXME: Remove this when the setting becomes part of the theming engine
    property alias __leftIconMargin: iconHelper.leftIconMargin

    /*!
      \internal
      The margin on the right side of the icon.
      \qmlproperty real rightIconMargin
     */
    // FIXME: Remove this when the setting becomes part of the theming engine
    property alias __rightIconMargin: iconHelper.rightIconMargin

    IconVisual {
        id: iconHelper
    }

    /*!
      \internal
      The \l icon property is an Item. The value is false if \l icon is a string,
      or when no icon was set.
     */
    property bool __iconIsItem: false

    /*!
      \internal
     */
    onIconChanged: {
        if (typeof icon == "string") {
            // icon is the url of an image
            iconHelper.source = icon;
            __iconIsItem = false;
        } else {
            // icon is an Item.
            __iconIsItem = true;
            iconHelper.source = "";

            icon.parent = baseListItem;
            icon.anchors.left = baseListItem.left;
            if (!icon.height) {
                icon.anchors.top = baseListItem.top;
                icon.anchors.bottom = baseListItem.bottom;
            }
        }
    }

    /*!
      \internal
     */
    property alias children: middle.children
    Item {
        id: middle
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: __iconIsItem ? parent.left : iconHelper.right
            right: progressionHelper.left
            leftMargin: (__iconIsItem) ? icon.width + icon.anchors.leftMargin + icon.anchors.rightMargin : 0
        }
    }

    ProgressionVisual {
        id: progressionHelper
        visible: baseListItem.progression
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
    }
}
