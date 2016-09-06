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

import QtQuick 2.0
import Ubuntu.Components 1.3

Column {
    width: 800
    height: 600

    Repeater {
        model: 5000
        ListItem {
            HardcodedFourContainersLayout {
                firstSlot: Item { width: units.gu(2); }
                secondSlot: Item { width: units.gu(2); }
                thirdSlot: Item { width: units.gu(2); }
            }
        }
    }
}
