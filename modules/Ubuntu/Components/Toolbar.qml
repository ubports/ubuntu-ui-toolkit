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

    /*!
      \deprecated
      Use property bool opened instead.
     */
    property bool active
    onActiveChanged: {
        print("Toolbar.active property is DEPRECATED. Use opened instead.");
        toolbar.opened = active;
    }

    /*
      \deprecated
      Use property locked instead.
     */
    property bool lock
    /*! \internal */
    onLockChanged: {
        print("Toolbar.lock property is DEPRECATED. Use locked instead.");
        toolbar.locked = lock;
    }

    /*!
      \preliminary
      The tools to be shown on the toolbar
     */
    property Item tools: null

    /*! \internal */
    onToolsChanged: {
        if (tools && tools.hasOwnProperty("locked")) locked = tools.locked;
        if (tools && tools.hasOwnProperty("locked") && tools.hasOwnProperty("opened")
                && tools.opened && tools.locked) {
            // toolbar is locked in visible state.
            internal.updateVisibleTools();
            opened = true;
        } else if (!opened && !animating) {
            // toolbar is closed
            internal.updateVisibleTools();
        } else {
            opened = false;
            // internal.visibleTools will be updated
            // when the hide animation is finished
        }
        if (tools && tools.hasOwnProperty("opened")) {
            tools.opened = toolbar.opened;
        }
    }

    // if tools is not specified, lock the toolbar in closed position
    locked: tools && tools.hasOwnProperty("locked") ? tools.locked : false

    onOpenedChanged: {
        if (tools && tools.hasOwnProperty("opened")) {
            tools.opened = toolbar.opened;
        }
    }

    Connections {
        target: tools
        ignoreUnknownSignals: true
        onOpenedChanged: toolbar.opened = tools.opened;
        onLockedChanged: toolbar.locked = tools.locked;
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
