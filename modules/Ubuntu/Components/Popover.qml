/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Authors:
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 2.0
//import ".."

Rectangle {



    id: popupBox

    signal popupInteractionCompleted();

    property int contentLeft: 1
    property int contentTop: 1
    property alias contentWidth: contentArea.width
    property alias contentHeight: contentArea.height

    property int originCueX
    property alias originCueHeight: originCueImage.height
    property alias originCueWidth: originCueImage.width

    property int popupOriginX: 0
    property int popupOriginY: 0
    property int fadeDuration: 300

    // readonly
    property int cueArrowOffset: Math.floor(originCueWidth / 2)
    property int boundingOffset: gu(0.5)

    function flipVisibility() {
        state = (state == "shown" ? "hidden" : "shown");
    }

    color: "transparent"

    x: {
        var result = (popupOriginX >= 0) ? popupOriginX - cueArrowOffset : (parent.width + popupOriginX) - width - cueArrowOffset;
        if (result < boundingOffset)
            result = boundingOffset;
        else if (result + width > parent.width - boundingOffset)
            result = parent.width - width - boundingOffset;

        return result;
    }
    y: {
        var result = (popupOriginY >= 0) ? popupOriginY : (parent.height + popupOriginY) - height;
        if (result < 0)
            result = boundingOffset;
        else if (result + height > parent.height)
            result = parent.height - height - originCueHeight;

        return result;
    }

    z: 32

    originCueX: {
        var result = (popupOriginX >= 0) ? x : width - originCueWidth;
        if (result < 0)
            result = 0;
        else if (result > width - originCueWidth)
            result = width - cueArrowOffset;

        return result;
    }

    state: "hidden"

    states: [
        State { name: "shown"; },
        State { name: "hidden"; }
    ]

    visible: state === "shown"

    //  transitions: [
    //    Transition { from: "shown"; to: "hidden";
    //      FadeOutAnimation { target: popupBox; duration: fadeDuration;
    //          easingType: Easing.InQuint; }
    //    },
    //    Transition { from: "hidden"; to: "shown";
    //      FadeInAnimation { target: popupBox; duration: fadeDuration;
    //          easingType: Easing.InQuint; }
    //    }
    //  ]

    Rectangle {
        id: contentArea

        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width
        height: parent.height - originCueHeight;

        color: "#bcbdc0"
        border.color: "#a7a9ac"
        border.width: 1
    }

    //  Image {
    Rectangle {
        color: "red"
        id: originCueImage

        x: originCueX
        anchors.top: contentArea.bottom
        width: 39
        height: 25

        //    source: "img/popup-arrow.png"
        //    cache: true
    }
}
