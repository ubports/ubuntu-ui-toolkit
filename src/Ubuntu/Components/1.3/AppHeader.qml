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
import Ubuntu.Components 1.3 as Components

/*!
    \internal
    \qmltype AppHeader
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
*/
Components.Header {
    id: header

    anchors {
        left: parent.left
        right: parent.right
    }
//    y: exposed ? 0 : -height

    // Workaround for when the header must be hidden initially.
    // If the height changes later, the y-value will be animated.
//    height: __styleInstance ? __styleInstance.implicitHeight : units.gu(6) + units.dp(1)
//onHeightChanged: print("AppHeader.height = "+height)

    /*!
      The background color of the divider. Value set by MainView.
     */
    property color dividerColor

    /*!
      The background color of the panel. Value set by MainView.
     */
    property color panelColor

    enabled: header.exposed && !header.moving

//    exposed: !header.config || header.config.visible
    exposed: false
//    flickable: header.config && !header.config.locked ? header.config.flickable : null
//    onFlickableChanged: print("new flickable = "+flickable)
    onExposedChanged: {
//        print("exposedChanged! to "+exposed+", flickable = "+flickable)
        if(header.config) {
            header.config.visible = exposed;
        }
    }
//    Component.onCompleted: print("WTF.exposed = "+header.exposed+", y = "+header.y)
//onYChanged: print("header.y = "+header.y)
//    /*!
//      Show the header
//     */
//    function show() {
//        if (internal.newConfig) {
//            header.config.visible = true;
//        }
//        // Enable the header as soon as it finished animating
//        //  to the fully visible state:
//        header.enabled = Qt.binding(function() { return header.y === 0; });
//        header.y = 0;
//    }

//    /*!
//      Hide the header
//     */
//    function hide() {
//        if (internal.newConfig) {
//            header.config.visible = false;
//        }
//        // Disable the header immediately (the update of the y-value
//        //  is delayed because of the Behavior defined on it):
//        header.enabled = false;
//        header.y = -header.height;
//    }

    /*!
      The text to display in the header
     */
    property string title: ""

    /*!
      The contents of the header. If this is set, \l title will be ignored.
      DEPRECATED and replaced by Page.head.contents.
     */
    property Item contents: null

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
      Configuration of the header.
      FIXME: Must be of type PageHeadConfiguration. Setting that as the property type
      however will use the latest version (1.3) and a Page that uses an older
      version (1.1) will no longer work.
     */
    property QtObject config: null
    onConfigChanged: {
//        print("Config changed! locked = "+config.locked+", visible = "+config.visible)
//        print("objectName = "+config.objectName)
        // set internal.newConfig because when we rely on the binding,
        //  the value of newConfig may be updated after executing the code below.
        internal.newConfig = config && config.hasOwnProperty("visible") &&
                config.hasOwnProperty("locked");

        if (header.config.locked) {
            header.flickable = null;
        } else {
            header.flickable = header.config.flickable;
        }

//        if (internal.newConfig && header.config.locked && !header.config.visible) {
        if (!header.flickable && !header.config.visible) {
            // locked.
//            print("locked hidden. setting exposed to false.");
            header.exposed = false;
        } else {
//            print("not locked. exposing header.")
            header.config.visible = true;
            header.exposed = true;
        }
    }
    Connections {
        target: header.config
        ignoreUnknownSignals: true // PageHeadConfiguration <1.2 lacks the signals below
        onVisibleChanged: {
//            print("head.visible changed to "+header.config.visible)
            if (header.config.visible) {
                header.exposed = true; //header.config.visible;
            } else {
                header.exposed = false;
            }
        }
        onLockedChanged: {
            if (header.config.locked) {
                header.flickable = null;
            } else {
                header.flickable = header.config.flickable;
            }
        }
    }


    QtObject {
        id: internal

        // This property is updated in header.onConfigChanged to ensure it
        //  is updated before other functions are called in onConfigChanged.
        property bool newConfig: header.config &&
                                 header.config.hasOwnProperty("locked") &&
                                 header.config.hasOwnProperty("visible")

        /*!
          Disconnect previous flickable, and connect the new one.
         */
//        function connectFlickable() {
//            // Finish the current header movement in case the current
//            // flickable is disconnected while scrolling:
//            internal.movementEnded();

//            if (previousFlickable) {
//                previousFlickable.contentYChanged.disconnect(internal.scrollContents);
//                previousFlickable.movementEnded.disconnect(internal.movementEnded);
//                previousFlickable.interactiveChanged.disconnect(internal.interactiveChanged);
//                previousFlickable = null;
//            }
//            if (flickable && !(internal.newConfig && header.config.locked)) {
//                // Connect flicking to movements of the header
//                previousContentY = flickable.contentY;
//                flickable.contentYChanged.connect(internal.scrollContents);
//                flickable.movementEnded.connect(internal.movementEnded);
//                flickable.interactiveChanged.connect(internal.interactiveChanged);
//                flickable.contentHeightChanged.connect(internal.contentHeightChanged);
//                previousFlickable = flickable;
//            }
//            internal.checkFlickableMargins();
        }

    theme.version: Components.Ubuntu.toolkitVersion
    styleName: "PageHeadStyle"
}
