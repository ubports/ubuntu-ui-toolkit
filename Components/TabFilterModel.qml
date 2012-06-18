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

import QtQuick 1.1

/*!
    \qmlclass TabFilterModel
    \inqmlmodule UbuntuUIToolkit
    \brief Internal
*/

QtObject {
    property list<QtObject> proxyModel
    property list<QtObject> model: [ QtObject { } ] // cannot initialize empty list?
    property int proxyCount: proxyModel ? proxyModel.count : 0 // ERROR: proxyModel.count is undefined

    function checkFilter(element) {
        return true;
    }

    function applyFilter() {
        model.clear(); // ERROR: no clear() function.
        for(var i=0; i < proxyModel.count; i++) {
            var element = proxyModel.get(i)
            if (checkFilter(element)) {
                model.append(element)
            }
        }
    }

    onProxyModelChanged: applyFilter()
    onProxyCountChanged: applyFilter()
}
