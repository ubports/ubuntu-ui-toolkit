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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

/*!
    \qmltype Label
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The Label class is DOCME

    \b{This component is under heavy development.}
*/
Text {
    id: label

    Theming.ItemStyle.class: "label"

    /*!
       \preliminary
       DOCME
    */
    property string fontSize: Theming.ComponentUtils.style(label, "fontSize", "medium")

    color: Theming.ComponentUtils.style(label, "color", "black")

    elide: Theming.ComponentUtils.style(label, "elide", Text.ElideNone)

    font.capitalization: Theming.ComponentUtils.style(label, "capitalization", Font.MixedCase)

    font.family: Theming.ComponentUtils.style(label, "fontFamily", "Ubuntu")

    font.italic: Theming.ComponentUtils.style(label, "italic", false)

    font.letterSpacing: Theming.ComponentUtils.style(label, "letterSpacing", 0.0)

    font.pixelSize: FontUtils.sizeToPixels(fontSize)

    font.strikeout: Theming.ComponentUtils.style(label, "strikeout", false)

    font.underline: Theming.ComponentUtils.style(label, "underline", false)

    font.wordSpacing: Theming.ComponentUtils.style(label, "wordSpacing", 0.0)

    horizontalAlignment: Theming.ComponentUtils.style(label, "horizontalAlignment", Text.AlignLeft)

    lineHeight: Theming.ComponentUtils.style(label, "lineHeight", 1.0)

    lineHeightMode: Theming.ComponentUtils.style(label, "lineHeightMode", Text.ProportionalHeight)

    maximumLineCount: Theming.ComponentUtils.style(label, "maximumLineCount")

    style: Theming.ComponentUtils.style(label, "style", Text.Normal)

    styleColor: Theming.ComponentUtils.style(label, "styleColor", color)

    wrapMode: Theming.ComponentUtils.style(label, "wrapMode", Text.NoWrap)

    opacity: Theming.ComponentUtils.style(label, "textOpacity", 1.0)

    verticalAlignment: Theming.ComponentUtils.style(label, "verticalAlignment", Text.AlignTop)

    visible: Theming.ComponentUtils.style(label, "visible", true)

    Theming.ItemStyle.onStyleChanged: {
        // only set font.bold and font.weight if they're defined in the style
        if (Theming.ComponentUtils.hasStyle(label, "bold")) {
            font.bold = Theming.ItemStyle.style.bold
        }
        if (Theming.ComponentUtils.hasStyle(label, "weight")) {
            font.weight = Theming.ItemStyle.style.weight
        }
    }
}
