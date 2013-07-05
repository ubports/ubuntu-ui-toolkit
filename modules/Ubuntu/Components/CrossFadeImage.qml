/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
/*!
   \qmltype CrossFadeImage
   \ingroup ubuntu
   \brief An Image like component which smoothly fades when its source is updated.

   \qml
        import QtQuick 2.0

        import Ubuntu.Components 0.1

        MainView {

            width: units.gu(100)
            height: units.gu(75)

            Page {
                title: "An example"

                CrossFadeImage {
                    anchors.fill: parent
                    anchors.margins: units.gu(10)

                    fillMode: Image.PreserveAspectCrop

                    property string url1: "http://design.ubuntu.com/wp-content/uploads/ubuntu-logo14.png"
                    property string url2: "http://design.ubuntu.com/wp-content/uploads/canonical-logo1.png"

                    source: url1

                    fadeDuration: 1000

                    MouseArea {
                        anchors.fill: parent
                        onClicked: parent.source = parent.url2
                    }
                }
            }
        }
    \endqml
    */

Item {
    /*!
      The image being displayed. Can be a URL to any image format supported by Qt.
     */
    property url source

    /*!
      Set this property to define what happens when the source image has a different size than the item.
    */
    property int fillMode

    /*!
      The time (in ms) over which to fade between images. Defaults to 400.
    */
    property int fadeDuration : 400

    /*!
      Whether the animation is running
    */
    readonly property bool running: nextImageFadeIn.running

    /*!
      The actual width and height of the lodaded image
    */
    readonly property size sourceSize: priv.currentImage.sourceSize

    /*!
      The status of image loading
    */
    readonly property var status: priv.currentImage ? priv.currentImage.status : Image.Null

    /*! \internal
      The current image
    */
    readonly property alias currentImage: priv.currentImage

    /*! \internal
      The next image
    */
    readonly property alias nextImage: priv.nextImage


    QtObject {
        id: priv

        /*! \internal
          Defines the image currently being shown
        */
        property Image currentImage: image1
        /*! \internal
          Defines the image being changed to
        */
        property Image nextImage: image2

        function swapImages() {
            priv.currentImage.z = 0
            priv.nextImage.z = 1
            nextImageFadeIn.start();

            var tmpImage = priv.currentImage
            priv.currentImage = priv.nextImage
            priv.nextImage = tmpImage
        }


    }

    Image {
        id: image1
        anchors.fill: parent
        cache: false
        asynchronous: true
        z: 1
    }

    Image {
        id: image2
        anchors.fill: parent
        cache: false
        asynchronous: true
        z: 0
    }

    /*!
      \internal
      Do the fading when the source is updated
    */
    onSourceChanged: {
        // On creation, the souce handler is called before image pointers are set.
        if (priv.currentImage === null) {
            priv.currentImage = image1;
            priv.nextImage = image2;
        }

        nextImageFadeIn.stop();

        // Don't fade in initial picture, only fade changes
        if (priv.currentImage.source == "") {
            priv.currentImage.fillMode = fillMode
            priv.currentImage.source = source;
        } else {
            nextImageFadeIn.stop();
            priv.nextImage.opacity = 0.0;
            priv.nextImage.fillMode = fillMode;
            priv.nextImage.source = source;

            // If case the image is still in QML's cache, status will be "Ready" immediately
            if (priv.nextImage.status === Image.Ready || priv.nextImage.source === "") {
                priv.swapImages();
            }
        }
    }

    /*!
      \internal
      Set the fill on the currently displaying image when it's updated.
    */
    onFillModeChanged: {
        priv.currentImage.fillMode = fillMode
    }

    Connections {
        target: priv.nextImage
        onStatusChanged: {
            if (priv.nextImage.status == Image.Ready) {
                 priv.swapImages();
             }
        }
    }

    NumberAnimation {
        id: nextImageFadeIn
        target: priv.nextImage
        property: "opacity"
        duration: fadeDuration
        to: 1.0
        easing.type: Easing.InOutQuad

        onRunningChanged: {
            if (!running) {
                priv.nextImage.source = "";
            }
        }
    }
}
