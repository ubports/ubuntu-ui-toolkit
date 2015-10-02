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

    /*!
      The background color of the divider. Value set by MainView.
     */
    property color dividerColor

    /*!
      The background color of the panel. Value set by MainView.
     */
    property color panelColor

    // prevent triggering buttons in the header when it is moving
    enabled: header.exposed && !header.moving

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
        if (header.config.locked) {
            header.flickable = null;
        } else {
            header.flickable = header.config.flickable;
        }

        if (!header.flickable && !header.config.visible) {
            // locked.
            header.exposed = false;
        } else {
            header.config.visible = true;
            header.exposed = true;
        }
    }
    onExposedChanged: {
        if(header.config) {
            header.config.visible = exposed;
        }
    }
    Connections {
        target: header.config
        ignoreUnknownSignals: true
        onVisibleChanged: {
            header.exposed = header.config.visible;
        }
        onLockedChanged: {
            if (header.config.locked) {
                header.flickable = null;
            } else {
                header.flickable = header.config.flickable;
            }
        }
        onFlickableChanged: {
            if (!header.config.locked) {
                header.flickable = header.config.flickable;
            }
        }
    }

    theme.version: Components.Ubuntu.toolkitVersion
    styleName: "PageHeadStyle"
}
