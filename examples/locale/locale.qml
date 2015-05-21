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
 *
 */

import QtQuick 2.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

/*!
  \brief An application to test locales on the device

  Demonstrates following:
     - ListItem
     - MainView
     - Label

  Uses:
     - Date()
     - Qt.locale
 */

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"

    // Note! applicationName needs to match the .desktop filename
    applicationName: "locale"

    /*
      This property enables the application to change orientation
      when the device is rotated. The default is false.
     */

    id: root
    width: units.gu(100)
    height: units.gu(75)
    automaticOrientation: true
    property string locale: view.currentItem.locale

    Page {
        id: page
        Item {
            id: chooser
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            height: parent.height/2
            ListView {
                highlightFollowsCurrentItem: true
                id: view
                clip: true
                focus: true
                anchors.fill: parent
                model: [
                    "hu_HU",
                    "he-IL",
                    "en_US",
                    "en_GB",
                    "fi_FI",
                    "de_DE",
                    "ar_SA",
                    "hi_IN",
                    "zh_CN",
                    "th_TH",
                    "fr_FR",
                    "nb_NO",
                    "sv_SE"
                ]
                delegate: ListItem.Standard {
                    property string locale: modelData
                    text: Qt.locale(modelData).name + " ("+ Qt.locale(modelData).nativeCountryName + "/" + Qt.locale(modelData).nativeLanguageName + ")"
                    onClicked: view.currentIndex = index
                }
                highlight: Rectangle {
                    height: units.gu(2)
                    width: view.width
                    color: theme.palette.selected.background
                }
            }
        }
        Rectangle {
            anchors {
                top: chooser.bottom
                topMargin: units.gu(0.5)
                leftMargin: units.gu(0.5)
                bottom: parent.bottom
            }
            color: theme.palette.normal.base
            width: parent.width
            Column {
                anchors.fill: parent
                spacing: units.gu(0.5)
                Label {
                    fontSize: "large"
                    property var date: new Date()
                    text: "Date: " + date.toLocaleDateString(Qt.locale(root.locale))
                }
                Label {
                    fontSize: "large"
                    property var date: new Date()
                    text: "Time: " + date.toLocaleTimeString(Qt.locale(root.locale))
                }
                Label {
                    fontSize: "large"
                    property var dow: Qt.locale(root.locale).firstDayOfWeek
                    text: "First day of week: " + Qt.locale(root.locale).standaloneDayName(dow)
                }
                Label {
                    fontSize: "large"
                    property var num: 10023823
                    text: "Number: " + num.toLocaleString(Qt.locale(root.locale))
                }
                Label {
                    fontSize: "large"
                    property var num: 10023823
                    text: "Currency: " + num.toLocaleCurrencyString(Qt.locale(root.locale))
                }
            }
        }
    }
}
