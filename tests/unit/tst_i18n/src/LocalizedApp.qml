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
import Ubuntu.Components 1.1

MainView {
    width: units.gu(48)
    height: units.gu(60)
    applicationName: 'localizedApp'

    Page {
        objectName: 'page'
        title: i18n.dtr('localizedApp', "Welcome")
        Button {
            id: button
            objectName: 'button'
            anchors.horizontalCenter: parent.horizontalCenter
            text: i18n.tr('Count the kilometres')
            width: units.gu(15)
        }
        Button {
            id: button2
            objectName: 'button2'
            anchors.top: button.bottom
            text: i18n.tag('Count the kittens')
            width: units.gu(15)
        }
        Label {
            id: all1
            objectName: 'all1'
            anchors.top: button2.bottom
            anchors.horizontalCenter: button.horizontalCenter
            text: i18n.ctr('All Contacts', 'All')
        }
        Label {
            id: all2
            objectName: 'all2'
            anchors.top: all1.bottom
            anchors.horizontalCenter: all1.horizontalCenter
            text: i18n.ctr('All Calls', 'All')
        }
        Label {
            id: all3
            objectName: 'all3'
            anchors.top: all2.bottom
            anchors.horizontalCenter: all2.horizontalCenter
            text: i18n.tag('All Cats', 'All')
        }
        Label {
            id: timeNow
            objectName: 'timeNow'
            anchors.top: all3.bottom
            anchors.horizontalCenter: all3.horizontalCenter
            text: i18n.relativeDateTime(new Date())
        }
        Label {
            id: timeMinuteBefore
            objectName: 'timeMinuteBefore'
            anchors.top: timeNow.bottom
            anchors.horizontalCenter: timeNow.horizontalCenter
            text: i18n.relativeDateTime(new Date(new Date().getTime() - 60000))
        }
        Label {
            id: timeMinuteAfter
            objectName: 'timeMinuteAfter'
            anchors.top: timeMinuteBefore.bottom
            anchors.horizontalCenter: timeMinuteBefore.horizontalCenter
            text: i18n.relativeDateTime(new Date(new Date().getTime() + 60000))
        }
        Label {
            id: tenMinutesBefore
            objectName: 'tenMinutesBefore'
            anchors.top: timeMinuteAfter.bottom
            anchors.horizontalCenter: timeMinuteAfter.horizontalCenter
            text: i18n.relativeDateTime(new Date(new Date().getTime() - 600000))
        }
        Label {
            id: tenMinutesAfter
            objectName: 'tenMinutesAfter'
            anchors.top: tenMinutesBefore.bottom
            anchors.horizontalCenter: tenMinutesBefore.horizontalCenter
            text: i18n.relativeDateTime(new Date(new Date().getTime() + 600000))
        }
        Label {
            id: timeFarAway
            objectName: 'timeFarAway'
            anchors.top: tenMinutesAfter.bottom
            anchors.horizontalCenter: tenMinutesAfter.horizontalCenter
            text: i18n.relativeDateTime(new Date(2000, 0, 1, 0, 0, 0, 0))
        }
    }
}
