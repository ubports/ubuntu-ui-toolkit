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
  \qmltype ColumnMetrics
  \inqmlmodule Ubuntu.Components 1.3
  \since Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief Component configuring the metrics of a column in MultiColumnView.
  \internal

  */
QtObject {
    /*!
      1-based value identifying the column the metrics to be applied to.
      */
    property int column

    /*!
      Specifies whether the width of the column should fill the available space
      of the MultiColumnView column or not. Defaults to \a false.
      */
    property bool fillWidth: false

    /*!
      Specifies the minimum width of the column. If the value is greater than
      \b MultiColumnView::defaultColumnWidth, the value will be set as width for
      the column.
      */
    property real minimumWidth: 0

    /*!
      Specifies the maximum width of the column. If the value is smaller than
      \b MultiColumnView::defaultColumnWidth, the value will be set as width for
      the column. A maximum value of 0 will be ignored.
      */
    property real maximumWidth: Number.POSITIVE_INFINITY
}
