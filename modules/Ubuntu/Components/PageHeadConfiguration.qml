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

/*!
    \qmltype PageHeadConfiguration
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \since Ubuntu.Components 1.1
    \brief Page.head is used to configure the header for a \l Page.

    For examples how to use Page.head, see \l Page.
 */
QtObject {
    // To be used inside a Page only.
    id: headerConfig

    /*!
      List of actions to show in the header.
     */
    property list<Action> actions

    /*!
      Overrides the default \l PageStack back button and the
        \l Tabs drawer button in the header.
     */
    property Action backAction: null
}
