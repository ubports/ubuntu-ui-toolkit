/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.2

/*!
    \brief MainView with a Label and Button elements.
*/

MainView {
    id: mainView
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "customtheme"

    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    //automaticOrientation: true

    width: units.gu(100)
    height: units.gu(75)

    Component.onCompleted: Theme.name = "theme"

    Page {
        title: i18n.tr("Theme sample")

        Column {
            spacing: units.gu(1)
            anchors {
                margins: units.gu(2)
                fill: parent
            }

            Label {
                text: i18n.tr("Theme.name:") + " " + Theme.name
            }

            Button {
                width: parent.width
                text: i18n.tr("Light background")

                onClicked: {
                    mainView.backgroundColor = "white";
                }
            }
            Button {
                width: parent.width
                text: i18n.tr("Dark background")

                onClicked: {
                    mainView.backgroundColor = "blue";
                }
            }
            Button {
                width: parent.width
                text: i18n.tr("Set Ambiance theme")

                onClicked: {
                    Theme.name = "Ubuntu.Components.Themes.Ambiance";
                }
            }
            Button {
                width: parent.width
                text: i18n.tr("Set SuruDark theme")

                onClicked: {
                    Theme.name = "Ubuntu.Components.Themes.SuruDark";
                }
            }
            Button {
                width: parent.width
                text: i18n.tr("Application theme")

                onClicked: {
                    Theme.name = "theme";
                }
            }
        }
    }
}

