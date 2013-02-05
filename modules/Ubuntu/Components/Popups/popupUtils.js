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

/*!
  The function opens a popup object (component or file) aligning it to its caller.
  The size of the popup can be given as parameter, as well as
  */
function open(popup, caller, params) {
    var popupComponent = null;
    if (popup.createObject) {
        // popup is a component and can create an object
        popupComponent = popup;
    } else if (typeof popup === "string") {
        popupComponent = Qt.createComponent(popup);
    } else {
        print("PopupUtils.open(): "+popup+" is not a component or a link");
        return;
    }

    var popupObject;
    if (params !== undefined) {
        popupObject = popupComponent.createObject(QuickUtils.rootObject, params);
    } else {
        popupObject = popupComponent.createObject(QuickUtils.rootObject);
    }
    if (!popupObject) {
        print("PopupUtils.open(): Failed to create the popup object.");
        return;
    } else if (popupObject.hasOwnProperty("caller"))
        popupObject.caller = caller;

    popupObject.show();
    popupObject.onVisibleChanged.connect(popupObject.__closeIfHidden);
    return popupObject;
}

function close(popupObject) {
    popupObject.hide();
    popupObject.destroy();
}
