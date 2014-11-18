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

import QtQuick 2.0
import Ubuntu.Components 1.1

Item {
  width: 300
  height: 300

  Image {
    width: 100
    height: 100
    anchors.centerIn: parent

    source: button.pressed ? "/usr/share/icons/ubuntu-mobile/actions/scalable/add.svg" : "/usr/share/icons/ubuntu-mobile/actions/scalable/delete.svg"

    onSourceChanged: print("source changed!", source)
  }

  Rectangle {
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    height: 70
    width: 150
    color: "gray"
    MouseArea {
      id: button
      anchors.fill: parent
    }
    Text {
      anchors.centerIn: parent
      text: "Click me!"
      color: "white"
    }
  }
}
