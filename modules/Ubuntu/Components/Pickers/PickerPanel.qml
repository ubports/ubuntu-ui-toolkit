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
import QtQuick.Window 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

/*!
    \qmltype DateTimePanel
    \inqmlmodule Ubuntu.Components.Pickers 0.1
    \ingroup ubuntu-pickers
    \brief Provides a panel for opening a Date picker in place of the input panel or
    as Popover, depending on the form factor.
  */

Object {
    /*!
      The function opens a date picker panel. The date picker is opened in input
      panel area if there is input panel available and the form factor describes
      a phone. The picked date will be read from and reported into the \a property
      of the \a caller as date type. This implies that the caller must have
      defined a property with date type.
      */
    function openDatePicker(caller, property, mode) {
        if (mode === undefined) {
            mode = "Years|Months|Days";
        }
        var params = {
            "date": caller[property],
            "mode": mode,
            "dateProperty": property
        }

        if (!internal.isPhone || QuickUtils.inputMethodProvider === "") {
            // we have no input panel defined, or the therefore we show the picker in a Popover
            return internal.openPanel(datePickerPopover, caller, params);
        }
        // OSK panel
        return internal.openPanel(datePickerPanel, caller, params);
    }

    /*!
      The function opens a month picker panel. The date picker is opened in input
      panel area if there is input panel available and the form factor describes a phone.
      */
    function monthPicker(caller, property) {
        if (!internal.isPhone || QuickUtils.inputMethodProvider === "") {
            // we have no input panel defined, or the therefore we show the picker in a Popover
            return internal.openPanel(datePickerPopover, caller,
                                      {"date": caller[property], "mode": "Years|Months", "dateProperty": property});
        }
        // OSK panel
        return internal.openPanel(datePickerPanel, caller,
                                  {"date": caller[property], "mode": "Years|Months", "dateProperty": property});
    }

    QtObject {
        id: internal

        property bool isPhone: Screen.width <= units.gu(40) && Screen.height <= units.gu(71)

        function openPanel(component, caller, params) {
            var panel = PopupUtils.open(component, caller, params);
            panel.parent = QuickUtils.rootItem(null);
            return panel;
        }
    }

    // popover
    Component {
        id: datePickerPopover
        Popover {
            property alias date: picker.date
            property alias mode: picker.mode
            property string dateProperty

            contentWidth: frame.width
            contentHeight: frame.height
            //FIXME: set the maximum width possible for the DatePicker
            Rectangle {
                id: frame
                width: units.gu(46)
                height: picker.height + units.gu(4)
                color: Qt.rgba(0, 0, 0, 0.02)
                DatePicker {
                    id: picker
                    width: frame.width
                    anchors.centerIn: parent
                }
            }

            Binding {
                target: caller
                property: dateProperty
                when: dateProperty != undefined
                value: picker.date
            }
        }
    }

    // OSK panel
    Component {
        id: datePickerPanel
        PopupBase {
            property alias date: picker.date
            property alias mode: picker.mode
            property string dateProperty

            __foreground: panel
            Rectangle {
                id: panel
//                width: Qt.inputMethod.keyboardRectangle.width
//                height: Qt.inputMethod.keyboardRectangle.height
                width: units.gu(40); height: units.gu(30)
                DatePicker {
                    id: picker
                    anchors {
                        fill: parent
                        margins: units.gu(2)
                    }
                }
            }
        }
    }
}
