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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Labs 1.0

Item {
    id: main
    width: units.gu(200)
    height: units.gu(70)

    SplitView {
        anchors.fill: parent
        layouts: SplitViewLayout {
            when: true
            ViewColumn {
                preferredWidth: units.gu(40)
            }
            ViewColumn {
                fillWidth: true
            }
            ViewColumn {
                preferredWidth: units.gu(50)
            }
        }
        Rectangle {
            color: UbuntuColors.red
            height: parent.height
            // render it the first column
            SplitView.column: 0
        }
        Rectangle {
            color: UbuntuColors.green
            height: parent.height
            // render in the second column (if available)
//            SplitView.column: 1
        }
        Rectangle {
            color: UbuntuColors.blue
            height: parent.height
            // render in the third column (if available)
//            SplitView.column: 2
        }
    }
}
