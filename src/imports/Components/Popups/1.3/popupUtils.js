/*
 * Copyright 2016 Canonical Ltd.
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
    \qmltype popupUtils
    \inqmlmodule Ubuntu.Components.Popups 1.0
    \ingroup ubuntu-popups
    \brief Popover utility functions.
*/

/*!
  \qmlmethod popupUtils::open(popup, caller, params)
  The function creates and shows a popup object from either a \b Component or URL of a
  QML document. The caller drives the placement of the popup as well as the
  pointer's. The third parameter (param) can hold a series of properties configuring
  the popup to be opened. This can be any property defined by the popups and additional
  custom ones defined in derived popups.

  \a caller should be given when a \l ComposerSheet or \l Dialog is specified using a URL
  and opened inside a \b Window. If not, the application's root item will be the dismiss area.

  Returns a popup object, which can be closed using \l close.

  Note that popups created from a file or component will be created as children
  of the root item. If that's not a good choice, a \b MainWindow used as the
  root item can set the \b visualRoot property an arbitrary \b Item that acts
  as the parent of all popups.

  \qml
      import Ubuntu.Components 1.3
      import Ubuntu.Components.Popups 1.3

      Button {
          onClicked: PopupUtils.open(popoverComponent, popoverButton, { 'dummy': true } )
      }
  \endqml

  See \l Popover, \l ComposerSheet and \l Dialog for their respective documentation and more examples.
  */
function open(popup, caller, params) {
    var popupComponent = null;
    var rootObject = null;
    if (popup.createObject) {
        // popup is a component and can create an object
        popupComponent = popup;
        rootObject = QuickUtils.rootItem(caller !== undefined ? caller : popup);
    } else if (typeof popup === "string") {
        popupComponent = Qt.createComponent(popup);
        rootObject = (caller !== undefined) ? QuickUtils.rootItem(caller) : QuickUtils.rootItem(null);
    } else {
        print("PopupUtils.open(): "+popup+" is not a component or a link");
        return null;
    }

    if (!rootObject) {
        print("PopupUtils.open(): Failed to get the root object.");
        return null;
    }

    var popupObject;
    // If there's an active item, save it so we can restore it later
    var prevFocusItem = (typeof window !== "undefined") && window ? window.activeFocusItem : null;
    if (params !== undefined) {
        popupObject = popupComponent.createObject(rootObject, params);
    } else {
        popupObject = popupComponent.createObject(rootObject);
    }
    if (!popupObject) {
        print(popupComponent.errorString().slice(0, -1));
        print("PopupUtils.open(): Failed to create the popup object.");
        return;
    } else if (popupObject.hasOwnProperty("caller") && caller) {
        popupObject.caller = caller;
    } else if (popupObject.hasOwnProperty("__setPreviousActiveFocusItem")) {
        popupObject.__setPreviousActiveFocusItem(prevFocusItem);
    }

    // if caller is specified, connect its cleanup to the popup's close
    // so popups will be removed together with the caller.
    if (caller)
        caller.Component.onDestruction.connect(popupObject.__closePopup);

    // Instantly (no fading) close and destroy the popup when the component
    //  gets destroyed because that invalidates the QML context of the popup
    //  and prevents the closing after fadeout. See bug 1568016.
    popupComponent.Component.destruction.connect(popupObject.__closePopup);

    popupObject.show();
    popupObject.onVisibleChanged.connect(popupObject.__closeIfHidden);
    return popupObject;
}

/*!
  \qmlmethod popupUtils::close(popupObject)
  Closes (hides and destroys) the given popup.

  \qml
      import Ubuntu.Components 1.3
      import Ubuntu.Components.Popups 1.3

      Button {
          onClicked: PopupUtils.close(popup)
      }
  \endqml
   */
function close(popupObject) {
    popupObject.hide();
}
