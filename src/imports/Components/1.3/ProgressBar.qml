/*
 * Copyright 2015-2016 Canonical Ltd.
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
    \qmltype ProgressBar
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu
    \brief The ProgressBar component visually indicates the progress of a process of
    determinate or indeterminate duration.

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
AnimatedItem {
    id: progressBar

    /*!
      Specifies whether the progress interval is unknown. When set, altering
      other properties do not have any effect on the component's behavior.
      By default the property is set to false.
    */
    property bool indeterminate: false

    /*!
      Specifies the minimum value of the progress.
    */
    property real minimumValue: 0.0

    /*!
      Specifies the maximum value of the progress.
    */
    property real maximumValue: 1.0

    /*!
      Specifies the current value of the progress
    */
    property real value: 0.5

    /*!
      \since Ubuntu.Components 1.1
      \deprecated
      Specifies if the value of the progress is visible. The current visuals in
      Ubuntu Components 1.3 do not support this feature.
    */
    property bool showProgressPercentage: true

    styleName: "ProgressBarStyle"
}
