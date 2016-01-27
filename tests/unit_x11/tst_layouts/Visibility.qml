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
import Ubuntu.Components 1.3
import Ubuntu.Layouts 1.0

MainView {
    id: main
    objectName: "mainView"
    applicationName: "test-app"

    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    automaticOrientation: true // TODO: support wide aspect ratios, esp. on prepareGamePage

    property bool boundValue: true

    width: units.gu(100)
    height: units.gu(75)

    function portraitLayout() {
        width = units.gu(50);
    }
    function landscapeLayout() {
        width = units.gu(100);
    }

    PageStack {
        Component.onCompleted: push(testPage)

        Page {
            title: i18n.tr("Conditional Layouts with PageStack")
            id: testPage
            visible: false

            Layouts {
                id: layouts
                objectName: "layoutManager"
                width: parent.width
                height: parent.height
                layouts: [
                    ConditionalLayout {
                        name: "wide"
                        when: layouts.width < units.gu(60)
                        UbuntuShape {
                            width: units.gu(20)
                            height: units.gu(20)
                            backgroundColor: "red"
                            Label { text: "wide" }
                        }
                    }
                ]

                // This is a default layout.
                // When using a PageStack and layouts is not empty, it won't display or switch to the default layout.
                // However it only works when putting it into the layouts list with an inversed condition.
                UbuntuShape {
                    objectName: "DefaultLayout"
                    width: units.gu(20)
                    height: units.gu(20)
                    backgroundColor: "green"
                    visible: main.boundValue
                    Label { text: "default" }
                }
            }
        }
    }
}
