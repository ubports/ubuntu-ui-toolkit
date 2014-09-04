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
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0

MainView {
    width: units.gu(40)
    height: units.gu(71)

    Component {
        id: dialog

        Dialog {
            id: item
            title: "A long title for the Dialog that wraps into two lines at least."
            text: "This is a question text"
            Button {
                text: "Close"
                onClicked: PopupUtils.close(item)
            }
        }
    }

    Button {
        text: "Open dialog"
        onClicked: PopupUtils.open(dialog)
    }

}
