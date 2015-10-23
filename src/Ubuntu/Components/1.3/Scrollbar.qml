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
    \qmltype ScrollBar
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief The ScrollBar component provides scrolling functionality for
    scrollable views (i.e. Flickable, ListView).

    The ScrollBar can be set to any flickable and has built-in anchoring setup
    to the attached flickable's front, rear, top or bottom. the scrollbar can
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
  */

Toolkit.StyledItem {
    id: scrollbar

    /*!
        This property holds the flickable item (Flickable, ListView or GridView)
        the Scrollbar is attached to.
      */
    property Flickable flickableItem: null

    /*!
        This property holds the other scrollbar that is attached to the same flickable,
        if any. For instance, if this scrollbar is horizontal, buddyScrollbar must be set
        to the vertical scrollbar, if any. This is to allow a correct layout of both
        horizontal and vertical scrollbars.
    */
    //can't use property Scrollbar here as it would complain "Scrollbar instantiated recursively"
    property var buddyScrollbar: null

    /*!
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
    */
    property bool __alwaysOnScrollbars: false

    focus: true
    Keys.enabled: true
    Keys.onLeftPressed: {
        console.log("Left pressed")
        if (__styleInstance !== null && !internals.vertical) {
            __styleInstance.scroll(-flickableItem.width*0.05)
        }
    }
    Keys.onRightPressed: {
        console.log("Right pressed")
        if (__styleInstance !== null && !internals.vertical) {
            __styleInstance.scroll(flickableItem.width*0.05)
        }
    }
    Keys.onDownPressed: {
        console.log("Down pressed")
        if (__styleInstance !== null && internals.vertical) {
            __styleInstance.scroll(flickableItem.height*0.05)
        }
    }
    Keys.onUpPressed: {
        console.log("Up pressed")
        if (__styleInstance !== null && internals.vertical) {
            __styleInstance.scroll(-flickableItem.height*0.05)
        }
    }
    Keys.onPressed:  {
        console.log("Pressed")
        if (__styleInstance !== null) {
            if (internals.vertical) {
                if (event.key == Qt.Key_PageDown) {
                    __styleInstance.scroll(flickableItem.height*0.9)
                } else if (event.key == Qt.Key_PageUp) {
                    __styleInstance.scroll(-flickableItem.height*0.9)
                }
            } else {
                if (event.key == Qt.Key_PageDown) {
                    __styleInstance.scroll(flickableItem.width*0.9)
                } else if (event.key == Qt.Key_PageUp) {
                    __styleInstance.scroll(-flickableItem.width*0.9)
                }
            }

            if (event.key == Qt.Key_Home) {
                __styleInstance.scrollToBeginning()
            } else if (event.key == Qt.Key_End) {
                __styleInstance.scrollToEnd()
            }
        }
    }

    implicitWidth: internals.vertical ? units.gu(4) : flickableItem.width
    implicitHeight: !internals.vertical ? units.gu(4) : flickableItem.height

    anchors {
        left: internals.leftAnchor(flickableItem)
        leftMargin: internals.leftAnchorMargin()
        right: internals.rightAnchor(flickableItem)
        rightMargin: internals.rightAnchorMargin()
        top: internals.topAnchor(flickableItem)
        topMargin: internals.topAnchorMargin()
        bottom: internals.bottomAnchor(flickableItem)
        bottomMargin: internals.bottomAnchorMargin()
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
                return __alwaysOnScrollbars ? 0 : __styleInstance.marginFromEdge
            case Qt.AlignBottom:
            case Qt.AlignTop:
                if (!__alwaysOnScrollbars) {
                    if (buddyScrollbar !== null
                            && buddyScrollbar.__styleInstance !== null
                            && buddyScrollbar.align === Qt.AlignLeading)
                        return buddyScrollbar.__styleInstance.troughThicknessThumbStyle
                                + buddyScrollbar.__styleInstance.marginFromEdge
                    else
                        return __styleInstance.marginFromEdge
                }
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
                return __alwaysOnScrollbars ? 0 : __styleInstance.marginFromEdge
            case Qt.AlignBottom:
            case Qt.AlignTop:
                if (!__alwaysOnScrollbars) {
                    if (buddyScrollbar !== null
                            && buddyScrollbar.align === Qt.AlignTrailing)
                        return buddyScrollbar.__styleInstance.troughThicknessThumbStyle
                                + buddyScrollbar.__styleInstance.marginFromEdge
                    else
                        return __styleInstance.marginFromEdge
                }
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
                return __alwaysOnScrollbars ? 0 : __styleInstance.marginFromEdge
            case Qt.AlignLeading:
            case Qt.AlignTrailing:
                if (!__alwaysOnScrollbars) {
                    if (buddyScrollbar !== null
                            && buddyScrollbar.align === Qt.AlignTop)
                        return buddyScrollbar.__styleInstance.troughThicknessThumbStyle
                                + buddyScrollbar.__styleInstance.marginFromEdge
                    else
                        return __styleInstance.marginFromEdge
                }
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
                return __alwaysOnScrollbars ? 0 : __styleInstance.marginFromEdge
            case Qt.AlignLeading:
            case Qt.AlignTrailing:
                if (!__alwaysOnScrollbars) {
                    if (buddyScrollbar !== null
                            && buddyScrollbar.align === Qt.AlignBottom)
                        return buddyScrollbar.__styleInstance.troughThicknessThumbStyle
                                + buddyScrollbar.__styleInstance.marginFromEdge
                    else
                        return __styleInstance.marginFromEdge
                }
            default:
                return 0
            }
        }
    }

    //NEEDED FOR NON-OVERLAY SCROLLBARS
    Binding {
        when: flickableItem && __alwaysOnScrollbars
              && __styleInstance && align === Qt.AlignTrailing
              && flickableItem.rightMargin < __styleInstance.troughThicknessSteppersStyle
        target: flickableItem
        property: LayoutMirroring.enabled ? "leftMargin" : "rightMargin"
        value: __styleInstance.troughThicknessSteppersStyle
    }
    Binding {
        when: flickableItem && __alwaysOnScrollbars
              && __styleInstance && align === Qt.AlignLeading
              && flickableItem.leftMargin < __styleInstance.troughThicknessSteppersStyle
        target: flickableItem
        property: LayoutMirroring.enabled ? "rightMargin" : "leftMargin"
        value: __styleInstance.troughThicknessSteppersStyle
    }
    Binding {
        when: flickableItem && __alwaysOnScrollbars
              && __styleInstance && align === Qt.AlignTop
              && flickableItem.topMargin < __styleInstance.troughThicknessSteppersStyle
        target: flickableItem
        property: "topMargin"
        value:__styleInstance.troughThicknessSteppersStyle
    }
    Binding {
        when: flickableItem && __alwaysOnScrollbars
              && __styleInstance && align === Qt.AlignBottom
              && flickableItem.bottomMargin < __styleInstance.troughThicknessSteppersStyle
        target: flickableItem
        property: "bottomMargin"
        value: __styleInstance.troughThicknessSteppersStyle
    }

    theme.version: Toolkit.Ubuntu.toolkitVersion
    styleName: "ScrollbarStyle"
}
