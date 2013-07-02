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
import Ubuntu.Unity.Action 1.0 as UnityActions

/*!
    \qmltype Action
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Describe an action that can be re-used in, for example a \l Button

    \b{This component is under heavy development.}

    Examples: See \l Page
*/
//QtObject {
UnityActions.Action {
    id: action

    /*!
      The title of the action.
      \qmlproperty string text
     */
//    property string text // now in unity action

    /*!
      The image associated with the action.
      \qmlproperty url iconSource
     */
    // TODO: Move iconSource to unity action if possible
    property url iconSource

    /*!
      Called when the action is triggered.
     */
//    signal triggered(var caller) // now in unity action. still needs some testing.

    /*!
      \deprecated
      \b {visible is DEPRECATED. Use \l ActionItem to specify the representation of an \l Action.}
      The action is visible to the user
     */
    property bool visible: true
    /*! \internal */
    onVisibleChanged: print("Action.visible is a DEPRECATED property. Use ActionItems to specify the representation of an Action.")

    /*!
      Enable the action. It may be visible, but disabled.
      \qmlproperty bool enabled
     */
//    property bool enabled: true // moved to unity action

    /*!
      \deprecated
      \b {itemHint is DEPRECATED. Use \l ActionItem to specify
      the representation of an \l Action.}
      Proposed Component to use as a representation for this action.
      Depending on the component that displays the action, the Component
      given here can replace the default representation of the action.
     */
    property Component itemHint
    /*! \internal */
    onItemHintChanged: print("Action.itemHint is a DEPRECATED property. Use ActionItems to specify the representation of an Action.")
}
