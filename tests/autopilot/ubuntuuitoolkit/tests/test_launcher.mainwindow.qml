/*
 * Copyright 2014-2016 Canonical Ltd.
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

import QtQuick 2.4
import QtQuick.Window 2.2 // Not Ubuntu.Test
import Ubuntu.Components 1.3
import Ubuntu.Components.Labs 1.3

MainWindow {
    title: i18n.tr("Hello World")
    minimumWidth: units.gu(30)
    minimumHeight: units.gu(50)
    maximumWidth: units.gu(90)
    maximumHeight: units.gu(120)

    Item {
        anchors.fill: parent
        objectName: "mainView"

        Page {
            anchors.fill: parent
            header: PageHeader {
                title: "Launcher/Window"
            }

            Column {
                anchors.top: parent.header.bottom
                Label {
                    objectName: "label"
                    text: i18n.tr("Lorem ipsum dolor sit amet")
                    width: units.gu(25)
                    height: units.gu(25)
                }
            }
        }
    }
}
