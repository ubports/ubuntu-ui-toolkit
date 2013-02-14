/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
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
GenericToolbar {
    id: toolbar
    Theming.ItemStyle.class: "toolbar"

    height: background.height
    hintSize: Theming.ComponentUtils.style(toolbar, "hintSize", units.gu(1))

    /*!
      \preliminary
      The list of \l Actions to be shown on the toolbar
     */
    property ToolbarActions tools
    onToolsChanged: {
        if (tools && tools.active && tools.lock) {
            internal.visibleTools = tools;
            active = true;
        }
        else {
            active = false;
            // internal.visibleTools will be updated
            // when the hide animation is finished
        }
    }

    // if tools is not specified, lock the toolbar in inactive position
    lock: tools ? tools.lock : true

    Connections {
        target: tools
        onActiveChanged: toolbar.active = tools.active;
    }
    onActiveChanged: if (tools) tools.active = toolbar.active
    QtObject {
        id: internal
        property ToolbarActions visibleTools: tools
    }

    onAnimatingChanged: {
        if (!animating && !active) {
            internal.visibleTools = toolbar.tools;
        }
    }

    Item {
        // All visual items go into the background because only the children
        //  of the GenericToolbar are being shown/hidden while the toolbar
        //  itself may stay in place.
        id: background
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: units.gu(8)

        Theming.ItemStyle.style: toolbar.Theming.ItemStyle.style
        Theming.ItemStyle.delegate: toolbar.Theming.ItemStyle.delegate

        MouseArea {
            // don't let mouse events go through the toolbar
            anchors.fill: parent
            // FIXME: Bug in qml? Without onClicked below, this MouseArea
            //      seems disabled.
            onClicked: { }
        }
    }

    Button {
        id: backButton
        property Action back: toolbar.tools && toolbar.tools.back ? toolbar.tools.back : null
        visible: back && back.visible
        Theming.ItemStyle.class: "toolbar-button"
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
            verticalCenter: parent.verticalCenter
        }
        iconSource: back ? back.iconSource : ""
        text: back ? back.text : ""
        onClicked: back.triggered(backButton)
        height: parent.height
    }

    // FIXME: The detection of a "leftItem" was introduced in order to be able to have
    //  a back button with different styling. Currently we cannot re-use backButton for that
    //  because setting Theming.ItemStyle.class dynamically gives errors.
    Item {
        id: leftItemContainer
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: units.gu(2)
        }
        width: childrenRect.width

        property Item item: toolbar.tools && toolbar.tools.hasOwnProperty("leftItem") ? toolbar.tools.leftItem : null
        onItemChanged: {
            for (var i=leftItemContainer.children.length-1; i >= 0; i--) leftItemContainer.children[i].parent = null;
            if (item) item.parent = leftItemContainer;
        }
    }

    Row {
        id: toolButtonsContainer
        anchors {
            right: parent.right
            bottom: parent.bottom
            top: parent.top
            rightMargin: units.gu(2)
        }
        width: childrenRect.width
        spacing: units.gu(1)

        Repeater {
            model: internal.visibleTools ? internal.visibleTools.children : 0
            Button {
                id: toolButton
                Theming.ItemStyle.class: "toolbar-button"
                anchors.verticalCenter: parent.verticalCenter
                text: modelData.text
                iconSource: modelData.iconSource ? modelData.iconSource : ""
                onClicked: modelData.triggered(toolButton)
                enabled: modelData.enabled
                visible: modelData.visible
                height: parent.height
            }
        }
    }
}
