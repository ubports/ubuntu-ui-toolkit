/*
 * Copyright 2013 Canonical Ltd.
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

FocusScope {
    id: styledItem

    property Component style
    readonly property Item delegate: delegateLoader.status == Loader.Ready ? delegateLoader.item : null

    implicitWidth: delegate ? delegate.implicitWidth : 0
    implicitHeight: delegate ? delegate.implicitHeight : 0

    Loader {
        id: delegateLoader
        anchors.fill: parent
        sourceComponent: style
        property Item styledItem: styledItem
    }
}
