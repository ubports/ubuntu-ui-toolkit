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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

/*!
    \qmltype Chrome
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief TODO

    \b{This component is under heavy development.}
*/
Item {
    id: chrome

    clip: true

    property Item page

    onPageChanged: print("new page = "+page.title)

    Toolbar {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        pageStack: chrome.page ? chrome.page.pageStack : null
        onPageStackChanged: print("pageStack = "+pageStack)
    }
}
