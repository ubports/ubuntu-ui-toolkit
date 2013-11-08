/*
 * Copyright (C) 2014 Canonical Ltd.
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

MainView {
    width: units.gu(50)
    height: units.gu(80)
    id: mainView

    PageStack {
        id: pageStack
    }

    Page {
        id: page
        title: "test page"
        Label {
            anchors.centerIn: parent
            text: "testing the toolbar"
        }
        tools: ToolbarItems {
            id: toolbarItems
            ToolbarButton {
                text: "action1"
            }
            locked: true
            opened: true
        }
    }

    Page {
        id: emptyPage
    }

    Component.onCompleted: {
        pageStack.push(page);
        pageStack.push(emptyPage);
        pageStack.pop();
    }
}
