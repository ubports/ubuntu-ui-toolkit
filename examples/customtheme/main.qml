/*
 * Copyright 2015 Canonical Ltd.
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
//![0]
import QtQuick 2.4
import Ubuntu.Components 1.3

MainView {
    id: mainView
    objectName: "mainView"
    applicationName: "customtheme"

    width: units.gu(100)
    height: units.gu(75)

    theme.name: "theme"

    Page {
        title: i18n.tr("Theme sample")

        Column {
            spacing: units.gu(1)
            anchors {
                margins: units.gu(2)
                fill: parent
            }

            Label {
                text: i18n.tr("Theme.name:") + " " + theme.name
            }

            Button {
                width: parent.width
                text: i18n.tr("Set Ambiance theme")

                onClicked: {
                    theme.name = "Ubuntu.Components.Themes.Ambiance";
                }
            }
            Button {
                width: parent.width
                text: i18n.tr("Set SuruDark theme")

                onClicked: {
                    theme.name = "Ubuntu.Components.Themes.SuruDark";
                }
            }
            Button {
                width: parent.width
                text: i18n.tr("Application theme")

                onClicked: {
                    theme.name = "theme";
                }
            }
        }
    }
}
//![0]
