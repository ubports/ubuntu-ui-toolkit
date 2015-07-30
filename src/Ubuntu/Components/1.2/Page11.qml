/*
 * Copyright 2014 Canonical Ltd.
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

/*! \internal */
// Documentation in Page.qdoc
Page10 {
    id: page
    /*!
      \qmlproperty PageHeadConfiguration head
     */
    readonly property alias head: headerConfig
    PageHeadConfiguration {
        id: headerConfig
    }

    onToolsChanged: {
        print("Page.tools is a deprecated property. Please use Page.head instead.");
    }

    Object {
        id: internal

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
