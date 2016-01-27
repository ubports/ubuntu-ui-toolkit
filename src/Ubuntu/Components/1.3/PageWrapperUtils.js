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

/*!
  \qmltype PageWrapperUtils
  \inqmlmodule Ubuntu.Components 1.3
  \ingroup ubuntu
  \internal
  \brief Internal book-keeping used by AdaptivePageLayout.
 */

//.pragma library // FIXME: cannot refer to Component.Error if I use this.
// FIXME: ideally we would make this a stateless library, but that breaks applications
//  that rely on accessing context variables in pages that were pushed on a PageStack
//  by url (PageStack.push("FileName.qml")) because of a Qt bug:
//  https://bugreports.qt-project.org/browse/QTBUG-31347

/*!
  \internal
  \qmlmethod PageWrapperUtils::Incubator(pageWrapper, pageComponent)
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

        // forward state change to the user
        if (pageWrapper.incubator.onStatusChanged) {
            // call onStatusChanged
            pageWrapper.incubator.onStatusChanged(status);
        }

        // emit pageWrapper's pageLoaded signal to complete page activation
        if (status === Component.Ready) {
            pageWrapper.pageLoaded();
        }

        // cleanup of ready or error
        if (status !== Component.Loading) {
            pageWrapper.incubator = null;
        }
    }

    function incubatePage() {
        if (pageComponent.status == Component.Loading) {
            return;
        }

        if (pageWrapper.properties) {
            incubator = pageComponent.incubateObject(pageWrapper, pageWrapper.properties);
        } else {
            incubator = pageComponent.incubateObject(pageWrapper);
        }

        if (!incubator) {
            throw new Error("Error while loading page: " + pageComponent.errorString());
        } else if (incubator.status != Component.Ready) {
            pageWrapper.incubator.status = incubator.status;
            incubator.onStatusChanged = incubatorStatusChanged;
        } else {
            incubatorStatusChanged(incubator.status);
        }
    }

    // main
    pageWrapper.incubator = this;
    if (pageComponent.status == Component.Loading) {
        pageComponent.statusChanged.connect(incubatePage);
        this.status = Component.Loading;
    }
    incubatePage();
}

/*******************************************************
 *
 */
/*!
  \internal
  \qmlmethod PageWrapperUtils::initPage(pageWrapper)
  Initialize pageWrapper.object.
 */
function initPage(pageWrapper) {
    var pageComponent;

    if (pageWrapper.reference.createObject) {
        // page reference is a component
        pageComponent = pageWrapper.reference;
    } else if (typeof pageWrapper.reference == "string") {
        // page reference is a string (url)
        if (pageWrapper.synchronous) {
            pageComponent = Qt.createComponent(pageWrapper.reference);
        } else {
            pageComponent = Qt.createComponent(pageWrapper.reference, Component.Asynchronous);
        }
    }

    if (pageComponent) {
        if (pageComponent.status === Component.Error) {
            throw new Error("Error while loading page: " + pageComponent.errorString());
        } else {
            // create the object
            if (pageWrapper.synchronous) {
                if (pageWrapper.properties) {
                    // initialize the object with the given properties
                    pageWrapper.object = pageComponent.createObject(pageWrapper, pageWrapper.properties);
                } else {
                    pageWrapper.object = pageComponent.createObject(pageWrapper);
                }
            } else {
                pageWrapper.incubator = new Incubator(pageWrapper, pageComponent);
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
  \qmlmethod PageWrapperUtils::activate(pageWrapper)
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
  \qmlmethod PageWrapperUtils::deactivate(pageWrapper)
  Hide page object.
 */
function deactivate(pageWrapper) {
    pageWrapper.active = false;
}

/*!
  \internal
  \qmlmethod PageWrapperUtils::destroyObject(pageWrapper)
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
