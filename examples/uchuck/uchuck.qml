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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import QtMultimedia 5.0

/*!
  \brief A Joke Downloader Page for Examples Gallery

  Connects to an online service which returns a json
  from which the joke is parsed into a Label.

  Demonstrates following:
     - Page
     - ActivityIndicator
     - UbuntuShape
     - Label
     - i18n

  Uses:
     - QtMultimedia / SoundEffect
     - QML State
 */

Page {
    id: page
    title: i18n.tr("uChuck")

    property string url : "http://api.icndb.com/jokes/random?limitTo=[nerdy,explicit]";
    readonly property string _stateLoading : "Loading"
    readonly property string _stateNormal : "Normal"
    property string rootPath : ""

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
                            page.state = _stateNormal;
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
        source: rootPath + "laugh.wav"
    }

    UbuntuShape {
        color: "#888888"
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
            fontSize: "normal"
            color: "#f1f1f1"
            textFormat: Text.RichText
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: TextInput.WrapAtWordBoundaryOrAnywhere
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            page.state = _stateLoading;
            page.getJoke();
        }
    }

    states: [
         State {
             name: _stateNormal
             PropertyChanges { target: activityIndicator; visible: false}
             PropertyChanges { target: jokeHolder; visible: true}
         },
         State {
             name: _stateLoading
             PropertyChanges { target: activityIndicator; visible: true}
             PropertyChanges { target: jokeHolder; visible: false}
         }
     ]
}

