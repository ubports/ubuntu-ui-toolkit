/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.0
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

/*!
    \internal
    \qmltype Header
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
*/
Item {
    id: header
    // FIXME: see above
    Theming.ItemStyle.class: "header"


    property string title

    anchors {
        left: parent.left
        right: parent.right
    }
    y: 0

    onYChanged: print("y = "+y)

    Behavior on y {
        enabled: !(header.selectedFlickable && header.selectedFlickable.moving)
        SmoothedAnimation {
            duration: 200
        }
    }

//    height: tabBar.height + separator.height + separatorBottom.height
//    height: tabBar.height //units.gu(10)
    height: Theming.ComponentUtils.delegateProperty(header, "height", units.gu(5))
//    height: units.gu(10)
    onHeightChanged: print("header height = "+height)
//    height: units.gu(5)


    function show() {
        header.y = 0;
    }

    function hide() {
        header.y = - header.height;
    }

//    NewTabBar {
//        id: tabBar
//        tabs: item
//        anchors {
//            top: parent.top
//            left: parent.left
//            right: parent.right
//        }
//    }

    Item {
        id: tabBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: units.gu(20)
    }

//    property Tab selectedTab: item ? item.selectedTab : null
//    // use updateFlickable() to update selectedFlickable so that events from the
//    // previous selectedFlickable can be disconnected.
    property Flickable selectedFlickable: null
    property real previousContentY: 0
//    onSelectedTabChanged: updateFlickable()
//    Component.onCompleted: updateFlickable()
    onSelectedFlickableChanged: connectFlickable()
    Component.onCompleted: connectFlickable()

    function connectFlickable() {
        if (selectedFlickable) {
            previousContentY = selectedFlickable.contentY;
            selectedFlickable.contentYChanged.connect(header.scrollContents);
            selectedFlickable.movementEnded.connect(header.movementEnded);
        }
    }

//    function updateFlickable() {
//        if (selectedFlickable) {
//            selectedFlickable.contentYChanged.disconnect(header.scrollContents);
//            selectedFlickable.movementEnded.disconnect(header.movementEnded);
//        }
//        if (selectedTab && selectedTab.autoHideTabBar && selectedTab.__flickable) {
//            selectedFlickable = selectedTab.__flickable;
//            previousContentY = selectedFlickable.contentY;
//            selectedFlickable.contentYChanged.connect(header.scrollContents);
//            selectedFlickable.movementEnded.connect(header.movementEnded);
//        } else {
//            selectedFlickable = null;
//        }
//    }

    function scrollContents() {
        // Avoid updating header.y when rebounding or being dragged over the bounds.
        if (!selectedFlickable.atYBeginning && !selectedFlickable.atYEnd) {
            var deltaContentY = selectedFlickable.contentY - previousContentY;
            print("scrolling by "+deltaContentY)
            header.y = MathUtils.clamp(header.y - deltaContentY, -header.height, 0);
            print("header.y = "+header.y);
        }
        previousContentY = selectedFlickable.contentY;
    }

    function movementEnded() {
        if (selectedFlickable.contentY < 0) header.show();
        else if (header.y < -header.height/2) header.hide();
        else header.show();
    }
}
