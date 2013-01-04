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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

/*!
    \qmltype CheckBox
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief CheckBox is a component with two states, checked or unchecked. It can
    be used to set boolean options. The behavior is the same as \l Switch, the
    only difference is the graphical style.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        CheckBox {
            checked: true
        }
    }
    \endqml
*/
AbstractButton {
    id: checkBox

//    width: border.width
//    height: border.height

    // default width and height depends on the delegate width and height
//    width: childrenRect.width
//    height: childrenRect.height

    width: units.gu(6)
    height: units.gu(6)
    // FIXME: see FIXME above
    Theming.ItemStyle.class: "checkbox"


    /*!
      \preliminary
      Specifies whether the checkbox is checked or not. By default the property
      is set to false.
    */
    property bool checked: false

    /*!
      \internal
     */
    onClicked: checked = !checked

}
