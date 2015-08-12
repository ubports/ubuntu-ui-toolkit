/*
 * Copyright 2013-2014 Canonical Ltd.
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
import Ubuntu.Components 1.2 as Components

/*!
    \internal
    \qmltype AppHeader
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
*/
Components.StyledItem {
    id: header

    anchors {
        left: parent.left
        right: parent.right
    }
    y: 0

    /*!
      Animate showing and hiding of the header.
     */
    property bool animate: true

    /*!
      The background color of the divider. Value set by MainView.
     */
    property color dividerColor

    /*!
      The background color of the panel. Value set by MainView.
     */
    property color panelColor

    Behavior on y {
        enabled: animate && !(header.flickable && header.flickable.moving)
        SmoothedAnimation {
            duration: Components.UbuntuAnimation.BriskDuration
        }
    }

    /*! \internal */
    onHeightChanged: {
        internal.checkFlickableMargins();
        internal.movementEnded();
    }

    visible: title || contents || tabsModel
    onVisibleChanged: {
        internal.checkFlickableMargins();
    }
    enabled: header.y === 0

    /*!
      Show the header
     */
    function show() {
        // Enable the header as soon as it finished animating
        //  to the fully visible state:
        header.enabled = Qt.binding(function() { return header.y === 0; });
        header.y = 0;
    }

    /*!
      Hide the header
     */
    function hide() {
        // Disable the header immediately (the update of the y-value
        //  is delayed because of the Behavior defined on it):
        header.enabled = false;
        header.y = - header.height;
    }

    /*!
      The text to display in the header
     */
    property string title: ""
    onTitleChanged: {
        header.show();
    }

    /*!
      The contents of the header. If this is set, \l title will be ignored.
     */
    property Item contents: null
    onContentsChanged: {
        header.show();
    }

    /*!
      A model of tabs to represent in the header.
      This is automatically set by \l Tabs.
     */
    property var tabsModel: null

    /*!
      If it is possible to pop this PageStack, a back button will be
      shown in the header.
     */
    property var pageStack: null

    /*!
      \deprecated
      \qmlproperty list<Action> actions
      The list of actions actions that will be shown in the header.
      DEPRECATED. Use Page.head.actions instead.
     */
    property var actions
    onActionsChanged: print("WARNING: Header.actions property is DEPRECATED. "+
                            "Use Page.head.actions instead.")

    /*!
      \internal
      \deprecated
      Action shown before the title. Setting this will disable the back
      button and tabs drawer button in the new header and replace it with a button
      representing the action below.
      DEPRECATED. Use Page.head.backAction property instead.
     */
    property var __customBackAction: null

    // FIXME: Currently autopilot can only get visual items, but once bug #1273956
    //  is fixed to support non-visual items, a QtObject may be used.
    //  --timp - 2014-03-20
    Item {
        // FIXME: This is a workaround to be able to get the properties of
        //  tabsModel in an autopilot test.
        objectName: "tabsModelProperties"
        property int count: tabsModel ? tabsModel.count : 0
        property int selectedIndex: tabsModel ? tabsModel.selectedIndex : -1
    }
    Item {
        // FIXME: This is a workaround to be able to get the properties of
        //  the sections in an autopilot test.
        objectName: "sectionsProperties"
        property int selectedIndex: header.config ? header.config.sections.selectedIndex : -1
    }

    /*!
      The flickable that controls the movement of the header.
      Will be set automatically by Pages inside a MainView, but can
      be overridden.
     */
    property Flickable flickable: null
    onFlickableChanged: {
        internal.checkFlickableMargins();
        internal.connectFlickable();
        header.show();
    }

    /*!
      Set by \l MainView
     */
    property bool useDeprecatedToolbar: true

    /*!
      Configuration of the header.
     */
    property PageHeadConfiguration config: null

    /*!
      The header is not fully opened or fully closed.

      This property is true if the header is animating towards a fully
      opened or fully closed state, or if the header is moving due to user
      interaction with the flickable.

      The value of moving is always false when using an old version of
      PageHeadConfiguration (which does not have the visible property).

      Used in tst_header_locked_visible.qml.
    */
    readonly property bool moving: false

    QtObject {
        id: internal

        /*!
          Track the y-position inside the flickable.
         */
        property real previousContentY: 0

        /*!
          The previous flickable to disconnect events
         */
        property Flickable previousFlickable: null

        /*!
          Disconnect previous flickable, and connect the new one.
         */
        function connectFlickable() {
            if (previousFlickable) {
                previousFlickable.contentYChanged.disconnect(internal.scrollContents);
                previousFlickable.movementEnded.disconnect(internal.movementEnded);
                previousFlickable.interactiveChanged.disconnect(internal.interactiveChanged);
            }
            if (flickable) {
                // Connect flicking to movements of the header
                previousContentY = flickable.contentY;
                flickable.contentYChanged.connect(internal.scrollContents);
                flickable.movementEnded.connect(internal.movementEnded);
                flickable.interactiveChanged.connect(internal.interactiveChanged);
                flickable.contentHeightChanged.connect(internal.contentHeightChanged);
            }
            previousFlickable = flickable;
        }

        /*!
          Update the position of the header to scroll with the flickable.
         */
        function scrollContents() {
            // Avoid updating header.y when rebounding or being dragged over the bounds.
            if (!flickable.atYBeginning && !flickable.atYEnd) {
                var deltaContentY = flickable.contentY - previousContentY;
                // FIXME: MathUtils.clamp  is expensive. Fix clamp, or replace it here.
                header.y = MathUtils.clamp(header.y - deltaContentY, -header.height, 0);
            }
            previousContentY = flickable.contentY;
        }

        /*!
          Fully show or hide the header, depending on its current y.
         */
        function movementEnded() {
            if (flickable && flickable.contentY < 0) header.show();
            else if (header.y < -header.height/2) header.hide();
            else header.show();
        }

        /*
          Content height of flickable changed
         */
        function contentHeightChanged() {
            if (flickable && flickable.height >= flickable.contentHeight) header.show();
        }

        /*
          Flickable became interactive or non-interactive.
         */
        function interactiveChanged() {
            if (flickable && !flickable.interactive) header.show();
        }

        /*
          Check the topMargin of the flickable and set it if needed to avoid
          contents becoming unavailable behind the header.
         */
        function checkFlickableMargins() {
            if (header.flickable) {
                var headerHeight = header.visible ? header.height : 0
                if (flickable.topMargin !== headerHeight) {
                    var previousHeaderHeight = flickable.topMargin;
                    flickable.topMargin = headerHeight;
                    // push down contents when header grows,
                    // pull up contents when header shrinks.
                    flickable.contentY -= headerHeight - previousHeaderHeight;
                }
            }
        }
    }

    style: header.useDeprecatedToolbar ? Theme.createStyleComponent("HeaderStyle.qml", header) :
                                         Theme.createStyleComponent("PageHeadStyle.qml", header)
}
