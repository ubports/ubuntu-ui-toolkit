/*
    Copyright 2014 Canonical Ltd.
 *
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; version 3.
 *
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
 *
    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2

/*!
    \qmltype UbuntuColors
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief Singleton defining the Ubuntu color palette.

    Each color of the Ubuntu color palette is accesible from it.
    For more information on how to appropriately use the colors according to
    the context, please refer to the
    \l{http://design.ubuntu.com/brand/colour-palette}{Ubuntu brand guidelines}.

    Example of use:

    \qml
    Rectangle {
       color: UbuntuColors.orange
    }
    \endqml
*/
QtObject {
    // old colors from UbuntuColors 1.0:

    /*!
       Ubuntu core color: Ubuntu orange
    */
    readonly property color orange: "#DD4814"
    /*!
       Ubuntu supporting color: light aubergine
    */
    readonly property color lightAubergine: "#77216F"
    /*!
       Ubuntu supporting color: mid aubergine
    */
    readonly property color midAubergine: "#5E2750"
    /*!
       Ubuntu supporting color: dark aubergine
    */
    readonly property color darkAubergine: "#2C001E"
    /*!
       Ubuntu neutral color: warm grey
    */
    readonly property color warmGrey: "#AEA79F"
    /*!
       Ubuntu neutral color: cool grey
    */
    readonly property color coolGrey: "#333333"

    /*!
       Ubuntu orange gradient

       \sa Button::gradient
    */
    property Gradient orangeGradient: Gradient {
        GradientStop { position: 0.0; color: "#DD4814" }
        GradientStop { position: 1.0; color: "#D9722D" }
    }
    /*!
       Ubuntu grey gradient

       \sa Button::gradient
    */
    property Gradient greyGradient: Gradient {
        GradientStop { position: 0.0; color: "#888888" }
        GradientStop { position: 1.0; color: "#BBBBBB" }
    }

    // New colors introduced in UbuntuColors 1.1:

    /*!
      Light grey
      \since Ubuntu.Components 1.1
     */
    readonly property color lightGrey: "#888888"

    /*!
      Dark grey
      \since Ubuntu.Components 1.1
     */
    readonly property color darkGrey: "#5d5d5d"

    /*!
      Red
      \since Ubuntu.Components 1.1
     */
    readonly property color red: "#fc4949"

    /*!
      Green
      \since Ubuntu.Components 1.1
     */
    readonly property color green: "#3fb24f"

    /*!
      Blue
      \since Ubuntu.Components 1.1
     */
    readonly property color blue: "#1ab6ef"

    /*!
      Aubergine
      \since Ubuntu.Components 1.1
     */
    readonly property color aubergine: "#762572"
}
