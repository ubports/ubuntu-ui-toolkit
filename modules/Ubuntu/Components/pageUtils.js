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

.pragma library

function isPage(object) {
    return (object && object.hasOwnProperty("__isPage") && object.__isPage);
}

function isMainView(object) {
    return (object && object.hasOwnProperty("__isMainView") && object.__isMainView);
}

function isFlickable(object) {
    return object && object.hasOwnProperty("flicking") && object.hasOwnProperty("flickableDirection");
}

/*!
  Return the first flickable child of this page.
 */
function getFlickableChild(item) {
    if (item && item.hasOwnProperty("children")) {
        for (var i=0; i < item.children.length; i++) {
            if (isFlickable(item.children[i])) return item.children[i];
        }
    }
    return null;
}

/*!
  Return the parent node in the PageTreeNode, or null if the item is the root node or invalid.
 */
function getParentPageTreeNode(item) {
    var node = null;
    if (item) {
        var i = item.parent;
        while (i && !node) {
            if (isPage(i)) node = i;
            i = i.parent;
        }
    }
    return node;
}
