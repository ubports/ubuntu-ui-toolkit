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

    property string title
//    property alias title: titleLabel
//    Label {
//        id: titleLabel
//        anchors {
//            left: leading.right
//            verticalCenter: parent.verticalCenter
//            leftMargin: units.gu(2)
//        }
//        color: header.__styleInstance.foregroundColor
//    }

//    Loader {
//        anchors {
//            left: leadingActionBar.right
//            right: trailingActionBar.left
//            leftMargin: 0
//        }
//        sourceComponent: __styleInstance.defaultContentsDelegate
//    }

//    property Item contents: __styleInstance.defaultContents
//    property Item contents: Rectangle { color: "pink"; anchors.fill: parent }



//    Item {
//        id: contentsHolder
//        anchors {
//            left: leading.right
//            right: trailing.left
//            top: parent.top
//            bottom: parent.bottom
//        }

//        Binding {
//            target: contents
//            property: "parent"
//            value: contentsHolder
//        }
//    }

//    Loader {
//        anchors {
//            left: leading.right
//            right: trailing.left
//            top: parent.top
//            bottom: parent.bottom
//        }
//        sourceComponent: __styleInstance.defaultContentsDelegate
//    }
    default property alias contentsHolder: contentsHolderItem
    Item {
        id: contentsHolderItem
        anchors {
            left: leading.right
            right: trailing.left
            top: parent.top
            bottom: parent.bottom
        }
    }

    Loader {
        sourceComponent: contentsHolderItem.children.count > 0
                         ? null : __styleInstance.defaultContentsDelegate
    }

    readonly property alias leadingActionBar: leading
    ActionBar {
        id: leading
        anchors {
            left: parent.left
            leftMargin: units.gu(1)
            verticalCenter: parent.verticalCenter
        }
        numberOfSlots: 1
        delegate: header.__styleInstance.defaultActionDelegate
    }

    readonly property alias trailingActionBar: trailing
    ActionBar {
        id: trailing
        anchors {
            right: parent.right
            rightMargin: units.gu(1)
            verticalCenter: parent.verticalCenter
        }
        numberOfSlots: 3
        delegate: header.__styleInstance.defaultActionDelegate
    }

    theme.version: Components.Ubuntu.toolkitVersion
    styleName: "PageHeaderStyle"
}
