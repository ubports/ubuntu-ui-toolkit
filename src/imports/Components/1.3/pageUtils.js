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
  \qmltype pageUtils
  \inqmlmodule Ubuntu.Components
  \ingroup ubuntu
  \internal
  \brief Page-related utility functions.
 */

/*!
  \qmlmethod pageUtils::isVerticalFlickable(object)
  Return true if the object is a Flickable that can be flicked in vertical direction.
 */
function isVerticalFlickable(object) {
    if (object && object.hasOwnProperty("flickableDirection") && object.hasOwnProperty("contentHeight")) {
        var direction = object.flickableDirection;
        if ( ((direction === Flickable.AutoFlickDirection) && (object.contentHeight !== object.height) )
                || direction === Flickable.VerticalFlick
                || direction === Flickable.HorizontalAndVerticalFlick) {
            return true;
        }
    }
    return false;
}

/*!
  \qmlmethod pageUtils::getFlickableChild(item)
  Return the first child of the item that is flickable in the vertical direction.
 */
function getFlickableChild(item) {
    if (item && item.hasOwnProperty("children")) {
        for (var i=0; i < item.children.length; i++) {
            var child = item.children[i];
            if (isVerticalFlickable(child)) {
                if (child.anchors.top === page.top || child.anchors.fill === page) {
                    return item.children[i];
                }
            }
        }
    }
    return null;
}
