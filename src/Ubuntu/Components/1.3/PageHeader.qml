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
    \qmltype PageHeader
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
*/
Header {
    id: header

    anchors {
        left: parent.left
        right: parent.right
    }

    height: units.gu(6) // TODO TIM: move to style

    property alias title: titleLabel
    Label {
        id: titleLabel
        anchors {
            left: leading.right
            verticalCenter: parent.verticalCenter
            leftMargin: units.gu(2)
        }
        color: "white" // TODO: from Style
    }

    property alias leadingActionBar: leading
    ActionBar {
        // TODO: ActionBar must have configurable delegate.
        //      and take foreground color from style.
        id: leading
        anchors {
            left: parent.left
            leftMargin: units.gu(1)
            verticalCenter: parent.verticalCenter
        }
        numberOfSlots: 1
    }

    property alias trailingActionBar: trailing
    ActionBar {
        id: trailing
        anchors {
            right: parent.right
            rightMargin: units.gu(1)
            verticalCenter: parent.verticalCenter
        }
        numberOfSlots: 3
    }

    theme.version: Components.Ubuntu.toolkitVersion
    styleName: "PageHeaderStyle"
}
