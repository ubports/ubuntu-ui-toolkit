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

import QtQuick 2.0

// Documentation is part of the Page documentation in Page11.qdoc
QtObject {
    // To be used inside a Page only.
    id: headerConfig
    /*!
          \qmlproperty headerConfig.actions
          List of actions to show in the header.
         */
    property var actions: []

    /*!
          \qmlproperty headerConfig.backAction
          Overrides the default \l PageStack back button and the
          \l Tabs drawer button in the header.
         */
    property Action backAction: null

//    readonly property TextField input: parent.__propagated.header.input
}
