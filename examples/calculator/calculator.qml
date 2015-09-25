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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components 1.3
import Ubuntu.Layouts 1.0
import "components"
import "components/calculator.js" as Calculator

/*!
    \brief MainView with a Label and Button elements.
*/

MainView {
    id: mainView
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "mainView"
    
    // Note! applicationName needs to match the .desktop filename
    applicationName: "calculator"
    
    /*
     This property enables the application to change orientation
     when the device is rotated. The default is false.
    */
    automaticOrientation: true
    
    width: units.gu(40)
    height: units.gu(71)

    property var calculator: new Calculator.Calculator(numericInput, memoryIndicator);

    // default portrait layout
    Page {
        id: page
        title: i18n.tr("Calculator")
        anchors.fill: parent

        Flickable {
            anchors.fill: parent
            contentWidth: column.width
            contentHeight: column.height
            Column {
                id: column
                width: page.width
                height: childrenRect.height
                spacing: units.gu(0.5)
                TextField {
                    id: numericInput
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    readOnly: true
                    font.pixelSize: FontUtils.sizeToPixels("x-large")
                    font.bold: true
                    horizontalAlignment: Text.AlignRight
                    height: layout.currentLayout === "landscape" ?
                                units.gu(5) : units.gu(7)
                    primaryItem: Label {
                        id: memoryIndicator
                        width: units.gu(3)
                        height: parent.height
                        text: "M"
                        textSize: Label.Large
                        font.bold: true
                    }
                }

                Layouts {
                    id: layout
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: page.height - numericInput.height - mainView.header.height

                    layouts: [
                        ConditionalLayout {
                            name: "landscape"
                            when: mainView.width > mainView.height
                            Item {
                                anchors.fill: parent
                                Functions {
                                    operations: calculator
                                    anchors.fill: parent
                                    anchors.rightMargin: parent.width / 2 + units.gu(0.25)
                                    spacing: units.gu(0.4)
                                }

                                ItemLayout {
                                    item: "buttons"
                                    anchors.fill: parent

                                    anchors.leftMargin: parent.width / 2 + units.gu(0.25)
                                }
                            }
                        }

                    ]
                    DefaultLayout {
                        anchors.fill: parent
                        anchors.margins: units.gu(0.5)
                        operations: calculator
                        spacing: (layout.currentLayout === "landscape") ? units.gu(0.4) : units.gu(2)
                    }
                }
            }
        }
    }
}
