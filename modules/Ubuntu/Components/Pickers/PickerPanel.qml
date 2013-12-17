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
    \brief Provides a panel for opening a DatePicker in place of the input panel or
    as Popover, depending on the form factor.

    PickerPanel is a singleton component designed to open a DatePicker in input panel
    area or in a Popover, depending on the form factor, following the design guides
    on date pickers.
  */

Object {

    /*!
      \qmlproperty bool closePanelOnDismissAreaPress
      The property drives whether the panel/popover should be closed when the
      user presses on the panel's inactive area.
      The default value is true.
      */
    property alias closePanelOnDismissAreaPress: internal.closePanelOnDismissAreaPress

    /*!
      The function opens a date picker panel. The date picker is opened in input
      panel area if there is on-screen input panel available and the form factor
      describes a phone. The picked date will be read from and reported into the
      \a property of the \a caller as date type. This implies that the caller
      must have defined a property with date type.

      On success the returned object is either a Popover or a PopupBase, with the
      following additional properties:
      \table
        \header
            \li Property
            \li Description
        \row
            \li \b picker
            \li instance of the DatePicker component shown in the panel/popup
        \row
            \li \b date
            \li
        \row
            \li \b mode
            \li
        \row
            \li \b dateProperty
            \li

      \endtable
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

        if (!internal.isPhone()) {
            // we have no input panel defined, or the therefore we show the picker in a Popover
            return internal.openPanel(datePickerPopover, caller, params);
        }
        // OSK panel
        return internal.openPanel(datePickerPanel, caller, params);
    }

    QtObject {
        id: internal

        property bool closePanelOnDismissAreaPress: true
        property bool formFactorPhone: Screen.width <= units.gu(40) && Screen.height <= units.gu(71)

        function isPhone() {
//            return (formFactorPhone && QuickUtils.inputMethodProvider !== "");
            return formFactorPhone;
        }

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
            property alias picker: picker
            property alias date: picker.date
            property alias mode: picker.mode
            property string dateProperty

            contentWidth: frame.width
            contentHeight: frame.height
            __closeOnDismissAreaPress: internal.closePanelOnDismissAreaPress
            //FIXME: set the maximum width possible for the DatePicker
            Rectangle {
                id: frame
                width: picker.width + units.gu(4)
                height: picker.height + units.gu(4)
                color: Qt.rgba(0, 0, 0, 0.02)
                DatePicker {
                    id: picker
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
            property alias picker: picker
            property alias date: picker.date
            property alias mode: picker.mode
            property string dateProperty

            __foreground: panel
            __closeOnDismissAreaPress: internal.closePanelOnDismissAreaPress
            Rectangle {
                id: panel
                width: Qt.inputMethod.keyboardRectangle.width > 0 ? Qt.inputMethod.keyboardRectangle.width : units.gu(40)
                height: Qt.inputMethod.keyboardRectangle.height > 0 ? Qt.inputMethod.keyboardRectangle.height : units.gu(30)
                y: dismissArea.height

                states: [
                    State {
                        name: "opened"
                        when: visible
                        PropertyChanges {
                            target: panel
                            y: dismissArea.height - height;
                        }
                    }
                ]
                transitions: [
                    Transition {
                        from: "*"
                        to: "opened"
                        reversible: true
                        UbuntuNumberAnimation {
                            target: panel
                            property: "y"
                        }
                    }

                ]
                DatePicker {
                    id: picker
                    anchors {
                        fill: panel
                        margins: units.gu(2)
                    }
                }
            }
        }
    }
}
