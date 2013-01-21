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

/*!
    \qmltype Page
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief A page that can be included in \l Tab object or pushed on a \l PageStack

    \b{This component is under heavy development.}

    Examples: See \l Tabs and \l PageStack.
*/
Item {
    id: page

    anchors.fill: parent ? parent : undefined
    visible: false

    /*!
      \preliminary
      The title of the page. May be shown in a \l PageStack header (optional).
     */
    property string title

    /*!
      \internal
      Used in PageContainer to determine whether an Item is a Page.
     */
    property bool __isPage: true

    /*!
      \preliminary
      The \l PageStack that this Page has been pushed on, or null if it is not
      part of a PageStack. This value is automatically updated by the \l PageStack.
     */
    property PageStack pageStack

    // TODO TIM: document
    property Item tools
}
