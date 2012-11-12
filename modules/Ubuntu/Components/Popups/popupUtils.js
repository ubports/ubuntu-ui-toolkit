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

function open(component, caller) {
    // TODO: Check that component is a valid component, and the object
    //  can be created.
    var popup = component.createObject(QuickUtils.rootObject, { "caller": caller });
    popup.show();

    popup.onVisibleChanged.connect(popup.__closeIfHidden);
    return popup;
}

//function close(popupObject) {
//    popupObject.hide();
//    popupObject.__closeIfHidden();
//}
