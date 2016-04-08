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
import Ubuntu.Components 1.3

/*!
    \qmltype ScrollView
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \inherits StyledItem
    \brief ScrollView is a scrollable view that features scrollbars and scrolling via keyboard keys.

    ScrollView is the recommended component to use in the implementation of scrollable content. It
    features scrollbars and handles keyboard input.
    Old applications that implemented scrollable views directly using the \l Scrollbar component should
    migrate to ScrollView, to ensure the UX is ready for convergent devices and is consistent with the
    rest of the platform.

    ScrollView wraps \l {Scrollbar}(s) in a view, and provides additional features such as:
    \list
    \li - Keyboard navigation and focus handling, for a complete convergent experience.
    \li - Automatic position handling of vertical and horizontal scrollbars, preventing them from
          overlapping one another when both on screen.
    \endlist

    Adding scrollbars and keyboard input handling to a QML item is as simple as wrapping that item in a
    ScrollView, as shown in the following example:

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    ScrollView {
        width: units.gu(40)
        height: units.gu(30)

        Rectangle {
            width: units.gu(140)
            height: units.gu(40)
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightsteelblue" }
                GradientStop { position: 1.0; color: "blue" }
            }
        }
    }
    \endqml

    NOTE: the items that are wrapped in the ScrollView are reparanted to \l viewport.
*/
StyledItem {
    id: root

    implicitWidth: 240
    implicitHeight: 150
    activeFocusOnPress: true

    //TODO: add horizontalScrollbarPolicy
    //TODO: add verticalScrollbarPolicy

    //Currently we want to push the platform towards overlay scrollbars
    //readonly property bool alwaysOnScrollbars: false

    /*!
        \qmlproperty Item ScrollView::viewport
        This property holds the viewport Item. The children of the ScrollView element are
        reparented to this item to make sure the scrollbars are correctly positioned and
        the items are clipped at their boundaries.
    */
    readonly property alias viewport: viewportItem

    /*!
        \qmlproperty Item ScrollView::flickableItem
        The flickableItem of the ScrollView. If the contentItem provided
        to the ScrollView is a Flickable, that will be the \l flickableItem.
        Otherwise ScrollView will create a Flickable which will hold the
        items provided as children.
    */
    readonly property alias flickableItem: internal.flickableItem

    /*!
        The contentItem of the ScrollView. This is set by the user.
        Note that the definition of contentItem is somewhat different to that
        of a Flickable, where the contentItem is implicitly created.
    */
    default property Item contentItem

    Keys.enabled: true
    Keys.onLeftPressed: {
        if (horizontalScrollbar.__styleInstance !== null) {
            horizontalScrollbar.__styleInstance.scrollBy(-flickableItem.width*internal.shortScrollingRatio, true)
        }
    }
    Keys.onRightPressed: {
        if (horizontalScrollbar.__styleInstance !== null) {
            horizontalScrollbar.__styleInstance.scrollBy(flickableItem.width*internal.shortScrollingRatio, true)
        }
    }
    Keys.onDownPressed: {
        if (verticalScrollbar.__styleInstance !== null) {
            verticalScrollbar.__styleInstance.scrollBy(flickableItem.height*internal.shortScrollingRatio, true)
        }
    }
    Keys.onUpPressed: {
        if (verticalScrollbar.__styleInstance !== null) {
            verticalScrollbar.__styleInstance.scrollBy(-flickableItem.height*internal.shortScrollingRatio, true)
        }
    }
    Keys.onPressed:  {
        if (event.key == Qt.Key_Escape) {
            var scrollbarWithActiveDrag = (horizontalScrollbar.__styleInstance && horizontalScrollbar.__styleInstance.draggingThumb && horizontalScrollbar)
                    || (verticalScrollbar.__styleInstance && verticalScrollbar.__styleInstance.draggingThumb && verticalScrollbar)
                    || null
            if (scrollbarWithActiveDrag !== null) {
                scrollbarWithActiveDrag.__styleInstance.resetScrollingToPreDrag()
                event.accepted = true
            }
        } else if (verticalScrollbar.__styleInstance !== null) {
            if (event.key == Qt.Key_PageDown) {
                verticalScrollbar.__styleInstance.scrollBy(flickableItem.height*internal.longScrollingRatio, true)
                event.accepted = true
            } else if (event.key == Qt.Key_PageUp) {
                verticalScrollbar.__styleInstance.scrollBy(-flickableItem.height*internal.longScrollingRatio, true)
                event.accepted = true
            } else if (event.key == Qt.Key_Home) {
                verticalScrollbar.__styleInstance.scrollToBeginning(true)
                event.accepted = true
            } else if (event.key == Qt.Key_End) {
                verticalScrollbar.__styleInstance.scrollToEnd(true)
                event.accepted = true
            }
        }
    }

    onContentItemChanged: {
        // Check if the item provided is a Flickable
        if (QuickUtils.inherits(contentItem, "QQuickFlickable")) {
            internal.flickableItem = contentItem
            internal.flickableItem.parent = viewportItem
        } else {
            //Create a dummy Flickable if the app dev didn't provide any
            internal.flickableItem = flickableComponent.createObject(viewportItem)
            contentItem.parent = internal.flickableItem.contentItem
        }
        internal.flickableItem.anchors.fill = viewportItem
    }

    /*!
        \qmlproperty Scrollbar ScrollView::horizontalScrollbar

        This property holds the horizontal scrollbar used in the ScrollView.
        It provides access to the scrollbar thus making it possible to change its properties or
        completely replace it with a custom \l Scrollbar object.

        This property defaults to an instance of \l Scrollbar.

        \b NOTE: some \l Scrollbar properties are already handled by ScrollView, and their value
        should hence not be overridden. These properties are:
        \list
        \li anchors margins, such as anchors.topMargin etc.
        \li the parent Item
        \li \l Scrollbar::flickableItem
        \endlist
    */
    property Scrollbar horizontalScrollbar: Scrollbar {
        objectName: "horizontalScrollbar"
        align: Qt.AlignBottom
        focus: false
    }

    /*!
        \qmlproperty Scrollbar ScrollView::verticalScrollbar

        This property holds the vertical scrollbar used in the ScrollView.
        It provides access to the scrollbar thus making it possible to change its properties or
        completely replace it with a custom \l Scrollbar object.

        This property defaults to an instance of \l Scrollbar.

        \b NOTE: some \l Scrollbar properties are already handled by ScrollView, and their value
        should hence not be overridden. These properties are:
        \list
        \li anchors margins, such as anchors.topMargin etc.
        \li the parent Item
        \li \l Scrollbar::flickableItem
        \endlist
    */
    property Scrollbar verticalScrollbar: Scrollbar {
        objectName: "verticalScrollbar"
        align: Qt.AlignTrailing
        focus: false
    }

    children: [
        Item {
            id: viewportItem
            anchors.fill: parent
            anchors.topMargin: (horizontalScrollbar.align === Qt.AlignTop && horizontalScrollbar.__alwaysOnScrollbars)
                               ? internal.nonOverlayScrollbarMargin : 0
            anchors.leftMargin: (verticalScrollbar.align === Qt.AlignLeading && horizontalScrollbar.__alwaysOnScrollbars)
                                ? internal.nonOverlayScrollbarMargin : 0
            anchors.rightMargin: (verticalScrollbar.align === Qt.AlignTrailing && horizontalScrollbar.__alwaysOnScrollbars)
                                 ? internal.nonOverlayScrollbarMargin : 0
            anchors.bottomMargin: (horizontalScrollbar.align === Qt.AlignBottom && horizontalScrollbar.__alwaysOnScrollbars)
                                  ? internal.nonOverlayScrollbarMargin : 0

            clip: true
            focus: true
        },

        Item {
            id: internal
            anchors.fill: parent

            property Flickable flickableItem
            property real nonOverlayScrollbarMargin: verticalScrollbar.__styleInstance ? verticalScrollbar.__styleInstance.nonOverlayScrollbarMargin : 0
            //shortScrollingRation is used for arrow keys, longScrollingRatio is used for pgUp/pgDown
            //0.1 means we will scroll 10% of the *visible* flickable area
            property real shortScrollingRatio: __styleInstance ? __styleInstance.shortScrollingRatio : 0.1
            property real longScrollingRatio: __styleInstance ? __styleInstance.longScrollingRatio : 0.9

            Component {
                id: flickableComponent
                Flickable {}
            }

            //if the flickable is not coming from the user but from our internal Component...
            Binding {
                target: flickableItem
                when: contentItem !== flickableItem
                property: "contentHeight"
                value: contentItem ? contentItem.height : 0
            }
            Binding {
                target: flickableItem
                when: contentItem !== flickableItem
                property: "contentWidth"
                value: contentItem ? contentItem.width : 0
            }

            Binding {
                when: root.verticalScrollbar
                target: root.verticalScrollbar
                property: "parent"
                value: root
            }
            Binding {
                when: root.horizontalScrollbar
                target: root.horizontalScrollbar
                property: "parent"
                value: root
            }

            //The viewport MUST be set, when Scrollbar is used inside a ScrollView, because that is the
            //item it will anchor to.
            Binding {
                //NOTE: we have to wait for the parent to be set before setting the viewport item or that
                //will trigger anchoring errors
                when: root.horizontalScrollbar && root.horizontalScrollbar.parent
                target: root.horizontalScrollbar
                property: "__viewport"
                value: viewportItem
            }
            Binding {
                when: root.verticalScrollbar && root.verticalScrollbar.parent
                target: root.verticalScrollbar
                property: "__viewport"
                value: viewportItem
            }

            Binding {
                //wait for viewport to be set, otherwise Scrollbar will try anchoring to flickableItem, which
                //is not a parent or sibling (see definition of Scrollbar anchors) when Scrollbar is inside
                //a ScrollView (it is a sibling usually when Scrollbar is used directly, without ScrollView)
                when: root.horizontalScrollbar && root.horizontalScrollbar.__viewport
                target: root.horizontalScrollbar
                property: "flickableItem"
                value: internal.flickableItem
            }
            Binding {
                when: root.verticalScrollbar && root.verticalScrollbar.__viewport
                target: root.verticalScrollbar
                property: "flickableItem"
                value: internal.flickableItem
            }

            Binding {
                when: root.verticalScrollbar
                target: root.verticalScrollbar
                property: "__buddyScrollbar"
                value: root.horizontalScrollbar
            }
            Binding {
                when: root.horizontalScrollbar
                target: root.horizontalScrollbar
                property: "__buddyScrollbar"
                value: root.verticalScrollbar
            }

            //When you click outside of a child of the scrollview, this restores the focus to the ScrollView
            //Why is this needed?
            //  Suppose the viewport has a child ScrollView (or another Item which handles some or all of the hw keys we handle).
            //  Now, if that child is coded to set its focus to true when you tap on it (which is, for instance, what
            //  StyledItem's activeFocusOnPress does), tapping on that child will set viewportItem.focus = false, because
            //  that child is just another Item inside "root" and "root" is a FocusScope, and that's how FocusScope works.
            //  At this point, we need something to restore the focus to the viewportItem once you tap on viewportItem, outside
            //  any other child. And that's what the MouseArea below does.
            //
            //The alternative could be making the viewport a StyledItem and using activeFocusOnPress:true, but that wouldn't work
            //because it would make viewportItem a focus scope, and we don't want that, because at that point,
            //being in a similar situation as described above, we wouldn't be able to transfer the focus to viewportItem, because
            //being a focus scope means viewportItem would forward the focus to the one between its children who asked it most
            //recently
            MouseArea {
                anchors.fill: parent
                onPressed: {
                    //FIXME: we can't force focus on the viewport, this breaks text selection inside scrollview!
                    //+ it still doesn't work in Sliders page, for yet unknown reasons
                    //Without this we can't shift focus from a child ScrollView to a parent ScrollView
                    //viewportItem.focus = true

                    //activeFocusOnPress only works if *all* the parents have activeFocusOnPress enabled
                    //Some applications (like messaging-app at the moment) have activeFocusOnPress:false on
                    //the MainView, and that would make focusing the scrollview impossible. Hence we also
                    //force it from QML side
                    root.forceActiveFocus()

                    mouse.accepted = false
                }
            }
        }
    ]
}
