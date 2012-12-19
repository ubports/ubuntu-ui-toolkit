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
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
/*!
  \brief Example unit converter application.

  The application demonstrates the usage of the Tabs, Tab, TextField and Button
  components.

  The application converts length and weight units between several metrics.
  Related units are grouped in the same page (Tab) and conversion happens when
  pressing Enter/Return after entering a number in one of the input fields (i.e.
  accepting the entered text), or by pressing the "Convert" button.

  The navigation between converter pages is provided by the Tabs component.
  */

Rectangle {
    id: root
    width: units.gu(60)
    height: units.gu(80)
    color: "lightgray"

    property real margins: units.gu(2)
    property real labelWidth: units.gu(12)

    Label {
        id: title
        ItemStyle.class: "title"
        text: i18n.tr("Unit Converter")
        height: paintedHeight + root.margins
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
    }

    property var lengthModel: [
        {"unit": "Inch", "rate": 63360.0},
        {"unit": "Meter", "rate": 1609.344},
        {"unit": "Miles", "rate": 1.0},
        {"unit": "Feets", "rate": 5280.0},
        {"unit": "Yards", "rate": 1760.0},
        {"unit": "Kilometers", "rate": 1.609344},
    ]

    property var weightModel: [
        {"unit": "Pounds", "rate": 1.0},
        {"unit": "Kilograms", "rate": 0.45359237},
        {"unit": "Ounces", "rate": 16},
        {"unit": "Stones", "rate": 0.0714285714},
        {"unit": "US Tons", "rate": 0.0005},
        {"unit": "UK Tons", "rate": 0.000446428571},
    ]

    Component {
        id: pageContent
        Column {
            id: pageLayout
            property alias model: converter.model

            anchors {
                fill: parent
                margins: root.margins
            }
            spacing: units.gu(1)
            Repeater {
                id: converter
                Row {
                    spacing: units.gu(1)
                    Label {
                        text: i18n.tr(modelData.unit)
                        fontSize: "large"
                        width: root.labelWidth
                        height: input.height
                        verticalAlignment: Text.AlignVCenter
                    }
                    TextField {
                        id: input
                        validator: DoubleValidator {notation: DoubleValidator.StandardNotation}
                        width: pageLayout.width - 2 * root.margins - root.labelWidth
                        text: "0.0"
                        font.pixelSize: FontUtils.sizeToPixels("large")
                        height: units.gu(4)
                        onAccepted: {
                            clear(input);
                            convert();
                        }
                    }
                }
            }
            Item {
                width: pageLayout.width
                height: childrenRect.height
                Button {
                    text: i18n.tr("Clear")
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    width: units.gu(12)
                    onClicked: clear()
                }
                Button {
                    text: i18n.tr("Convert")
                    anchors{
                        top: parent.top
                        right: parent.right
                    }
                    width: units.gu(12)
                    color: "green"
                    onClicked: convert()
                }
            }

            function convert() {
                var item;
                var baseValue = 0.0;
                for (var i = 0; !baseValue && i < converter.count; i++) {
                    item = converter.itemAt(i).children[1];
                    if (item.text !== "0.0") {
                        baseValue = parseFloat(item.text) / parseFloat(converter.model[i].rate);
                    }
                }
                if (baseValue > 0) {
                    for (i = 0; i < converter.count; i++) {
                        item = converter.itemAt(i).children[1];
                        item.text = parseFloat(converter.model[i].rate) * parseFloat(baseValue);
                    }
                }
            }

            function clear(except) {
                for (var i = 0; i < converter.count; i++) {
                    if (!except || (except && except !== converter.itemAt(i).children[1]))
                        converter.itemAt(i).children[1].text = "0.0";
                }
            }
        }
    }

    Tabs {
        ItemStyle.class: "new-tabs"
        anchors {
            left: parent.left
            right: parent.right
            top: title.bottom
            bottom: parent.bottom
            margins: units.gu(0.5)
        }

        Tab {
            title: i18n.tr("Lengths")
            property var model
            page: Loader {
                anchors.fill: parent
                sourceComponent: pageContent
                onStatusChanged: {
                    if (status == Loader.Ready && item) {
                        item.parent = parent;
                        item.model = lengthModel;
                    }
                }
            }
        }
        Tab {
            title: i18n.tr("Weights")
            property var model
            page: Loader {
                anchors.fill: parent
                sourceComponent: pageContent
                onStatusChanged: {
                    if (status == Loader.Ready && item) {
                        item.parent = parent;
                        item.model = weightModel;
                    }
                }
            }
        }
    }
}
