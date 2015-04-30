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

import QtQuick 2.4
import Ubuntu.Components 1.2

/*!
    \qmltype Base
    \inqmlmodule Ubuntu.Components.ListItems 1.0
    \ingroup ubuntu-listitems
    \brief Parent class of various list item classes that can have
        an icon and a progression symbol.
    \note \b{The component is deprecated. Use ListItem component instead.}

    Examples: See subclasses
*/
Empty {
    id: baseListItem

    /*!
      \deprecated

      \b{Use iconName or iconSource instead.}

      The location of the icon to show in the list item (optional), or an Item that is
      shown on the left side inside the list item. The icon will automatically be
      anchored to the left side of the list item, and if its height is undefined, to the top
      and bottom of the list item.
      \qmlproperty variant icon
    */
    property variant icon: iconSource != "" ? iconSource : undefined

    /*!
      The location of the icon to show in the list item if iconSource failed to load (optional).
      \qmlproperty url fallbackIconSource
     */
    property alias fallbackIconSource: iconHelper.fallbackSource

    /*!
      The icon shown in the list item if iconName failed to load (optional).

      \qmlproperty string fallbackIconName

      If both fallbackIconSource and fallbackIconName are defined, fallbackIconName will be ignored.

      \note The complete list of icons available in Ubuntu is not published yet.
            For now please refer to the folders where the icon themes are installed:
            \list
              \li Ubuntu Touch: \l file:/usr/share/icons/suru
              \li Ubuntu Desktop: \l file:/usr/share/icons/ubuntu-mono-dark
            \endlist
            These 2 separate icon themes will be merged soon.
    */
    property alias fallbackIconName: iconHelper.fallbackIconName

    /*!
      Show or hide the progression symbol.
     */
    property bool progression: false

    /*!
      Show or hide the frame around the icon
      \qmlproperty bool iconFrame
     */
    property alias iconFrame: iconHelper.hasFrame

    /*!
      \internal
      \deprecated
      Width of the icon to be displayed
    */
    property real __iconWidth

    /*!
      \internal
      \deprecated
      Height of the icon to be displayed
    */
    property real __iconHeight

    /*!
      \internal
      \deprecated
      The margin on the left side of the icon.
     */
    property real __leftIconMargin

    /*!
      \internal
      \deprecated
      The margin on the right side of the icon.
     */
    property real __rightIconMargin


    IconVisual {
        id: iconHelper

        width: height
        height: Math.min(units.gu(5), parent.height - units.gu(1))
        anchors {
            left: parent.left
            leftMargin: baseListItem.__contentsMargins
            verticalCenter: parent.verticalCenter
        }
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
        if (icon == undefined) return;
        if (icon != iconSource) {
            console.warn("WARNING: ListItems.Base.icon is DEPRECATED. " +
                         "Use iconName and iconSource instead.")
        }
        if (typeof icon == "string" || typeof icon == typeof iconSource) {
            // icon is the url of an image
            iconHelper.source = icon;
            __iconIsItem = false;
        } else {
            // icon is an Item.
            __iconIsItem = true;
            iconHelper.source = "";

            icon.parent = baseListItem;
            icon.anchors.left = baseListItem.left;
            icon.anchors.margins = Qt.binding(function() { return baseListItem.__contentsMargins });
            if (!icon.height) {
                icon.anchors.top = baseListItem.top;
                icon.anchors.bottom = baseListItem.bottom;
            }
        }
    }

    /*!
      \internal
     */
    default property alias children: middle.data
    Item {
        id: middle
        property bool anchorToIconHelper: !__iconIsItem && iconHelper.source != ""
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: anchorToIconHelper ? iconHelper.right : parent.left
            right: baseListItem.progression ? progressionHelper.left : parent.right
            rightMargin: baseListItem.__contentsMargins
            leftMargin: __iconIsItem ? icon.width + 2 * baseListItem.__contentsMargins : baseListItem.__contentsMargins
        }
    }

    ProgressionVisual {
        id: progressionHelper
        visible: baseListItem.progression
        anchors {
            right: parent.right
            rightMargin: baseListItem.__contentsMargins
            top: parent.top
            bottom: parent.bottom
        }
    }
}
