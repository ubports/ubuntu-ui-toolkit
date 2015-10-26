/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3

/*!
    \qmltype BottomEdgeHint
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \inherits StyledItem
    \brief The BottomEdgeHint shows the availability of extra features
    available from the bottom edge of the application.

    It displays either a label or an icon at the bottom of the application.

    It has 4 states: Hidden, Idle, Active and Locked. When Idle, part of it is
    still visible hinting the existence of the bottom edge.

    When used with a mouse it acts like a button. The typical action associated
    with clicking on it should be revealing the extra features provided by the
    bottom edge. However, the click can only happen if the hint is in \e Locked
    state.

    Example:
    \qml
    BottomEdgeHint {
        id: bottomEdgeHint
        text: i18n.tr("Favorites")
        onClicked: revealBottomEdge()
    }
    \endqml

    The component is styled through \b BottomEdgeHintStyle.
*/
StyledItem {
    id: bottomEdgeHint

    anchors.bottom: parent.bottom

    /*!
       This handler is called when there is a mouse click on the BottomEdgeHint
       and the BottomEdgeHint is not disabled.
    */
    signal clicked()

    Keys.onEnterPressed: clicked()
    Keys.onReturnPressed: clicked()

    /*!
      \qmlproperty string text
      The label displayed by the BottomEdgeHint.
     */
    property string text

    /*!
      \qmlproperty url iconSource
      The icon displayed by the BottomEdgeHint.

      This is the URL of any image file.
      If both iconSource and iconName are defined, iconName will be ignored.
     */
    property url iconSource

    /*!
      \qmlproperty string iconName
      The icon associated with the BottomEdgeHint in the icon theme.

      If both iconSource and iconName are defined, iconName will be ignored.
     */
    property string iconName

    /*!
      The property holds the flickable, which when flicked hides the hint.
      \e Hidden state is reached when this property is set to a Flickable
      which is flicking or moving. It is recommended to set the property
      when the hint is placed above a flickable content. Defaults to null.
      */
    property Flickable flickable: null

    /*!
      \qmlproperty string state
      BottomEdgeHint can take 4 states of visibility: "Hidden", "Idle", "Active"
      and "Locked".

      When \e Hidden, the hint is not shown at all. When \e Active, the full hint
      with its content is shown. When \e Idle, only part of the hint is visible
      leaving more space for application content. \e Idle extends the empty state.
      \e Locked is similar to Active, except that it is a final state, meaning the
      hint will be shown no matter of the flickable's status.

      Defaults to \e Idle.
     */
    state: "Idle"

    styleName: "BottomEdgeHintStyle"
}
