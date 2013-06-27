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

/*!
    \qmlabstract StyledItem
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The StyledItem class allows items to be styled by the theme.

    StyledItem provides facilities for making an Item stylable by the theme.

    In order to make an Item stylable by the theme, it is enough to make the Item
    inherit from StyledItem and set its \l style property to be the result of the
    appropriate call to Theme.createStyleComponent().

    Example definition of a custom Item MyItem.qml:
    \qml
        StyledItem {
            id: myItem
            style: Theme.createStyleComponent("MyItemStyle.qml", myItem)
        }
    \endqml

    The Component set on \l style is instantiated and placed below everything else
    that the Item contains. That instance is accessible via the \l delegate property.

    A reference to the Item being styled is accessible from the style and named
    'styledItem'.

    \sa {Theme}
*/
FocusScope {
    id: styledItem

    /*!
       Component instantiated immediately and placed below everything else.
    */
    property Component style

    /*!
       Instance of the \l style.
    */
    readonly property Item delegate: styleLoader.status == Loader.Ready ? styleLoader.item : null

    implicitWidth: delegate ? delegate.implicitWidth : 0
    implicitHeight: delegate ? delegate.implicitHeight : 0

    Loader {
        id: styleLoader
        anchors.fill: parent
        sourceComponent: style
        property Item styledItem: styledItem
    }
}
