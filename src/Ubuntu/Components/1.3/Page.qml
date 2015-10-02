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

import QtQuick 2.4
import Ubuntu.Components 1.3 as Toolkit13
import "pageUtils.js" as Utils

/*!
  \internal
  Documentation is in Page.qdoc
*/
PageTreeNode {
    id: page
    anchors {
        left: parent ? parent.left : undefined
        bottom: parent ? parent.bottom : undefined
    }
    // Set width and height so that a parent Loader can be automatically resized
    // to the size of the loaded Page.
    width: parentNode ? parentNode.width - page.x : undefined
    height: parentNode ? page.flickable ? parentNode.height : parentNode.height - internal.headerHeight : undefined

    isLeaf: true
    property string title: parentNode && parentNode.hasOwnProperty("title") ? parentNode.title : ""
    property Flickable flickable: Utils.getFlickableChild(page)

    /*!
      \qmlproperty PageHeadConfiguration head
     */
    readonly property alias head: headerConfig
    Toolkit13.PageHeadConfiguration {
        id: headerConfig
        title: page.title
        flickable: page.flickable
    }

    Toolkit13.Object {
        id: internal

        property AppHeader header: page.__propagated && page.__propagated.header ? page.__propagated.header : null
        // Used to position the Page when there is no flickable.
        // When there is a flickable, the header will automatically position it.
        property real headerHeight: internal.header && internal.header.visible ?
                                        internal.header.height + internal.header.y : 0

        // Note: The bindings below need to check whether headerConfig.contents
        // is valid in the "value", even when that is required in the Binding's "when"
        // property, to avoid TypeErrors while/after a page becomes (in)active.
        //
        // Note 2: contents.parent binding is made by PageHeadStyle.
        property bool hasParent: headerConfig.contents &&
                                 headerConfig.contents.parent

        Binding {
            target: headerConfig.contents
            property: "visible"
            value: page.active
            when: headerConfig.contents
        }
        Binding {
            target: headerConfig.contents
            property: "anchors.verticalCenter"
            value: internal.hasParent ? headerConfig.contents.parent.verticalCenter :
                                        undefined
            when: headerConfig.contents
        }
        Binding {
            target: headerConfig.contents
            property: "anchors.left"
            value: internal.hasParent ? headerConfig.contents.parent.left : undefined
            when: headerConfig.contents
        }
    }
}
