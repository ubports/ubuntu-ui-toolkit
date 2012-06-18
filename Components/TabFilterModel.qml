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
    //property list<QtObject> proxyModel
    property variant proxyModel
    property ListModel model: ListModel { }
    property int proxyCount: proxyModel ? proxyModel.length : 0

    function checkFilter(element) {
        print("checkFilter("+element+")");
        //if (element instanceof Item) return true; // not sure if this works
        if ("page" in element) return true;
        else return false;
    }

    function applyFilter() {
        print("applying filter")
        model.clear();
        for(var i=0; i < proxyCount; i++) {
            print("filtering element "+i)
            var element = proxyModel[i]
            //print("element = "+element)         // element = null
            if (checkFilter(element)) {
                print("adding element")
                var listElement = Qt.createComponent(ListElement);

                model.append(listElement)
           }
        }
    }

    onProxyModelChanged: applyFilter()
    onProxyCountChanged: {
        print("proxy count changed to "+proxyCount)
        applyFilter()
    }
}
