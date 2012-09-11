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
  \internal
  Initialize __pageObject.
 */
function init(page, owner) {
    var pageComponent;

    if (page.createObject) {
        // page is defined as a component
        pageComponent = page;
    }
    else if (typeof page =="string") {
        // page is defined as a string (url)
        pageComponent = Qt.createComponent(page);
    }

    var pageObject;
    if (pageComponent) {
        if (pageComponent.status == Component.Error) {
            throw new Error("Error while loading page: " + pageComponent.errorString());
        } else {
            pageObject = pageComponent.createObject(owner);
        }
    } else {
        pageObject = page;
    }

    if (pageObject.parent !== owner) {
        pageObject.parent = owner;
    }

    return pageObject;
}

/*!
  \internal
  Create the page object if \l page is link, and make the page object visible.
 */
function activate(pageObject, pageRef, owner) {
    if (!pageObject) {
        pageObject = init(pageRef, owner);
        pageObject.anchors.fill = owner;
    }
    pageObject.visible = true;
    return pageObject;
}

/*!
  \internal
  Hide the page object, and destroy it if it is not equal to \l page.
 */
function deactivate(pageObject, pageRef) {
    print("deactivating "+pageObject.title);
    if (pageObject) {
        pageObject.visible = false;
        if (pageObject !== pageRef) {
            pageObject.destroy();
            pageObject = null; // FIXME: hmm how does this work?
        }
    }
    return pageObject;
}

