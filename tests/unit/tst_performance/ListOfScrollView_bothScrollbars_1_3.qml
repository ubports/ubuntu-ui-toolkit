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
import Ubuntu.Components 1.3

//FIXME: This test uses 3.2Gb of RAM with a model:5000!
Column {
    width: 800
    height: 600
    Repeater {
        id: repeater
        model: 50
        ScrollView {
            width: units.gu(10)
            height: units.gu(20)
            Item {
                width: units.gu(20)
                height: units.gu(30)
            }
        }
    }
}
