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

// FIXME(loicm) It would be nice to have a draggable thumb to toggle the switch,
//     that needs to be validated with designers though.

import QtQuick 2.0

// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

/*!
    \qmltype Switch
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Switch is a component with two states, checked or unchecked. It can
    be used to set boolean options. The behavior is the same as \l CheckBox, the
    only difference is the graphical style.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        Switch {
            checked: true
        }
    }
    \endqml
*/
AbstractButton {

    // FIXME(loicm) switch is a reserved keyword. Shouldn't we specify a prefix
    //     as a guideline to prevent that?
    id: sweetch


    // FIXME: see FIXME above
    Theming.ItemStyle.class: "switch"
    width: Theming.ComponentUtils.delegateProperty(sweetch, "implicitWidth", units.gu(8))
    height: Theming.ComponentUtils.delegateProperty(sweetch, "implicitHeight", units.gu(4))

    //    width: units.gu(8)
    //    height: units.gu(4)
//    width: backgroundShape.width
//    height: backgroundShape.height


    /*!
      \preliminary
      Specifies whether the switch is checked or not. By default the property
      is set to false.
    */
    property bool checked: false

    /*!
      \internal
     */
    onClicked: sweetch.checked = !sweetch.checked
}
