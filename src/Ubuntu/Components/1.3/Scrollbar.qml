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
import Ubuntu.Components 1.3 as Toolkit

/*!
    \qmltype Scrollbar
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief The Scrollbar component provides scrolling functionality for
    scrollable views (i.e. Flickable, ListView).

    \b NOTE: the Scrollbar component was revamped for OTA9 and used for the implementation
    of \l ScrollView. Apps targeting system version OTA9 (or newer) should use ScrollView instead
    of Scrollbar, as it features convergent-ready features, such as (but not limited to)
    keyboard input handling.

    The Scrollbar can be set to any flickable and has built-in anchoring setup
    to the attached flickable's front, rear, top or bottom. The scrollbar can
    also be aligned using anchors, however the built-in align functionality
    makes sure to have the proper alignemt applied based on theme and layout
    direction (RTL or LTR).

    The content position is driven through the attached Flickable. Therefore every
    style implementation should drive the position through contentX/contentY
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
        Scrollbar {
            flickableItem: list
            align: Qt.AlignTrailing
        }
    }
    \endqml

    \section1 Vertical Scrollbar and Flickable behaviour

    Since Flickable's topMargin and bottomMargin are modified by Ubuntu components
    such as \l BottomEdgeHint and \l Header in their positioning logic, the Scrollbar component
    uses the value of those properties to its top and bottom anchors margins, by default. This is
    to prevent any overlap with BottomEdgeHint or Header (see \l {Vertical Scrollbar and Header behaviour}
    for more info about that). In case you need to specify custom Flickable
    content margins (note: we're talking about content margins, not anchors ones), you will also have to
    override the top and bottom anchors margins of the Scrollbar accordingly.

    \section1 Vertical Scrollbar and Header behaviour

    The thumb of the Scrollbar should not move or resize unexpectedly. It would be confusing,
    for instance, if it moved under the user's finger (or pointer) while he's dragging it.

    Because the size and position of the thumb in a scrollbar is related to the size of the
    trough (or track) it slides in, it is important that the trough does not resize or move while the
    user is interacting with the component.

    In the context of a \l Page with a \l Header that slides in and out dynamically when the user
    flicks the surface (see \l {Header::flickable}), a vertical Scrollbar that is linked
    to the same flickable surface as the Header behaves as described below:

    \list
    \li - when the Header is exposed, the Scrollbar component sits right below it, \
          thus avoiding overlaps;

    \li - when the Header hides due to the user either flicking the surface or dragging \
          the thumb, the trough of the Scrollbar does not resize or move, thus avoiding \
          unexpected changes in thumb's position or size. As a side effect, the space \
          above the Scrollbar, previously occupied by Header, stays empty until the Header \
          is exposed again.

    \endlist
    \br
    This behaviour is intended and makes for a less confusing User Experience.
  */

