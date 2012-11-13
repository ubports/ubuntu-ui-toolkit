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
    \qmltype ScrollBar
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The ScrollBar component provides scrolling functionality for
    scrollable views (i.e. Flickable, ListView).

    The ScrollBar can be set to any flickable and has built-in align setup
    to the attached flickable's front, rear, top or bottom. the scrollbar can
    also be aligned using anchors, however the built-in align functionality
    makes sure to have the proper alignemt applied based on theme and layout
    direction (RTL or LTR).

    Example:
    \qml
    Item {
        ListView {
            id: list
            width: units.gu(6)
            height: units.gu(6)
            model: 30
            delegate: Rectangle {
                width: ListView.view.width
                height: units.gu(5)
                Text {
                    anchors.fill: parent
                    text: "Item " + modelData
                }
            }
        }
        ScrollBar {
            flickableItem: list
            orientation: Qt.Vertical
            align: "rear"
        }
    }
    \endqml
  */

Item {
    id: scrollbar

    /*!
      \preliminary

      */
    property Flickable flickableItem: null

    /*!
      \preliminary
      */
    property int orientation: Qt.Vertical

    /*!
      \preliminary
      */
    property string align: "rear"

    /*!
      \preliminary
      */
    property bool interactive: true

    // styling
    Theming.ItemStyle.class: "scrollbar"
    implicitWidth: (Theming.ItemStyle.style && Theming.ItemStyle.style.hasOwnProperty("thickness")) ?
                       Theming.ItemStyle.style.thickness : units.gu(4)
    implicitHeight: (Theming.ItemStyle.style && Theming.ItemStyle.style.hasOwnProperty("thickness")) ?
                       Theming.ItemStyle.style.thickness : units.gu(4)

    anchors {
        left: internals.leftAnchor(flickableItem)
        right: internals.rightAnchor(flickableItem)
        top: internals.topAnchor(flickableItem)
        bottom: internals.bottomAnchor(flickableItem)
    }

    opacity: internals.scrollable ? 1.0 : 0.0
    //FIXME: see previous FIXME
    Behavior on opacity {
        animation: (Theming.ItemStyle.style && Theming.ItemStyle.style.fadeAnimation) ?
                       Theming.ItemStyle.style.fadeAnimation : null
    }
    /*
      Internals: contains the common logic of the scrollbar
    */
    property alias internals: internalObject
    Object {
        id: internalObject
        property bool scrollable: flickableItem && flickableItem.interactive && pageSize > 0.0
                                  && contentSize > 0.0 && contentSize > pageSize

        property real pageSize: (scrollbar.orientation == Qt.Vertical) ? flickableItem.height : flickableItem.width
        property real contentSize: (scrollbar.orientation == Qt.Vertical) ?
                                       flickableItem.contentHeight : flickableItem.contentWidth
        // LTR and RTL are provided by LayoutMirroring, so no need to check that
        function leftAnchor(item)
        {
            if ((orientation == Qt.Horizontal) || ((orientation == Qt.Vertical) && (align == "front")))
                return item.left;
            return undefined;
        }
        function rightAnchor(item)
        {
            if ((orientation == Qt.Horizontal) || ((orientation == Qt.Vertical) && (align == "rear")))
                return item.right;
            return undefined;
        }
        function topAnchor(item)
        {
            if ((orientation == Qt.Vertical) || ((orientation == Qt.Horizontal) && (align == "top")))
                return item.top;
            return undefined;
        }
        function bottomAnchor(item)
        {
            if ((orientation == Qt.Vertical) || ((orientation == Qt.Horizontal) && (align == "bottom")))
                return item.bottom;
            return undefined;
        }
    }
}
