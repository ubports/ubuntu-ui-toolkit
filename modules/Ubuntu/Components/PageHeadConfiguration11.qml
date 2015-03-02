/*
 * Copyright 2014-2015 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Components 1.1

/*!
  \internal
  Documented in PageHeadConfiguration.qdoc
 */
Object {
    // To be used inside a Page only.
    id: headerConfig

    property list<Action> actions
    property Action backAction: null
    property Item contents: null
    property string preset: ""
    readonly property alias sections: headSections
    PageHeadSections {
        id: headSections
    }
    property color foregroundColor: Theme.palette.selected.backgroundText
}
