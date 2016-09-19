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

/*!
    \qmltype ActivityIndicator
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu
    \brief The ActivityIndicator component visually indicates that a task of
    unknown duration is in progress, e.g. busy indication, connection in progress
    indication, etc.

    Note: for time consuming JavaScript operations use WorkerScript, otherwise no
    UI interaction will be possible and the ActicityIndicator animation will freeze.

    Example:
    \qml
    Item {
        ActivityIndicator {
            id: activity
        }

        Button {
            id: toggleActive
            text: (activity.running) ? "Deactivate" : "Activate"
            onClicked: activity.running = !activity.running
        }
    }
    \endqml
*/
AnimatedItem {
    id: indicator

    /*!
       Presents whether there is activity to be visualized or not. The default value is false.
       When activated (set to true), an animation is shown indicating an ongoing activity, which
       continues until deactivated (set to false).
    */
    property bool running: false

    implicitWidth: units.gu(3)
    implicitHeight: units.gu(3)
    width: units.gu(3)
    height: units.gu(3)

    styleName: "ActivityIndicatorStyle"
}
