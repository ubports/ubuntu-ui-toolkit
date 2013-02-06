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

import QtQuick 2.0
import Ubuntu.Components 0.1

Page {
    title: i18n.tr("My custom page")
    Label {
        anchors.centerIn: parent
        text: i18n.tr("This is an external page.")
        color: "#757373"
    }

    tools: ToolbarActions {
        Action {
            text: "action 1"
            iconSource: Qt.resolvedUrl("avatar_contacts_list.png")
        }
        Action {
            text: "action 2"
            iconSource: Qt.resolvedUrl("call_icon.png")
        }
    }
}
