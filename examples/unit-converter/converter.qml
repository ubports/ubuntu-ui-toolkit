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

  The application demonstrates the usage of
    - i18n
    - units
    - Label
    - Tabs
    - Tab
    - TextField and
    - Button components

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

    // Title label styled using "title" style class
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

    // Length conversion model; the unit is Mile
    property var lengthModel: [
        {"unit": "Inch", "rate": 63360.0},
        {"unit": "Meter", "rate": 1609.344},
        {"unit": "Miles", "rate": 1.0},
        {"unit": "Feets", "rate": 5280.0},
        {"unit": "Yards", "rate": 1760.0},
        {"unit": "Kilometers", "rate": 1.609344},
    ]

    // Weight conversion model; the base unit is Pound
    property var weightModel: [
        {"unit": "Pounds", "rate": 1.0},
        {"unit": "Kilograms", "rate": 0.45359237},
        {"unit": "Ounces", "rate": 16},
        {"unit": "Stones", "rate": 0.0714285714},
        {"unit": "US Tons", "rate": 0.0005},
        {"unit": "UK Tons", "rate": 0.000446428571},
    ]

    // Page layout containing rows od Label showing the unit and an attached
    // TextField for conversion
    Component {
        id: pageContent
        Column {
            id: pageLayout
            // expose Repeater's model for reusability, so we can set it from
            // outside, when we build the tabs
            property alias model: converter.model

            anchors {
                fill: parent
                margins: root.margins
            }
            spacing: units.gu(1)
            // show as many lines as many units we have in the model
            // it is assumed that the model has "unit" and "rate" roles
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
                    // input field performing conversion
                    TextField {
                        id: input
                        errorHighlight: false
                        validator: DoubleValidator {notation: DoubleValidator.StandardNotation}
                        width: pageLayout.width - 2 * root.margins - root.labelWidth
                        text: "0.0"
                        font.pixelSize: FontUtils.sizeToPixels("large")
                        height: units.gu(4)
                        // on-the-fly conversion
                        onTextChanged: if (activeFocus) convert(input, index)
                    }
                }
            }
            // clear content
            Button {
                text: i18n.tr("Clear")
                width: units.gu(12)
                onClicked: clear()
            }

            // the function converts the value entered in the input to the other units
            function convert(input, index) {
                if (input && input.text.length <= 0)
                    return;
                var baseValue = parseFloat(input.text) / parseFloat(converter.model[index].rate);
                if (baseValue <= 0.0)
                    return;
                for (var i = 0; i < converter.count; i++) {
                    if (converter.itemAt(i)) {
                        var inputItem = converter.itemAt(i).children[1]
                        if (inputItem !== input) {
                            inputItem.text = parseFloat(converter.model[i].rate) * baseValue;
                        }
                    }
                }
            }

            // the function clears every input field data
            function clear(except) {
                for (var i = 0; i < converter.count; i++) {
                    if (!except || (except && except !== converter.itemAt(i).children[1]))
                        converter.itemAt(i).children[1].text = "0.0";
                }
            }
        }
    }

    // Tabs using "new-tabs" Look and Feel (scrolling tab-bar and flickable tabs).
    Tabs {
        // remove the following line to use tab-strips, the old-style tabs
        ItemStyle.class: "new-tabs"
        anchors {
            left: parent.left
            right: parent.right
            top: title.bottom
            bottom: parent.bottom
            margins: units.gu(0.5)
        }

        // Tab for Length conversions
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

        // Tab for Weight conversions
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
