/*
 * Copyright 2013 Canonical Ltd.
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
    \qmltype ProgressBar
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief The ProgressBar component visually indicates the progress of a process of
    determinate or indeterminate duration.

    \b{This component is under heavy development.}

    Example:
    \qml
    Item {
        ProgressBar {
            id: indeterminateBar
            indeterminate: true
        }

        ProgressBar {
            id: determinateBar
            minimumValue: -3.0
            maximumValue: 15
        }
    }
    \endqml
*/
ProgressBar {
    id: progressBar

    /*!
      Shows the current percentage of the progress
    */
    property bool showProgressPercentage: true

    style: Theme.createStyleComponent("ProgressBarStyle.qml", progressBar)
}
