/*
 * Copyright (C) 2013 Canonical Ltd.
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

/*!
    \internal
    \qmltype Toolbar
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Application toolbar. This class is not exposed because it will
            be automatically added when a Page defines tools.
*/
Panel {
    id: toolbar
    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }
    height: background.height

    // Closing of the toolbar on app contents ineraction is handled by the Page.
    __closeOnContentsClicks: false

    /*!
      \preliminary
      The list of \l Actions to be shown on the toolbar
     */
    property Item tools: null

    /*!
      \preliminary
      The time in milliseconds before the toolbar automatically hides after inactivity
      when it is not locked.
     */
    property int hideTimeout: 5000

    /*! \internal */
    onToolsChanged: {
        internal.updateVisibleTools();
        if (tools) {
            if (tools && tools.hasOwnProperty("locked")) locked = tools.locked;
            // open the toolbar, except when it is locked in closed position
            if (tools && tools.hasOwnProperty("locked") && tools.hasOwnProperty("opened")
                    && !tools.opened && tools.locked) {
                // toolbar is locked in closed state
                toolbar.close();
            } else {
                toolbar.open();
            }

            if (tools && tools.hasOwnProperty("opened")) {
                tools.opened = toolbar.opened;
            }

            if (!toolbar.locked) {
                hideTimer.restart();
            }
        } else { // no tools
            locked = true;
            toolbar.close();
        }
    }

    // if tools is not specified, lock the toolbar in closed position
    locked: tools && tools.hasOwnProperty("locked") ? tools.locked : false

    Timer {
        id: hideTimer
        interval: toolbar.hideTimeout
        running: toolbar.opened && !toolbar.locked
        onTriggered: {
            if (!toolbar.locked) {
                toolbar.close();
            }
        }
    }

    onOpenedChanged: {
        if (tools && tools.hasOwnProperty("opened")) {
            tools.opened = toolbar.opened;
        }
        if (!toolbar.locked) hideTimer.restart();
    }

    Connections {
        target: tools
        ignoreUnknownSignals: true
        onOpenedChanged: {
            if (tools.opened) {
                toolbar.open();
            } else {
                toolbar.close();
            }
        }
        onLockedChanged: {
            toolbar.locked = tools.locked;
            // open the toolbar when it becomes unlocked
            // (may be because a new page was pushed to the page stack)
            if (!toolbar.locked) toolbar.open();
        }
    }

    QtObject {
        id: internal
        property Item visibleTools: tools
        function updateVisibleTools() {
            if (internal.visibleTools !== toolbar.tools) {
                if (internal.visibleTools) internal.visibleTools.parent = null;
                internal.visibleTools = toolbar.tools;
                if (internal.visibleTools) internal.visibleTools.parent = visibleToolsContainer;
            }
        }
    }

    onAnimatingChanged: {
        if (!animating && !opened) {
            internal.updateVisibleTools();
        }
    }

    StyledItem {
        // FIXME:
        // All theming items go into the background because only the children
        //  of the Panel are being shown/hidden while the toolbar
        //  itself may stay in place.
        id: background
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: units.gu(8)

        // The values of opened and animated properties are used in the style
        property bool opened: toolbar.opened
        property bool animating: toolbar.animating

        style: Theme.createStyleComponent("ToolbarStyle.qml", background)
    }

    Item {
        id: visibleToolsContainer
        anchors {
            fill: background
        }
    }
}
