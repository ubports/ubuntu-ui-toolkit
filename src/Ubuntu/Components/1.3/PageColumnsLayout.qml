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

/*!
  \qmltype PageColumnsLayout
  \inqmlmodule Ubuntu.Components 1.3
  \since Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief Component configuring a layout in an AdaptivePageLayout component.
  */
QtObject {
    id: layout

    /*!
      Condition activating the layout. Defaults to false.
      */
    property bool when: false

    /*!
      \qmlproperty list<PageColumn> data
      \default
      Default property holding the PageColumn elements configuring each column.
      */
    default property alias data: layout.__data
    property list<PageColumn> __data//: [PageColumn{}]
}
