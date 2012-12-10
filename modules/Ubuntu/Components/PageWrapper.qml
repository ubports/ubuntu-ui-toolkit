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

import QtQuick 2.0
import "PageWrapperUtils.js" as Utils

/*!
    \internal
    \qmltype PageWrapper
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Internal class used by \l Tab

    \b{This component is under heavy development.}
*/
QtObject {
    id: pageWrapper

    /*!
      \preliminary
      The reference to the page object. This can be the page
      itself (which is an Item), but also a url pointing to a QML file.
     */
    property variant reference

    /*!
      \preliminary
      The initialized page object, or null if the object needs to be created.
     */
    property Item object: null

    /*!
      \preliminary
      The parent Item of the page object.
     */
    property Item parent

    /*!
      \preliminary
      This variable will be true if \l object holds an object that was created
      from the given reference, and thus can be destroyed when no the page is deactivated.
     */
    property bool canDestroy: false

    /*!
      \preliminary
      Determines whether the wrapped page is currently visible.
     */
    property bool active: false

    /*!
      \preliminary
      Properties are use to initialize a new object, or if reference
      is already an object, properties are copied to the object when activated.
     */
    property variant properties

    /*!
      \preliminary
      The \l PageStack that the \l Page is part of, if any (null otherwise).
     */
    property PageStack pageStack

    /*!
      \preliminary
      The object if it is Flickable, or the first of its children that is Flickable.
      If neither object nor its children are Flickable, then null.
      Call PageWrapperUtils.detectFlickable(pageWrapper) to detect.
     */
    property Flickable flickable: null

    /*!
      \internal
     */
    onActiveChanged: {
        if (reference) {
            if (pageWrapper.active) Utils.activate(pageWrapper);
            else Utils.deactivate(pageWrapper);
        }
    }

    /*!
      \internal
      */
    onReferenceChanged: {
        Utils.deactivate(pageWrapper);
        if (pageWrapper.object) pageWrapper.object = null;
        if (pageWrapper.active && reference) {
            Utils.activate(pageWrapper);
        }
    }

    /*!
      \internal
     */
    onParentChanged: Utils.updateParent(pageWrapper)

    /*!
      \internal
     */
    onPageStackChanged: Utils.updatePageStack(pageWrapper)

    /*!
      \internal
     */
    Component.onDestruction: {
        Utils.deactivate(pageWrapper);
        if (pageWrapper.canDestroy) Utils.destroyObject(pageWrapper);
    }

    /*!
      \internal
      Destroy \l object. Only call this function if \l canDestroy
     */
    function destroyObject() {
        Utils.destroyObject(pageWrapper);
    }
}
