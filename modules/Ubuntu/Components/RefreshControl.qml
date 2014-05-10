/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2

/*!
    \qmltype RefreshControl
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Pull-to-refresh component for Flickables to reload a model upon pull.

    When used with Flickable, set parent to the flickable explicitly so the component
    does not land in the contentItem of Flickable.
    \qml
    ListModel {
        id: listModel
        function refresh() {
            // [...]
        }
    }
    Flickable {
        id: flicker
        width: units.gu(10)
        height: units.gu(10)

        RefreshControl {
            parent: flicker
            model: someModel
            refreshMethod: "refresh"
        }

        contentWidth: rect.width
        contentHeight: rect.height
        Rectangle {
            id: rect
            width: units.gu(20)
            height: units.gu(20)
            color: "red"
        }
    }
    \endqml
*/

StyledItem {
    id: control

    /*!
      */
    property Flickable target: parent

    /*!
      */
    property bool completeWhen: true

    /*!
      */
    readonly property alias refreshing: internals.refreshing

    /*!
      */
    property string pullText: i18n.tr("Pull to refresh...")

    /*!
      */
    property string releaseText: i18n.tr("Release to refresh...")

    /*!
      */
    property var model

    /*!
      */
    signal refresh()

    function beginRefreshing() {
        internals.refreshing = true;
    }

    style: Theme.createStyleComponent("RefreshControlStyle.qml", control)
    height: units.gu(5)
    anchors {
        left: target.left
        right: target.right
    }
    y: -(internals.contentY + __styleInstance.layoutHeight)

    Item {
        id: internals
        property bool completed: false
        property bool refreshing: false
        property bool triggerRefresh: false
        property real contentY: target.contentY - target.originY
        property real threshold: control.__styleInstance.flipThreshold
        property real baseTopMargin: 0.0

        states: [
            State {
                name: ""
                when: !internals.refreshing && !(internals.contentY < -internals.threshold)
                PropertyChanges {
                    target: internals
                    refreshing: false
                }
            },
            State {
                name: "release-to-refresh"
                when: internals.completed && (internals.contentY < -internals.threshold) && !internals.refreshing
                PropertyChanges {
                    target: internals
                    triggerRefresh: true
                }
            },
            State {
                name: "refresh-in-progress"
                when: internals.completed && internals.refreshing
            }
        ]

        Binding {
            target: control.__styleInstance
            property: "state"
            value: internals.state
        }
        Binding {
            target: control.__styleInstance
            property: "flickableTopMargin"
            value: internals.baseTopMargin
        }
    }

    Component.onCompleted: {
        internals.baseTopMargin = control.target.topMargin;
        internals.completed = true;
    }
    /*! \internal */
    onCompleteWhenChanged: {
        if (completeWhen) {
            internals.refreshing = false;
            internals.state = "";
        }
    }

    // catch when to update
    Connections {
        target: control.target
        onDraggingChanged: {
            if (!control.parent.dragging && internals.triggerRefresh) {
                internals.refreshing = true;
                control.refresh();
            }
        }
        onTopMarginChanged: {
            if (!internals.completed) {
                internals.baseTopMargin = control.target.topMargin;
            }

//            print("topMargin="+control.target.topMargin)
        }
    }
}
