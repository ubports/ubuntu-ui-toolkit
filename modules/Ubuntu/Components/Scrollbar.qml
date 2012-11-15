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

    The ScrollBar can be set to any flickable and has built-in anchoring setup
    to the attached flickable's front, rear, top or bottom. the scrollbar can
    also be aligned using anchors, however the built-in align functionality
    makes sure to have the proper alignemt applied based on theme and layout
    direction (RTL or LTR).

    Scrollbars are passive on handheld devices and active on desktops. Passive
    scrollbars are simple decorators which only show the flickableItem's content
    position but cannot modify that.

    The content position is driven through the attaced Flickable. Therefore every
    delegate implementation should drive the position through contentX/contentY
    properties, depending on whether the orientation is vertical or horizontal.

    Example:
    \qml
    Item {
        ListView {
            id: list
            width: units.gu(37)
            height: units.gu(37)
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
        This proeprty holds the flickable item (Flickable, ListView or GridView)
        the Scrollbar is attached to.
      */
    property Flickable flickableItem: null

    /*!
      \preliminary
      The property handles the alignment of the scrollbar to the flickableItem.
      It is translated into anchoring as follows:
        \list
        \li Qt.AlignLeft anchors to the left on LTR and to the right on RTL layouts
        \li Qt.AlignRight anchors to the right on LTR and to the left on RTL layouts
        \li Qt.AlignTop anchors to the top
        \li Qt.AlignBottom anchors to the bottom
        \endlist
        The default value is \b Qt.AlignRight.
      */
    property int align: Qt.AlignRight

    /*!
      \internal
      This property holds whether the scrollbar is active or passive. It is present
      for testing purposes.
    */
    property bool __interactive: false

    // styling
    Theming.ItemStyle.class: "scrollbar"
    implicitWidth: (Theming.ItemStyle.style && Theming.ItemStyle.style.hasOwnProperty("sensingAreaThickness")) ?
                       Theming.ItemStyle.style.sensingAreaThickness : units.gu(4)
    implicitHeight: (Theming.ItemStyle.style && Theming.ItemStyle.style.hasOwnProperty("sensingAreaThickness")) ?
                       Theming.ItemStyle.style.sensingAreaThickness : units.gu(4)

    anchors {
        left: internals.leftAnchor(flickableItem)
        right: internals.rightAnchor(flickableItem)
        top: internals.topAnchor(flickableItem)
        bottom: internals.bottomAnchor(flickableItem)
    }

    /*!
      \internal
      */
    onAlignChanged: if (!internals.checkAlign()) console.log("Wrong alignment set to Scrollbar: "+align)

    /*!
      \internal
      Internals: contains the common logic of the scrollbar like anchoring, size
      calculations and ListView-specific size calculation/detection.
    */
    property alias __private: internals
    Object {
        id: internals
        property bool vertical: (align === Qt.AlignLeft) || (align === Qt.AlignRight)
        property bool scrollable: flickableItem && flickableItem.interactive && checkAlign()

        function checkAlign()
        {
            return (align === Qt.AlignLeft) || (align === Qt.AlignRight) || (align === Qt.AlignTop) || (align === Qt.AlignBottom);
        }

        // LTR and RTL are provided by LayoutMirroring, so no need to check that
        function leftAnchor(object)
        {
            if (!internals.vertical || (align == Qt.AlignLeft))
                return object.left;
            return undefined;
        }
        function rightAnchor(object)
        {
            if (!internals.vertical || (align == Qt.AlignRight))
                return object.right;
            return undefined;
        }
        function topAnchor(object)
        {
            if (internals.vertical || (align == Qt.AlignTop))
                return object.top;
            return undefined;
        }
        function bottomAnchor(object)
        {
            if (internals.vertical || (align == Qt.AlignBottom))
                return object.bottom;
            return undefined;
        }
    }
}
