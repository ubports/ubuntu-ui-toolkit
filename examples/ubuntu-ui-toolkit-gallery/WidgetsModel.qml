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

ListModel {
    // Already in design guidelines
    ListElement {
        label: "Navigation"
        source: "Navigation.qml"
    }

    ListElement {
        label: "Toggles"
        source: "Toggles.qml"
    }
    ListElement {
        label: "Buttons"
        source: "Buttons.qml"
    }
    ListElement {
        label: "Slider"
        source: "Sliders.qml"
    }
    ListElement {
        label: "Text Field"
        source: "TextInputs.qml"
    }

    // Not in design guidelines yet
    ListElement {
        label: "Progress and activity"
        source: "ProgressBars.qml"
    }
    ListElement {
        label: "Ubuntu Shape"
        source: "UbuntuShape.qml"
    }
    ListElement {
        label: "Icons"
        source: "Icons.qml"
    }
    ListElement {
        label: "Label"
        source: "Label.qml"
    }

    // Already in design guidelines but should be reordered
    ListElement {
        label: "List Items"
        source: "ListItems.qml"
    }
    ListElement {
        label: "Dialog"
        source: "Dialog.qml"
    }
    ListElement {
        label: "Popover"
        source: "Popover.qml"
    }
    ListElement {
        label: "Sheet"
        source: "Sheet.qml"
    }

    // Not in design guidelines yet
    ListElement {
        label: "Animations"
        source: "Animations.qml"
    }
}
