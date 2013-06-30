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
    \brief Standard Ubuntu button.

    \l {http://design.ubuntu.com/apps/building-blocks/buttons}{See also the Design Guidelines on Buttons}.

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
    An \l Action can be used to specify \b clicked, iconSource and text. Example:
    \qml
        Item {
            Action {
                id: action1
                text: "Click me"
                onTriggered: print("action!")
                iconSource: "icon.png"
            }
            Button {
                anchors.centerIn: parent
                action: action1
                color: "green"
            }
       }
    \endqml
*/
AbstractButton {
    id: button

    // FIXME: see FIXME above
    Theming.ItemStyle.class: "button"
    implicitWidth: Theming.ItemStyle.delegate.implicitWidth
    implicitHeight: Theming.ItemStyle.delegate.implicitHeight

    /*!
       The background color of the button.
    */
    property color color: "#DD4814"

    /*!
       The source URL of the icon to display inside the button.
       Leave this value blank for a text-only button.
       If \l action is set, the default iconSource is that of the action.
       \qmlproperty url iconSource
    */

    /*!
       The text to display in the button. If an icon was defined,
       the text will be shown next to the icon, otherwise it will
       be centered. Leave blank for an icon-only button.
       If \l action is set, the default text is that of the action.
       \qmlproperty string text
    */

    /*!
       The position of the icon relative to the text. Options
       are "left" and "right". The default value is "left".

       If only text or only an icon is defined, this
       property is ignored and the text or icon is
       centered horizontally and vertically in the button.

       Currently this is a string value. We are waiting for
       support for enums:
       https://bugreports.qt-project.org/browse/QTBUG-14861
    */
    property string iconPosition: "left"
}
