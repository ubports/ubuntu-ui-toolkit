/*
 * Copyright 2015 Canonical Ltd.
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
  Incubator wrapper object. Used when page is loaded asynchronously.
  */

function Incubator(pageWrapper, pageComponent) {
    // private variable for QmlIncubatorObject
    var incubator = null;

    // public API
    this.status = Component.Ready;
    this.object = null;
    this.onStatusChanged = null;
    this.forceCompletion = function () {
        if (incubator) {
            incubator.forceCompletion();
        }
    }

    // internal function to catch status changes
    function incubatorStatusChanged(status) {
        // update wrapper incubator fields
        pageWrapper.incubator.status = status;
        pageWrapper.incubator.object = pageWrapper.object = incubator.object;

        // emit pageWrapper's pageLoaded signal to complete page activation and loading
        if (status === Component.Ready) {
            pageWrapper.pageLoaded();
        }

        // forward state change to the user
        if (pageWrapper.incubator.onStatusChanged) {
            // call onStatusChanged
            pageWrapper.incubator.onStatusChanged(status);
        }

        // cleanup of ready or error
        if (status !== Component.Loading) {
            pageWrapper.incubator = null;
            incubator = null;
        }
    }

    if (pageWrapper.properties) {
        incubator = pageComponent.incubateObject(pageWrapper, pageWrapper.properties);
    } else {
        incubator = pageComponent.incubateObject(pageWrapper);
    }

    this.status = incubator.status;
    if (incubator.status != Component.Ready) {
        incubator.onStatusChanged = incubatorStatusChanged;
    } else {
        incubatorStatusChanged(incubator.status);
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

    if (pageWrapper.reference.createObject) {
        // page reference is a component
        pageComponent = pageWrapper.reference;
    } else if (typeof pageWrapper.reference == "string") {
        // page reference is a string (url)
        pageComponent = Qt.createComponent(pageWrapper.reference);
    }

    // PageWrapper can override the synchronous loading
    var synchronous = pageWrapper.hasOwnProperty("synchronous") ? pageWrapper.synchronous : true;

    if (pageComponent) {
        if (pageComponent.status === Component.Error) {
            throw new Error("Error while loading page: " + pageComponent.errorString());
        } else {
            // create the object
            pageWrapper.incubator = new Incubator(pageWrapper, pageComponent);
            if (synchronous) {
                pageWrapper.incubator.forceCompletion();
            }
            pageWrapper.canDestroy = true;
        }
    } else {
        // page reference is an object
        pageWrapper.object = pageWrapper.reference;
        pageWrapper.object.parent = pageWrapper;
        pageWrapper.canDestroy = false;

        // copy the properties to the page object
        for (var prop in pageWrapper.properties) {
            if (pageWrapper.properties.hasOwnProperty(prop)) {
                pageWrapper.object[prop] = pageWrapper.properties[prop];
            }
        }
    }

    return pageWrapper.object;
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
