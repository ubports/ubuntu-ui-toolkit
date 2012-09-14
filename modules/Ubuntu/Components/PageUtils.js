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

/*!
  \internal
  Initialize pageWrapper.object.
 */
function __initPage(pageWrapper) {
    var pageComponent;

    if (pageWrapper.reference.createObject) {
        // page reference is a component
        pageComponent = pageWrapper.reference;
    }
    else if (typeof pageWrapper.reference == "string") {
        // page reference is a string (url)
        pageComponent = Qt.createComponent(pageWrapper.reference);
    }

    var pageObject;
    if (pageComponent) {
        if (pageComponent.status === Component.Error) {
            throw new Error("Error while loading page: " + pageComponent.errorString());
        } else {
            // create te object
            pageObject = pageComponent.createObject(pageWrapper.owner);
            pageWrapper.canDestroy = true;
        }
    } else {
        // page reference is an object
        pageObject = pageWrapper.reference;
        pageObject.parent = pageWrapper.owner;
        pageWrapper.canDestroy = false;
    }

    pageObject.anchors.fill = pageWrapper.owner; // TODO: Do we always want this?

    return pageObject;
}

/*!
  \internal
  Create the page object if needed, and make the page object visible.
 */
function activate(pageWrapper) {
    if (!pageWrapper.object) {
        pageWrapper.object = __initPage(pageWrapper);
    }
    pageWrapper.object.visible = true;
}

/*!
  \internal
  Hide the page object, and destroy it if needed.
 */
function deactivate(pageWrapper) {
    if (pageWrapper.object) {
        pageWrapper.object.visible = false;

        if (pageWrapper.canDestroy) {
            pageWrapper.object.destroy();
            pageWrapper.object = null;
            pageWrapper.canDestroy = false;
        }
    }
}
