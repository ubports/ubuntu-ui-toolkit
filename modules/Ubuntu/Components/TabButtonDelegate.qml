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

Item {
    anchors.fill: parent

    BorderImage {
        anchors.fill: parent
        source: {
            if (tabButton.__isFirst) {
                return itemStyle.backgroundLeft;
            } else if (tabButton.__isLast) {
                return itemStyle.backgroundRight;
            } else {
                return itemStyle.backgroundMiddle;
            }
        }
    }

    TransparentButtonDelegate {
    }
}
