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
 *
 */

import QtQuick 2.0
import Ubuntu.Components 1.3
import QtMultimedia 5.0

/*!
  \brief A Joke Downloader Page for Examples Gallery

  Connects to an online service which returns a json
  from which the joke is parsed into a Label.

  Demonstrates following:
     - ActivityIndicator
     - UbuntuShape
     - Label
     - i18n

  Uses:
     - QtMultimedia / SoundEffect
     - QML State
 */

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the .desktop filename
    applicationName: "jokes"

    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    automaticOrientation: true

    width: units.gu(100)
    height: units.gu(75)

    id: page


    property string url : "http://api.icndb.com/jokes/random?limitTo=[nerdy,explicit]";
    readonly property string _stateLoading : "Loading"
    readonly property string _stateNormal : "Normal"

    function getJoke() {
        laugh.stop();
        var xhr = new XMLHttpRequest();
        xhr.open("GET",url,true);
        xhr.onreadystatechange = function() {
            if ( xhr.readyState === xhr.DONE) {
                if ( xhr.status === 200) {
                    try {
                        var obj = JSON.parse(xhr.responseText);
                        if (obj.type === "success") {
                            jokeHolder.text = obj.value.joke;
                            page.state = page._stateNormal;
                            laugh.play();
                        }
                    } catch (e) {
                        getJoke();
                    }
                }
            }
        }
        xhr.send();
    }

    SoundEffect {
        id: laugh
        source: "laugh.wav"
    }

    UbuntuShape {
        color: Theme.palette.normal.base
        radius: "medium"
        anchors.centerIn: parent
        anchors.margins: units.gu(10)
        anchors.fill: parent

        ActivityIndicator {
            id: activityIndicator
            anchors.centerIn: parent
            running: true
            visible: false
        }

        Label {
            id: jokeHolder
            anchors.fill: parent
            text: "Tap here!"
            fontSize: "large"
            color: Theme.palette.normal.baseText
            textFormat: Text.RichText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: TextInput.WrapAtWordBoundaryOrAnywhere
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            page.state = page._stateLoading;
            page.getJoke();
        }
    }

    states: [
         State {
             name: page._stateNormal
             PropertyChanges { target: activityIndicator; visible: false}
             PropertyChanges { target: jokeHolder; visible: true}
         },
         State {
             name: page._stateLoading
             PropertyChanges { target: activityIndicator; visible: true}
             PropertyChanges { target: jokeHolder; visible: false}
         }
     ]
}
