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
  Initialize __pageObject.
 */
function initPage(pageWrapper) {
    var pageComponent;

    if (pageWrapper.reference.createObject) {
        // page is defined as a component
        pageComponent = pageWrapper.reference;
    }
    else if (typeof pageWrapper.reference == "string") {
        // page is defined as a string (url)
        pageComponent = Qt.createComponent(pageWrapper.reference);
    }

    var pageObject;
    if (pageComponent) {
        if (pageComponent.status == Component.Error) {
            throw new Error("Error while loading page: " + pageComponent.errorString());
        } else {
            pageObject = pageComponent.createObject(pageWrapper.owner);
            pageWrapper.canDestroy = true;
        }
    } else {
        pageObject = pageWrapper.reference;
        pageWrapper.canDestroy = false;
    }

    if (pageObject.parent !== pageWrapper.owner) {
        pageObject.parent = pageWrapper.owner;
    }

    return pageObject;
}

/*!
  \internal
  Create the page object if \l page is link, and make the page object visible.
 */
function activate(pageWrapper) {
    if (!pageWrapper.object) {
        pageWrapper.object = initPage(pageWrapper);
        // TODO: remove owner? Make pageWrapper child of owner?
        pageWrapper.object.anchors.fill = pageWrapper.owner;
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
