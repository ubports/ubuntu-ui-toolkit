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

function checkDepends(deps,par) {
    var ok = false;
    try {
        ok = (null !== Qt.createQmlObject(deps + "; import QtQuick 2.0; Rectangle { }",par,"dependsCheck"));
    } catch (e) {
        console.log(e);
    }
    return ok;
}

function loadApp(ps, app, res) {
    var component = Qt.createComponent(app);
    if (component.status === Component.Ready) {
        var obj = component.createObject(ps);
        obj.rootPath = res;
        ps.push(obj);
    } else {
        console.log("ERROR: Unable to load app" + app);
    }
}
