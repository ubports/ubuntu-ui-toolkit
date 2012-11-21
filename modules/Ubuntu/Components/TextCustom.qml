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
    \qmltype TextCustom
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The TextCustom class is DOCME

    \b{This component is under heavy development.}
*/
Text {
    /*!
       \preliminary
       DOCME
    */
    property string fontSize: (Theming.ItemStyle.style && Theming.ItemStyle.style.fontSize) ?
                                Theming.ItemStyle.style.fontSize : "medium"

    Theming.ItemStyle.class: "textcustom"

    color: (Theming.ItemStyle.style && Theming.ItemStyle.style.color) ?
             Theming.ItemStyle.style.color : "black"

    elide: (Theming.ItemStyle.style && Theming.ItemStyle.style.elide) ?
             Theming.ItemStyle.style.elide : Text.ElideNone

    font.bold: (Theming.ItemStyle.style && Theming.ItemStyle.style.bold) ?
                 Theming.ItemStyle.style.bold : false

    font.capitalization: (Theming.ItemStyle.style && Theming.ItemStyle.style.capitalization) ?
                           Theming.ItemStyle.style.capitalization : Font.MixedCase

    font.family: (Theming.ItemStyle.style && Theming.ItemStyle.style.fontFamily) ?
                   Theming.ItemStyle.style.fontFamily : ""

    font.italic: (Theming.ItemStyle.style && Theming.ItemStyle.style.italic) ?
                   Theming.ItemStyle.style.italic : false

    font.letterSpacing: (Theming.ItemStyle.style && Theming.ItemStyle.style.letterSpacing) ?
                          Theming.ItemStyle.style.letterSpacing : 1.0

    font.pixelSize: FontUtils.sizeToPixels(fontSize)

    font.strikeout: (Theming.ItemStyle.style && Theming.ItemStyle.style.strikeout) ?
                      Theming.ItemStyle.style.strikeout : false

    font.underline: (Theming.ItemStyle.style && Theming.ItemStyle.style.underline) ?
                      Theming.ItemStyle.style.underline : false

    font.weight: (Theming.ItemStyle.style && Theming.ItemStyle.style.weight) ?
                   Theming.ItemStyle.style.weight : Font.Normal

    font.wordSpacing: (Theming.ItemStyle.style && Theming.ItemStyle.style.wordSpacing) ?
                        Theming.ItemStyle.style.wordSpacing : 0.0

    horizontalAlignment: (Theming.ItemStyle.horizontalAlignment && Theming.ItemStyle.horizontalAlignment.horizontalAlignment) ?
                           Theming.ItemStyle.horizontalAlignment.horizontalAlignment : Text.AlignHLeft

    lineHeight: (Theming.ItemStyle.lineHeight && Theming.ItemStyle.lineHeight.lineHeight) ?
                  Theming.ItemStyle.lineHeight.lineHeight : 1.0

    lineHeightMode: (Theming.ItemStyle.lineHeightMode && Theming.ItemStyle.lineHeightMode.lineHeightMode) ?
                      Theming.ItemStyle.lineHeightMode.lineHeightMode : Text.ProportionalHeight

    maximumLineCount: (Theming.ItemStyle.style && Theming.ItemStyle.style.maximumLineCount) ?
                        Theming.ItemStyle.style.maximumLineCount : undefined

    style: (Theming.ItemStyle.style && Theming.ItemStyle.style.style) ?
             Theming.ItemStyle.style.style : Text.Normal

    styleColor: (Theming.ItemStyle.style && Theming.ItemStyle.style.styleColor) ?
                  Theming.ItemStyle.style.styleColor : color

    wrapMode: (Theming.ItemStyle.style && Theming.ItemStyle.style.wrapMode) ?
                Theming.ItemStyle.style.wrapMode : Text.NoWrap

    opacity: (Theming.ItemStyle.style && Theming.ItemStyle.style.textOpacity) ?
               Theming.ItemStyle.style.textOpacity : 1.0

    verticalAlignment: (Theming.ItemStyle.verticalAlignment && Theming.ItemStyle.verticalAlignment.verticalAlignment) ?
                         Theming.ItemStyle.verticalAlignment.verticalAlignment : Text.AlignVTop
}
