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
    property ToolbarActions tools: null
    /*! \internal */
    onToolsChanged: {
        if (tools && tools.opened && tools.locked) {
            // toolbar is locked in visible state.
            internal.visibleTools = tools;
            opened = true;
        } else if (!opened && !animating) {
            // toolbar is invisible
            internal.visibleTools = tools;
        } else {
            opened = false;
            // internal.visibleTools will be updated
            // when the hide animation is finished
        }
    }

    // if tools is not specified, lock the toolbar in closed position
    locked: tools ? tools.locked : true

    Connections {
        target: tools
        onOpenedChanged: toolbar.opened = tools.opened;
        onLockedChanged: toolbar.locked = tools.locked;
    }
    onOpenedChanged: if (tools) tools.opened = toolbar.opened
    onLockedChanged: if (tools) tools.locked = toolbar.locked
    QtObject {
        id: internal
        property ToolbarActions visibleTools: tools
    }

    onAnimatingChanged: {
        if (!animating && !opened) {
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

    Component {
        id: toolButtonComponent
        Item {
            id: toolButton
            Theming.ItemStyle.class: "toolbar-button"
            property string text: action && action.text ? action.text : ""
            property url iconSource: action && action.iconSource ? action.iconSource : ""
            signal clicked()
            onClicked: action.triggered(toolButton)
            enabled: action && action.enabled
            visible: action && action.visible
            width: units.gu(5)
            height: toolbar.height
        }
    }

    Loader {
        id: backButton
        property Action action: toolbar.tools && toolbar.tools.back ? toolbar.tools.back : null
        sourceComponent: action ? action.itemHint ? action.itemHint : toolButtonComponent : null
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
            verticalCenter: parent.verticalCenter
        }
        onStatusChanged: {
            if (item && status == Loader.Ready && action && action.itemHint) {
                if (item.hasOwnProperty("clicked")) item.clicked.connect(backButton.itemTriggered);
                if (item.hasOwnProperty("accepted")) item.accepted.connect(backButton.itemTriggered);
                if (item.hasOwnProperty("triggered")) item.accepted.connect(backButton.itemTtriggered);
            }
        }
        signal itemTriggered()
        onItemTriggered: action.triggered(item)
    }

    Row {
        id: toolButtonsContainer
        anchors {
            right: parent.right
            bottom: parent.bottom
            top: parent.top
            rightMargin: units.gu(2)
        }
        spacing: units.gu(1)

        Repeater {
            model: internal.visibleTools ? internal.visibleTools.children : 0
            Loader {
                sourceComponent: modelData.itemHint ? modelData.itemHint : toolButtonComponent
                property Action action: modelData
                anchors.verticalCenter: toolButtonsContainer.verticalCenter
            }
        }
    }
}
