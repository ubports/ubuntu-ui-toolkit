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

Object {
    property PropertyAnimation fadeAnimation
    property PropertyAnimation sliderAnimation
    property PropertyAnimation thumbConnectorFading
    property PropertyAnimation thumbFading
    property real thickness
    property color sliderColor
    property color thumbConnectorColor
    property url forwardThumbReleased
    property url forwardThumbPressed
    property url backwardThumbReleased
    property url backwardThumbPressed
}
