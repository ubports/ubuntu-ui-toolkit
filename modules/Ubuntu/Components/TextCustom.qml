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
    \qmlclass TextCustom
    \inqmlmodule Ubuntu.Components 0.1
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

    /*!
      \preliminary
      The property holds the style class information used by the widget.
      */
    property alias styleClass: styleRule.styleClass

    /*!
      \preliminary
      The property holds the instance ID information if the widget used in styling
      */
    property alias instanceId: styleRule.instanceId

    // example of private implementation of using StyledItem privately
    Theming.StyledItem {
        id: styleRule
        onStyleChanged: {
            if (style) {
                font.family = style.fontFamily
                color = style.color
                fontSize = style.fontSize
            }
        }
    }
}
