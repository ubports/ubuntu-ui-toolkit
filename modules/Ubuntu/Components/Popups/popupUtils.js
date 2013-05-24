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
  The function opens a popup object which can be either a Component or in a separate
  QML document. The caller drives the Popup's and Dialog's placement as well as the
  pointer's. The third parameter (param) can hold a series of properties configuring
  the popup to be opened. This can be any property defined by the popups and additional
  custom ones defined in derived popups.

  Sheets do not need callers to be specified, however when using non-modal Sheets or
  Dialogs, it is worth to set the caller when opening Sheets to avoid leaving orphan
  sheets on the screen.
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
        popupObject = popupComponent.createObject(QuickUtils.rootItem(popup), params);
    } else {
        popupObject = popupComponent.createObject(QuickUtils.rootItem(popup));
    }
    if (!popupObject) {
        print("PopupUtils.open(): Failed to create the popup object.");
        return;
    } else if (popupObject.hasOwnProperty("caller"))
        popupObject.caller = caller;

    // if caller is specified, connect its cleanup to the popup's close
    // so popups will be removed together with the caller.
    if (caller)
        caller.Component.onDestruction.connect(popupObject.__closePopup);

    popupObject.show();
    popupObject.onVisibleChanged.connect(popupObject.__closeIfHidden);
    return popupObject;
}

/*!
  Closes (hides and destroys) the given popup.
  */
function close(popupObject) {
    popupObject.hide();
}
