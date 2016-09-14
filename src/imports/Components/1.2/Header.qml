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

import QtQuick 2.4

/*!
    \qmltype Header
    \internal
    \deprecated
*/
AppHeader {

    /*!
      \internal
      We need this property so QML exposes this class as Header instead of
      AppHeader. This way autopilot can select the deprecated header.
    */
    property string _for_autopilot

    Component.onCompleted: {
        print("WARNING: Header is an internal component of Ubuntu.Components and" +
              "its API may change or be removed at any moment." +
              "Please use MainView and Page instead."
              );
    }
}
