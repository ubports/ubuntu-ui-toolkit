/*
    Copyright 2016 Canonical Ltd.
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

pragma Singleton
import QtQuick 2.4

/*!
    \qmltype UbuntuColors
    \inqmlmodule Ubuntu.Components
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

    /*---------------------------------------------------------------
      Deprecated colors
      ---------------------------------------------------------------*/
    /*!
      \deprecated
      Ubuntu supporting color: light aubergine
    */
    readonly property color lightAubergine: "#77216F"
    /*!
      \deprecated
      Ubuntu supporting color: mid aubergine
    */
    readonly property color midAubergine: "#5E2750"
    /*!
      \deprecated
      Ubuntu supporting color: dark aubergine
    */
    readonly property color darkAubergine: "#2C001E"
    /*!
      \deprecated
      Ubuntu neutral color: warm grey
    */
    readonly property color warmGrey: "#AEA79F"
    /*!
      \deprecated
      Ubuntu neutral color: cool grey
    */
    readonly property color coolGrey: "#333333"

    /*!
      Ubuntu orange gradient
      \deprecated
      \sa Button::gradient
    */
    property Gradient orangeGradient: Gradient {
        GradientStop { position: 0.0; color: "#DD4814" }
        GradientStop { position: 1.0; color: "#D9722D" }
    }
    /*!
      Ubuntu grey gradient
      \deprecated
      \sa Button::gradient
    */
    property Gradient greyGradient: Gradient {
        GradientStop { position: 0.0; color: "#888888" }
        GradientStop { position: 1.0; color: "#BBBBBB" }
    }

    /*---------------------------------------------------------------
      Supported colors
      ---------------------------------------------------------------*/
    /*!
      Orange. Recommended for branded elements, display progress
      and intensity, textual links on light backgrounds.
    */
    readonly property color orange: "#E95420"

    /*!
      \deprecated
      Light grey. Recommended for neutral action buttons and
      secondary text.
      \since Ubuntu.Components 1.1
     */
    readonly property color lightGrey: "#cdcdcd"

    /*!
      Silk. Recommended for neutral action buttons and
      secondary text.
      \since Ubuntu.Components 1.3
     */
    readonly property color silk: "#cdcdcd"

    /*!
      \deprecated
      Dark grey. Recommended for text and action icons.
      \since Ubuntu.Components 1.1
     */
    readonly property color darkGrey: "#5d5d5d"

    /*!
      Slate. Recommended for text and action icons.
      \since Ubuntu.Components 1.3
     */
    readonly property color slate: "#5d5d5d"

    /*!
      Red. Recommended for negative and irreversible action
      buttons, errors and alerts.
      \since Ubuntu.Components 1.1
     */
    readonly property color red: "#ed3146"

    /*!
      Green. Recommended for positive action buttons.
      \since Ubuntu.Components 1.1
     */
    readonly property color green: "#3eb34f"

    /*!
      Blue. Recommended for text selection and text cursor for Ambiance theme.
      \since Ubuntu.Components 1.3
     */
    readonly property color blue: "#335280"

    /*!
      Blue. Recommended for text selection and text cursor for SuruDark theme.
      \since Ubuntu.Components 1.1
     */
    readonly property color lightBlue: "#19b6ee"

    /*!
      Purple. Recommended for proper nouns in
      list items.
      \since Ubuntu.Components 1.1
     */
    readonly property color purple: "#762572"

    /*!
      Porcelain. Recommended for foregrounds.
      \since Ubuntu.Components 1.3
      */
    readonly property color porcelain: "#f7f7f7"

    /*!
      Ash. Recommended for subtitles and other tertiary content.
      \since Ubuntu.Components 1.3
      */
    readonly property color ash: "#888888"

    /*!
      Graphite. Recommended for coloring dark themes' background.
      \since Ubuntu.Components 1.3
      */
    readonly property color graphite: "#666666"

    /*!
      Inkstone. Recommended for foreground colors in dark themes.
      \since Ubuntu.Components 1.3
      */
    readonly property color inkstone: "#3b3b3b"

    /*!
      Jet. Recommended for content coloring.
      \since Ubuntu.Components 1.3
      */
    readonly property color jet: "#111111"
}
