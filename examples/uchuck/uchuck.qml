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
import QtMultimedia 5.0 // depends on libdeclarative-multimedia

PageStack {
    id: pageStack
    width: units.gu(38)
    height: units.gu(48)
    anchors.margins: units.gu(10)
    Component.onCompleted: push(page0)

    SoundEffect {
        id: laugh
        source: "laugh.wav"
    }

    Page {
        id: page0
        title: i18n.tr("uChuck - the app which gives you the kick")

        property string url : "http://api.icndb.com/jokes/random?limitTo=[nerdy,explicit]";

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
                                activityIndicator.visible = false;
                                jokeHolder.visible = true;
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
                    fontSize: "x-large"
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
                activityIndicator.visible = true;
                jokeHolder.visible = false;
                page0.getJoke();
            }
        }
    }
}
