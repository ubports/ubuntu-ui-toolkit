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
      Scrollbar orientation, Qt.Vertical or Qt.Horizontal. The default value is
      Qt.Vertical.
      */
    property int orientation: Qt.Vertical

    /*!
      \preliminary
        String property holding the anchoring of the Scrollbar to the flickableItem.
        Possible values are \b{front, rear, top, bottom}.
        \list
        \li front anchors to the left on LTR and to the right on RTL layouts
        \li rear anchors to the right on LTR and to the left on RTL layouts
        \li top anchors to the top
        \li bottom anchors to the bottom
        \endlist
        The default value is \b rear.
      */
    property string align: "rear"

    /*!
      \preliminary
      Read-only property presenting the content's position within the flickableItem.
      */
    readonly property real contentPosition: internals.contentPosition

    /*!
      \internal
      This property holds whether the scrollbar is active or passive. It is present
      for testing purposes.
    */
    property bool __passive: false


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
      Detecting the proper flickable type
      */
    onFlickableItemChanged: internals.detectFlickableLogic();

    /*!
      \internal
      Internals: contains the common logic of the scrollbar like anchoring, size
      calculations and ListView-specific size calculation/detection.
    */
    property alias __private: internals
    Object {
        id: internals
        property QtObject listView: null
        // The content position is driven through the flickableItem's contentX
        property real contentPosition
        property bool scrollable: flickableItem && flickableItem.interactive && pageSize > 0.0
                                  && contentSize > 0.0 && contentSize > pageSize
        property real pageSize: (scrollbar.orientation == Qt.Vertical) ? flickableItem.height : flickableItem.width
        property real contentSize: (scrollbar.orientation == Qt.Vertical) ?
                                       ((internals.listView) ? internals.listView.size : flickableItem.contentHeight) :
                                       ((internals.listView) ? internals.listView.size : flickableItem.contentWidth)
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

        // size detection and content position tracking logic
        // common logic for Flickable and ListView to update contentPosition when Flicked
        Connections {
            target: flickableItem
            onContentYChanged: if (orientation == Qt.Vertical) internals.contentPosition = flickableItem.contentY - flickableItem.originY
            onContentXChanged: if (orientation == Qt.Horizontal) internals.contentPosition = flickableItem.contentX - flickableItem.originX
        }
        // logic for ListView
        Component {
            id: listViewLogic
            Object {
                property real size: sectionCounter.sectionCount * sectionHeight + contentSize + spacingSize
                property int itemHeight: delegateHeight(flickableItem.delegate)
                property int sectionHeight: sectionCounter.sectionCount * delegateHeight(flickableItem.section.delegate)
                property int spacingSize: flickableItem.spacing * (flickableItem.count - 1)
                property int contentSize: flickableItem.count * itemHeight

                VisualDataModel {
                    id: sectionModel
                    model: flickableItem.section.delegate != undefined ? flickableItem.model : null
                    delegate: Item {}
                }

                Theming.ModelSectionCounter {
                    id: sectionCounter
                    model: flickableItem.section.delegate != undefined ? sectionModel : null
                    sectionProperty: flickableItem.section.property
                    sectionCriteria: flickableItem.section.criteria
                }

                function delegateHeight(delegate)
                {
                    // FIXME: this causes QML warnings because of unknown roles,
                    // but we need it for correct content calculations
                    if (delegate) {
                        var instance = delegate.createObject(null);
                        var ret = instance.height;
                        instance.destroy();
                        return ret;
                    }

                    return 0;
                }
            }
        }
        function detectFlickableLogic()
        {
            if (flickableItem) {
                if (flickableItem.hasOwnProperty("header")) {
                    // we consider Grids same as Flickables
                    if (flickableItem.hasOwnProperty("cellWidth")) {
                        loadFlickableLogic();
                    } else {
                        loadListViewLogic();
                    }
                } else
                    loadFlickableLogic();
            }
        }

        function loadFlickableLogic()
        {
            if (internals.listView)
                internals.listView.destroy();
            internals.listView = null;
        }

        function loadListViewLogic()
        {
            if (internals.listView)
                internals.listView.destroy();
            internals.listView = listViewLogic.createObject(scrollbar);
        }

    }
}
