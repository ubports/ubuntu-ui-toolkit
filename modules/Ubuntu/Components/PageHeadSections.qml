/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2

/*!
    \qmltype PageHeadSections
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \since Ubuntu.Components 1.1
    \brief PageHeadSections is used to configure the sections for a \l Page.

    TODO: doc
 */
QtObject {
    // To be used inside PageHeadConfiguration
    id: sections

    property bool enabled: true
    property var model
    property int selectedIndex: model ? 0 : -1
}
