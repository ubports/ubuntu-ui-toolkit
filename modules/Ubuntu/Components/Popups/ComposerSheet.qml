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
import Ubuntu.Components 0.1
/*!
    \qmltype ComposerSheet
    \inqmlmodule Ubuntu.Components.Popups 0.1
    \ingroup ubuntu-popups
    \brief Much like the \l DefaultSheet the Composer Sheet allows an application to insert a content
        view over the focused view without disrupting the navigation pattern. However the Composer Sheet
        is optimised for scenarios in which user content is at risk of corruption, most often (but not
        solely) when creating or editing content (e.g compose new message).
        There are two ways to dismiss it: user confirming the manipulation or user cancelling
        the manipulation, using the "confirm" and "cancel" buttons shown in the right and left side
        of the composer header.
*/

SheetBase {
    id: composer

    /*!
      \preliminary
      The user clicked the "cancel" button.
    */
    signal cancelClicked

    /*!
      \preliminary
      The user clicked the "confirm" button.
     */
    signal confirmClicked

    leftButton: Button {
        text: "cancel"
        /*! \internal */
        onClicked: composer.cancelClicked()
    }

    rightButton: Button {
        text: "confirm"
        color: "orange"
        /*! \internal */
        onClicked: composer.confirmClicked()
    }
}
