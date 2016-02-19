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
import Ubuntu.Components 1.3

/*!
    \internal
    \deprecated
    Older version of Sections to work around bug 1511839
    (only for controlling sections via PageHeadSections).
 */
StyledItem {
    id: sections
    styleName: "SectionsForPageHeadStyle"

    property list<Action> actions
    property var model: actions
    onModelChanged: {
        if (model && model.length > 3) {
            console.warn("PageHeadSections does not support more than 3 sections. Use PageHeader and Sections instead.");
        }
    }
    property int selectedIndex: model ? 0 : -1
    onSelectedIndexChanged: {
        if ((selectedIndex >= 0) && (selectedIndex < model.length)) {
            if (model[selectedIndex].hasOwnProperty("trigger")) {
                model[selectedIndex].trigger();
            }
        }
    }
}
