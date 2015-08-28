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

//.pragma library // FIXME: cannot refer to Component.Error if I use this.
// FIXME: ideally we would make this a stateless library, but that breaks applications
//  that rely on accessing context variables in pages that were pushed on a PageStack
//  by url (PageStack.push("FileName.qml")) because of a Qt bug:
//  https://bugreports.qt-project.org/browse/QTBUG-31347

/*!
  \internal
  Incubator wrapper object.
  */

// internal function

// initialize
function Incubator(pageWrapper, pageComponent) {
    this.status =  Component.Ready;
    this.object = null;
    this.onStatusChanged = null;
    this.forceCompletion = function () {
        this.__incubator.forceCompletion();
        this.__pageWrapper.incubator = null;
        this.__incubator = null;
        this.destroy();
    }

    this.__incubator = null;
    this.__pageWrapper = null
    this.__activate = false;
    this.__pageWrapper = pageWrapper;

    this.__incubatorStatusChanged = function (status) {
        this.status = status;
        this.object = this.__incubator.object;
        // set pageWrapper.object if we're ready
        if (status == Component.Ready) {
            this.__pageWrapper.object = this.object;
        }
        // forward completion to the user
        if (this.onStatusChanged !== null) {
            // call onStatusChanged
            this.onStatusChanged(status);
            // activate if needed
            if (this.__activate && this.__pageWrapper.active && this.__pageWrapper.reference) {
                activate(this.__pageWrapper);
            }
        }
        // cleanup
        if (status !== Component.Loading) {
            this.__incubator = null;
            this.__pageWrapper.incubator = null;
        }
    }

    if (pageWrapper.properties) {
        this.__incubator = pageComponent.incubateObject(pageWrapper, pageWrapper.properties);
    } else {
        this.__incubator = pageComponent.incubateObject(pageWrapper);
    }

    this.status = this.__incubator.status;
    if (this.__incubator.status != Component.Ready) {
        this.__incubator.onStatusChanged = this.__incubatorStatusChanged;
    } else {
        this.__incubatorStatusChanged(this.__incubator.status);
    }
}

/*******************************************************
 *
 */
/*!
  \internal
  Initialize pageWrapper.object.
 */
function initPage(pageWrapper) {
    var pageComponent;

    // asynchronous cases
    if (pageWrapper.reference.createObject) {
        // page reference is a component
        pageComponent = pageWrapper.reference;
        pageWrapper.synchronous = false;
    } else if (typeof pageWrapper.reference == "string") {
        // page reference is a string (url)
        pageComponent = Qt.createComponent(pageWrapper.reference);
        pageWrapper.synchronous = false;
    }

    var pageObject = null;
    if (pageComponent) {
        if (pageComponent.status === Component.Error) {
            throw new Error("Error while loading page: " + pageComponent.errorString());
        } else {
            // create the object
            if (pageWrapper.synchronous) {
                if (pageWrapper.properties) {
                    // initialize the object with the given properties
                    pageObject = pageComponent.createObject(pageWrapper, pageWrapper.properties);
                } else {
                    pageObject = pageComponent.createObject(pageWrapper);
                }
            } else {
                print("PRE INCUBATOR")
                pageWrapper.incubator = new Incubator(pageWrapper, pageComponent);

                print("INCUBATOR", pageWrapper.incubator, pageWrapper.incubator.status)
            }
            pageWrapper.canDestroy = true;
        }
    } else {
        // page reference is an object
        pageObject = pageWrapper.reference;
        pageObject.parent = pageWrapper;
        pageWrapper.canDestroy = false;

        // copy the properties to the page object
        for (var prop in pageWrapper.properties) {
            if (pageWrapper.properties.hasOwnProperty(prop)) {
                pageObject[prop] = pageWrapper.properties[prop];
            }
        }
    }

    pageWrapper.object = pageObject;
    return pageObject;
}

/*!
  \internal
  Create the page object if needed, and make the page object visible.
 */
function activate(pageWrapper) {
    if (!pageWrapper.object) {
        initPage(pageWrapper);
    }

    // Having the same page pushed multiple times on a stack changes
    // the parent of the page object. Change it back here.
    pageWrapper.object.parent = pageWrapper;

    // Some page objects are invisible initially. Make visible.

    pageWrapper.object.visible = true;
    pageWrapper.active = true;
}

/*!
  \internal
  Hide page object.
 */
function deactivate(pageWrapper) {
    pageWrapper.active = false;
}

/*!
  \internal
  Destroy the page object if pageWrapper.canDestroy is true.
  Do nothing if pageWrapper.canDestroy is false.
 */
function destroyObject(pageWrapper) {
    if (pageWrapper.canDestroy) {
        pageWrapper.object.destroy();
        pageWrapper.object = null;
        pageWrapper.canDestroy = false;
    }
}
