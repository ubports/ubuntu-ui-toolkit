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
    hintSize: Theming.ComponentUtils.style(toolbar, "hintSize", units.gu(2))
    triggerSize: Theming.ComponentUtils.style(toolbar, "triggerSize", units.gu(2))

    /*!
      \preliminary
      The list of \l Actions to be shown on the toolbar
     */
    property ToolbarActions tools
    onToolsChanged: {
        if (tools && tools.active && tools.lock) {
            // toolbar is locked in visible state.
            internal.visibleTools = tools;
            active = true;
        } else if (!active && !animating) {
            // toolbar is invisible
            internal.visibleTools = tools;
        } else {
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

    // FIXME: It would be better to use a single Loader for the backButton and
    //  back.itemHint, but that gives problems in setting the height of the component
    Loader {
        id: something
        property Action back: toolbar.tools && toolbar.tools.back ? toolbar.tools.back : null
//        sourceComponent: back && back.itemHint ? back.itemHint : null
        property Action action: back
        sourceComponent: back && back.itemHint ? back.itemHint : toolButtonComponent
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
            verticalCenter: parent.verticalCenter
        }
        height: back && back.itemHint ? undefined : parent.height
    }
//    Button {
//        id: backButton
//        property Action back: toolbar.tools && toolbar.tools.back ? toolbar.tools.back : null
//        visible: back && back.visible && !back.itemHint
//        Theming.ItemStyle.class: "toolbar-button"
//        anchors {
//            left: parent.left
//            leftMargin: units.gu(2)
//            verticalCenter: parent.verticalCenter
//        }
//        iconSource: back ? back.iconSource : ""
//        text: back ? back.text : ""
//        onClicked: back.triggered(backButton)
//        height: parent.height
//    }

    Component {
        id: toolButtonComponent

        Button {
            id: toolButton
            Theming.ItemStyle.class: "toolbar-button"
            text: action.text
            iconSource: action.iconSource ? action.iconSource : ""
            onClicked: action.triggered(toolButton)
            enabled: action.enabled
            visible: action.visible
            width: visible ? implicitWidth : 0
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
            Loader {
                sourceComponent: modelData.itemHint ? modelData.itemHint : toolButtonComponent
                property Action action: modelData
                anchors.verticalCenter: toolButtonsContainer.verticalCenter
                height: modelData.itemHint ? implicitHeight : toolButtonsContainer.height
            }
        }
    }
}
