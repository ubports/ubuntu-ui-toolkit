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
import "fontUtils.js" as FontUtils
import "." 0.1 as Theming

/*!
    \qmltype TextCustom
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The TextCustom class is DOCME

    \b{This component is under heavy development.}
*/
Text {
    color: "black"
//    font.family: "UbuntuBeta"

    /*!
       \preliminary
       DOCME
    */
    property string fontSize: "medium"
    font.pixelSize: FontUtils.sizeToPixels(fontSize)

    // the following line instructs ItemStyle attached properties to delay
    // applying the style
    Component.onCompleted: {}
    // apply styling
    Theming.ItemStyle.onStyleChanged: {
        // define the new binding till we get the style to apply the theme
        // properties straight on the Item!
        color = Qt.binding(function () {
                               if (Theming.ItemStyle.style && Theming.ItemStyle.style.color)
                                   return Theming.ItemStyle.style.color
                               return "black"
                           })
        fontSize = Qt.binding(function() {
                                 if (Theming.ItemStyle.style && Theming.ItemStyle.style.fontSize)
                                     return Theming.ItemStyle.style.fontSize
                                 return "medium"
                             })
        if (Theming.ItemStyle.style && Theming.ItemStyle.style.style) {
            style = Qt.binding(function() {
                               return Theming.ItemStyle.style.style
                               })
        }
        if (Theming.ItemStyle.style && Theming.ItemStyle.style.styleColor) {
            styleColor = Qt.binding(function() {
                               return Theming.ItemStyle.style.styleColor
                               })
        }
    }
}
