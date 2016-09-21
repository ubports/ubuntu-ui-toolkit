/*
 * Copyright 2013 Canonical Ltd.
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
import QtQuick.LocalStorage 2.0

MainView {
    objectName: "localStorage"
    applicationName: "tv.island.pacific"

    Component.onCompleted: {
        var db = LocalStorage.openDatabaseSync("pacific.island.tv", "1.0", "lorem ipsum", 1000)
        db.transaction(function(tx){
            tx.executeSql('CREATE TABLE IF NOT EXISTS Island(name TEXT)')
            tx.executeSql('INSERT INTO Island VALUES(?)', ['Tuvalu'])
        })
    }
}
