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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

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
      The list of \l Actions to be shown on the toolbar
     */
//    property ToolbarActions tools: null
    property Item tools: null

    /*! \internal */
    onToolsChanged: {
        if (tools.hasOwnProperty("locked")) locked = tools.locked;
        if (tools && tools.hasOwnProperty("locked") && tools.hasOwnProperty("opened")
                && tools.opened && tools.locked) {
            // toolbar is locked in visible state.
            if (internal.visibleTools) internal.visibleTools.parent = null;
            internal.visibleTools = tools;
            opened = true;
        } else if (!opened && !animating) {
            // toolbar is invisible
            if (internal.visibleTools) internal.visibleTools.parent = null;
            internal.visibleTools = tools;
        } else {
            opened = false;
            // internal.visibleTools will be updated
            // when the hide animation is finished
        }
    }

    // if tools is not specified, lock the toolbar in closed position
    locked: tools && tools.hasOwnProperty("locked") ? tools.locked : true

    Connections {
        target: tools
        onOpenedChanged: toolbar.opened = tools.opened;
        onLockedChanged: toolbar.locked = tools.locked;
    }

    QtObject {
        id: internal
//        property ToolbarActions visibleTools: tools
        property Item visibleTools: tools
        onVisibleToolsChanged: {
            if (internal.visibleTools) {
                internal.visibleTools.parent = visibleToolsContainer;
            }
        }
    }

    onAnimatingChanged: {
        if (!animating && !opened) {
            if (internal.visibleTools) internal.visibleTools.parent = null;
            internal.visibleTools = toolbar.tools;
        }
    }

    Item {
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

        Theming.ItemStyle.class: "toolbar"
        // The values of opened and animated properties are used in the delegate
        property bool opened: toolbar.opened
        property bool animating: toolbar.animating
    }

    Item {
        id: visibleToolsContainer
        anchors {
            fill: background
        }
    }


//    Component {
//        id: toolButtonComponent
//        Button {
//            id: toolButton
//            // Disable the mouse area so swipes on the button will not be blocked
//            // from going to the toolbar. The panel will take care calling the button's clicked().
//            __mouseArea.visible: false
//            Theming.ItemStyle.class: "toolbar-button"
//            width: units.gu(5)
//            height: toolbar.height
//        }
//    }

//    Loader {
//        id: backButton
//        property Action backAction: toolbar.tools ? toolbar.tools.back : null
//        sourceComponent: backAction ? backAction.itemHint ? backAction.itemHint : toolButtonComponent : null
//        anchors {
//            left: parent.left
//            leftMargin: units.gu(2)
//            verticalCenter: parent.verticalCenter
//        }
//        onStatusChanged: {
//            if (item && status == Loader.Ready) {
//                if (item.hasOwnProperty("action")) item.action = backAction;
//            }
//        }
//        // ensure the item's action is up-to-date (which is not the case without this line):
//        onBackActionChanged: if (item && item.hasOwnProperty("action")) item.action = backAction;
//    }

//    Row {
//        id: toolButtonsContainer
//        anchors {
//            right: parent.right
//            bottom: parent.bottom
//            top: parent.top
//            rightMargin: units.gu(2)
//        }
//        spacing: units.gu(1)

//        Repeater {
//            model: internal.visibleTools ? internal.visibleTools.children : 0
//            Loader {
//                sourceComponent: modelData.itemHint ? modelData.itemHint : toolButtonComponent
//                anchors.verticalCenter: toolButtonsContainer.verticalCenter
//                onStatusChanged: {
//                    if (item && status == Loader.Ready) {
//                        if (item.hasOwnProperty("action")) item.action = modelData
//                    }
//                }
//            }
//        }
//    }
}