Toolkit.StyledItem {
    id: scrollbar

    /*!
        \qmlproperty Flickable Scrollbar::flickableItem
        This property holds the flickable item (Flickable, ListView or GridView)
        the Scrollbar is attached to.
      */
    property Flickable flickableItem: null

    /*
        This property holds the other scrollbar that is attached to the same flickable,
        if any. For instance, if this scrollbar is horizontal, buddyScrollbar must be set
        to the vertical scrollbar, if any. This is to allow a correct layout of both
        horizontal and vertical scrollbars when a view is scrollable in both directions.
    */
    //can't use property Scrollbar here as it would complain "Scrollbar instantiated recursively"
    property var __buddyScrollbar: null

    /*!
      \qmlproperty int Scrollbar::align
      The property defines the alignment of the scrollbar to the flickableItem.
      The implementation handles the alignment as follows:
        \list
        \li Qt.AlignLeading anchors to the left on LTR and to the right on RTL layouts
        \li Qt.AlignTrailing anchors to the right on LTR and to the left on RTL layouts
        \li Qt.AlignTop anchors to the top
        \li Qt.AlignBottom anchors to the bottom
        \endlist
        The default value is \b Qt.AlignTrailing.
      */
    property int align: Qt.AlignTrailing

    /*!
      \internal
      This property holds whether the scrollbar is active or passive. It is present
      for testing purposes.
    */
    property bool __interactive: __styleInstance !== null && __styleInstance.interactive


    /*!
      \internal
      This trough of the scrollbar, it is used to define the position of the slider.
    */
    property Item __trough: __styleInstance !== null && __styleInstance.trough


    /*!
      \internal
      simulate the system setting (which will be implemented in unity8, I guess)
      True --> Steppers style, non-overlay scrollbars
      False --> Indicator and Trough styles
      CURRENTLY UNUSED
    */
    property bool __alwaysOnScrollbars: false

    /*! internal
      Used by ScrollView to tweak Scrollbar's anchoring logic for the always-on scrollbars.
      CURRENTLY UNUSED
    */
    property Item __viewport: null

    //Disable the input handling to let the events pass through in case we have an
    //interactive scrollbar right below us (can happen with nested views)
    enabled: __interactive//&& __alwaysOnScrollbars

    implicitWidth: internals.vertical ? units.gu(3) : (flickableItem ? flickableItem.width : 0)
    implicitHeight: !internals.vertical ? units.gu(3) : (flickableItem ? flickableItem.height : 0)

    anchors {
        left: (__viewport || flickableItem)
              ? internals.leftAnchor(__viewport ? __viewport : flickableItem)
              : undefined
        leftMargin: internals.leftAnchorMargin()
        right: (__viewport || flickableItem)
               ? internals.rightAnchor(__viewport ? __viewport : flickableItem)
               : undefined
        rightMargin: internals.rightAnchorMargin()
        top: (__viewport || flickableItem)
             ? internals.topAnchor(__viewport ? __viewport : flickableItem)
             : undefined
        topMargin: (flickableItem ? flickableItem.topMargin : 0) + internals.topAnchorMargin()
        bottom: (__viewport || flickableItem)
                ? internals.bottomAnchor(__viewport ? __viewport : flickableItem)
                : undefined
        bottomMargin: (flickableItem ? flickableItem.bottomMargin : 0) + internals.bottomAnchorMargin()
    }

    /*!
      \internal
      */
    onAlignChanged: if (!internals.checkAlign()) console.log("Wrong alignment set to Scrollbar: "+align)

    /*!
      \internal
      Internals: contains the common logic of the scrollbar like anchoring,
      alignemt check, scrollability check.
    */
    property alias __private: internals
    QtObject {
        id: internals
        property bool vertical: (align === Qt.AlignLeading) || (align === Qt.AlignTrailing)
        property bool scrollable: flickableItem && flickableItem.interactive && checkAlign()
        property real nonOverlayScrollbarMargin: __styleInstance ? __styleInstance.nonOverlayScrollbarMargin : 0

        function checkAlign()
        {
            return (align === Qt.AlignLeading) || (align === Qt.AlignTrailing) || (align === Qt.AlignTop) || (align === Qt.AlignBottom);
        }

        // LTR and RTL are provided by LayoutMirroring, so no need to check that
        function leftAnchor(object)
        {
            if (!internals.vertical || (align == Qt.AlignLeading)) {
                return object.left;
            }
            return undefined;
        }
        function leftAnchorMargin()
        {
            if (__styleInstance === null) return 0

            switch (align) {
            case Qt.AlignLeading:
                return __alwaysOnScrollbars ? -nonOverlayScrollbarMargin : 0
            case Qt.AlignBottom:
            case Qt.AlignTop:
                if (!__alwaysOnScrollbars && __buddyScrollbar !== null
                        && __buddyScrollbar.align === Qt.AlignLeading
                        && __buddyScrollbar.__styleInstance.isScrollable)
                    return __buddyScrollbar.__styleInstance.troughThicknessSteppersStyle
                // *ELSE FALLTHROUGH*
            default:
                return 0
            }
        }
        function rightAnchor(object)
        {
            if (!internals.vertical || (align == Qt.AlignTrailing)) {
                return object.right;
            }
            return undefined;
        }
        function rightAnchorMargin()
        {
            if (__styleInstance === null) return 0

            switch (align) {
            case Qt.AlignTrailing:
                return __alwaysOnScrollbars ? -nonOverlayScrollbarMargin : 0
            case Qt.AlignBottom:
            case Qt.AlignTop:
                if (!__alwaysOnScrollbars && __buddyScrollbar !== null
                        && __buddyScrollbar.align === Qt.AlignTrailing
                        && __buddyScrollbar.__styleInstance.isScrollable)
                    return __buddyScrollbar.__styleInstance.troughThicknessSteppersStyle
                // *ELSE FALLTHROUGH*
            default:
                return 0
            }
        }
        function topAnchor(object)
        {
            if (internals.vertical || (align == Qt.AlignTop))
                return object.top;
            return undefined;
        }
        function topAnchorMargin()
        {
            if (__styleInstance === null) return 0

            switch (align) {
            case Qt.AlignTop:
                return __alwaysOnScrollbars ? -nonOverlayScrollbarMargin : 0
            case Qt.AlignLeading:
            case Qt.AlignTrailing:
                if (!__alwaysOnScrollbars && __buddyScrollbar !== null
                        && __buddyScrollbar.align === Qt.AlignTop
                        && __buddyScrollbar.__styleInstance.isScrollable)
                    return __buddyScrollbar.__styleInstance.troughThicknessSteppersStyle
                // *ELSE FALLTHROUGH*

            default:
                return 0
            }
        }
        function bottomAnchor(object)
        {
            if (internals.vertical || (align == Qt.AlignBottom))
                return object.bottom;
            return undefined;
        }
        function bottomAnchorMargin()
        {
            if (__styleInstance === null) return 0

            switch (align) {
            case Qt.AlignBottom:
                return __alwaysOnScrollbars ? -nonOverlayScrollbarMargin : 0
            case Qt.AlignLeading:
            case Qt.AlignTrailing:
                if (!__alwaysOnScrollbars && __buddyScrollbar !== null
                        && __buddyScrollbar.align === Qt.AlignBottom
                        && __buddyScrollbar.__styleInstance.isScrollable)
                    return __buddyScrollbar.__styleInstance.troughThicknessSteppersStyle
                // *ELSE FALLTHROUGH*
            default:
                return 0
            }
        }
    }

    styleName: "ScrollbarStyle"
}
