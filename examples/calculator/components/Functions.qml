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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components 1.3
import Ubuntu.Layouts 1.0

Grid {
    id: buttonGrid
    columns: 4
    rows: 7
    spacing: units.gu(1)

    property var operations

    Repeater {
        model: [
            "%", "1/x", "x!", "x^2",
            "x^3", "x^y", "2^x", "sqrt",
            "ln", "e^x", "sin()", "cos()",
            "tan()", "cot()", "sec()", "csc()",
        ]
        delegate: Button {
            text: modelData
            width: (buttonGrid.width - 3 * buttonGrid.spacing) / 4
            height: (buttonGrid.height - 3 * buttonGrid.spacing) / 4
            onClicked: operations.execute(modelData)
        }
    }

}
