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
  \qmltype PageColumn
  \inqmlmodule Ubuntu.Components 1.3
  \since Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief Component configuring the metrics of a column in AdaptivePageLayout.

  */
QtObject {
    /*!
      1-based value identifying the column the metrics to be applied to.
      \internal
      */
    property int __column

    /*!
      Specifies whether the width of the column should fill the available space
      of the AdaptivePageLayout column or not. Defaults to \a false.
      */
    property bool fillWidth: false

    /*!
      Specifies the minimum width of the column. Defaults to 0.
      */
    property real minimumWidth: 0

    /*!
      Specifies the maximum width of the column. A maximum value of 0 will be ignored.
      Defaults to the maximum positive value.
      */
    property real maximumWidth: Number.POSITIVE_INFINITY

    /*!
      Specifies the preferred width of the column when the layout is initialized.
      Defaults to 0. AdaptivePageLayout clamps the given value between \l minimumWidth
      and \l maximumWidth. The value must be set if the \l fillWidth and \l minimumWidth
      are not set.
      */
    property real preferredWidth: 0
}
