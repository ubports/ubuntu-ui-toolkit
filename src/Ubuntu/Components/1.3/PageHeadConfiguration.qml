/*
 * Copyright 2016 Canonical Ltd.
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
import Ubuntu.Components 1.3

/*!
  \internal
  documented in PageHeadConfiguration.qdoc
 */
Object {
    // To be used inside a Page only.
    id: headerConfig

    property list<Action> actions
    property Action backAction: null
    property Item contents: null

    QtObject {
        id: internal
        property Item oldContents: null
    }
    onContentsChanged: {
        if (internal.oldContents) {
            // FIX: bug #1341814 and #1400297
            // We have to force the removal of the previous head.contents
            // in order to show the new contents
            internal.oldContents.parent = null;
        }
        internal.oldContents = contents;
    }

    property string preset: ""
    /*!
      \qmlproperty PageHeadSections sections
     */
    readonly property alias sections: headSections
    PageHeadSections {
        id: headSections
    }
    property color foregroundColor: theme.palette.normal.backgroundText

    property bool locked: false

    // auto-updated by AppHeader, but may be set by the developer
    property bool visible: true

    property string title
    property Flickable flickable
}
