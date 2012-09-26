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
      The location of the icon to show in the list item (optional).
      \qmlproperty url iconSource
     */
    //    property alias iconSource: iconHelper.source

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
    //    property alias iconFrame: iconHelper.hasFrame

    /*!
      \internal
      The margin on the left side of the icon.
      \qmlproperty real leftIconMargin
     */
    // FIXME: Remove this when the setting becomes part of the theming engine
    //    property alias __leftIconMargin: iconHelper.leftIconMargin

    /*!
      \internal
      The margin on the right side of the icon.
      \qmlproperty real rightIconMargin
     */
    // FIXME: Remove this when the setting becomes part of the theming engine
    //    property alias __rightIconMargin: iconHelper.rightIconMargin

    //    Icon {
    //        id: iconHelper
    //    }

    property variant icon
    property Item __leftItem
    onIconChanged: {
        if (__leftItem) {
            __leftObject.visible = false;
            if (__leftItem.pleaseCleanMeUp === true) __leftObject.destroy();
            __leftItem = null;
        }

        if (typeof icon == "string") {
            // leftItem is the url of an image
            var objectstring = 'import Ubuntu.Components.ListItems 0.1; IconVisual { source: "'+icon+'" }';
            __leftItem = Qt.createQmlObject(objectstring, baseListItem);
            __leftItem.__pleaseCleanMeUp = true;
        } else {
            __leftItem = icon;
            __leftItem.parent = baseListItem;
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
            //            left: iconHelper.right
            left: parent.left
            right: progressionHelper.left
            leftMargin: __leftItem ? __leftItem.width + __leftItem.anchors.leftMargin + __leftItem.anchors.rightMargin : 0
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
