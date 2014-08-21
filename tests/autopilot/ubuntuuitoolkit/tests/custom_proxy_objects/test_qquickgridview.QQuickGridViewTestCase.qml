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
import Ubuntu.Components.ListItems 0.1 as ListItems

MainView {

    width: units.gu(60)
    height: units.gu(80)
    objectName: 'mainView'

    GridView {
        objectName: 'testListView'
        anchors.fill: parent
        clip: true
        model: 20

        delegate: ListItems.Standard {
            objectName: 'testListElement%1'.arg(index)
            text: 'test list element %1'.arg(index)
            height: units.gu(5)
            width: units.gu(15)
        }
    }
}
