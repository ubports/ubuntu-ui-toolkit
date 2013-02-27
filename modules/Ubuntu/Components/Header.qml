/*
 * Copyright 2013 Canonical Ltd.
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
    onTitleChanged: {
        if (title) header.show();
        else header.hide();
    }

    anchors {
        left: parent.left
        right: parent.right
    }
    y: 0

    Behavior on y {
        enabled: !(header.flickable && header.flickable.moving)
        SmoothedAnimation {
            duration: 200
        }
    }
    height: Theming.ComponentUtils.delegateProperty(header, "height", units.gu(10))

    function show() {
        header.y = 0;
    }

    function hide() {
        header.y = - header.height;
    }

    //    Rectangle {
    //        id: tabBar
    //        color: "#00ff00aa"
    //        anchors {
    //            top: parent.top
    //            left: parent.left
    //            right: parent.right
    //        }
    //        height: units.gu(20)
    //    }

    property Flickable flickable: null
    onFlickableChanged: internal.connectFlickable()
    Component.onCompleted: internal.connectFlickable()

    QtObject {
        id: internal

        property real previousContentY: 0
        //    onSelectedTabChanged: updateFlickable()
        //    Component.onCompleted: updateFlickable()

        property Flickable previousFlickable: null

        function connectFlickable() {
            if (previousFlickable) {
                previousFlickable.contentYChanged.disconnect(header.scrollContents);
                previousFlickable.movementEnded.disconnect(header.movementEnded);
            }

            if (flickable) {
                // Set-up the top-margin of the contents of the Flickable so that
                //  the contents is never hidden by the header:
                flickable.contentY = -header.height;
                flickable.topMargin = header.height;

                // Connect moving inside the flickable to movements of the header
                previousContentY = flickable.contentY;
                flickable.contentYChanged.connect(internal.scrollContents);
                flickable.movementEnded.connect(internal.movementEnded);
            }
            previousFlickable = flickable;
        }

        //    function updateFlickable() {
        //        if (flickable) {
        //            flickable.contentYChanged.disconnect(header.scrollContents);
        //            flickable.movementEnded.disconnect(header.movementEnded);
        //        }
        //        if (selectedTab && selectedTab.autoHideTabBar && selectedTab.__flickable) {
        //            flickable = selectedTab.__flickable;
        //            previousContentY = flickable.contentY;
        //            flickable.contentYChanged.connect(header.scrollContents);
        //            flickable.movementEnded.connect(header.movementEnded);
        //        } else {
        //            flickable = null;
        //        }
        //    }

        /*!
      Update the position of the header to scroll with the flickable.
     */
        function scrollContents() {
            // Avoid updating header.y when rebounding or being dragged over the bounds.
            if (!flickable.atYBeginning && !flickable.atYEnd) {
                var deltaContentY = flickable.contentY - previousContentY;
                header.y = MathUtils.clamp(header.y - deltaContentY, -header.height, 0);
            }
            previousContentY = flickable.contentY;
        }

        /*!
      Fully show or hide the header, depending on its current y.
     */
        function movementEnded() {
            if (flickable.contentY < 0) header.show();
            else if (header.y < -header.height/2) header.hide();
            else header.show();
        }
    }
}
