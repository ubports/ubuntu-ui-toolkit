/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0

Item {
    id: foo
    property string spam
    /*!
       \qmlproperty string eggs
    */
    property string eggs: "white"
    /*!
       \qmlproperty string bar
       \since Extinct.Animals 1.1
    */
    property alias bar: foo.eggs
    /*!
       \internal
    */
    property var basket: ['brown', 'white']
    property list<Item> buckets

    states: [
State {
        name: ""
        }
    ]

    signal jump()
    /*!
       \since 2.2
    */
    signal bounce(int length)
    /*!
       \qmlproperty url bazz
    */
    property url bazz:"http://example.com"
    /*!
       \qmlmethod void fly(Wing wing)
    */
    signal fly(var wing)

    function first() {
        console.log('Haha!')
    }
    /*!
       \since 3.3
    */
    function second(sliff) {
    }
    function third(sloff) { }

    // Member name may overlap with internal name in apicheck
    property var type
    property var version

    Item {
        id: one
        function two() {
        }
        function three() { }
    }
}
