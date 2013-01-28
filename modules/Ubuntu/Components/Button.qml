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
    \qmltype Button
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The Button class is DOCME

    \b{This component is under heavy development.}

    Examples:
    \qml
        Column {
            Button {
                text: "Send"
                onClicked: print("clicked text-only Button")
            }
            Button {
                iconSource: "icon.png"
                color: "green"
                onClicked: print("clicked icon-only Button")
            }
            Button {
                iconSource: "icon.png"
                text: "Icon on left"
                iconPosition: "left"
                onClicked: print("clicked text and icon Button")
            }
        }
    \endqml
*/
AbstractButton {
    id: button

    width: Theming.ComponentUtils.delegateProperty(button, "implicitWidth", units.gu(4))
    height: Theming.ComponentUtils.delegateProperty(button, "implicitHeight", units.gu(4))

    // FIXME: see FIXME above
    Theming.ItemStyle.class: "button"

    /*!
       \preliminary
       The foreground color of the button in idle state.
    */
    // FIXME: see FIXME above
    property color color: Theming.ComponentUtils.style(button, "color", "transparent")

    /*!
       \preliminary
       The source URL of the icon to display inside the button.
       Leave this value blank for a text-only button.
       \qmlproperty url iconSource
    */
    property url iconSource: Theming.ComponentUtils.style(button, "iconSource", "")

    /*!
       \preliminary
       The text to display in the button. If an icon was defined,
       the text will be shown next to the icon, otherwise it will
       be centered. Leave blank for an icon-only button.
       \qmlproperty string text
    */
    property string text

    /*!
       \preliminary

       The position of the icon relative to the text. Options
       are "left" and "right". The default value is "left".

       If only text or only an icon is defined, this
       property is ignored and the text or icon is
       centered horizontally and vertically in the button.

       Currently this is a string value. We are waiting for
       support for enums:
       https://bugreports.qt-project.org/browse/QTBUG-14861
    */
    property string iconPosition: Theming.ComponentUtils.style(button, "iconPosition", "left")
}
