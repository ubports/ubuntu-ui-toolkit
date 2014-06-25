/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2

// TODO: Document
State {
    id: state

    /*!
      The head property of the \l Page which will be the target of
      the property changes of this State. This property must always
      be set before the State can be used.
     */
    property HeaderConfiguration head

    /*!
      The actions to be shown in the header with this state.
     */
    property list<Action> actions

    /*!
      The back action for this state.
     */
    property Action backAction

    property TextField input

    PropertyChanges {
        target: state.head
        backAction: state.backAction
        actions: state.actions
        input: state.input
    }
}
