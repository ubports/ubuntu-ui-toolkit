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

    The component provides ability to initiate data model refresh by pulling the
    attached Flickable's content. The Flickable's content must be at its top boundary.
    It can be attached to any Flickable or to any derivate component.

    When placed inside Flickable, set parent to the flickable explicitly so the component
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

    \section2 Styling
    The component style API is defined by the \l RefreshControlStyle component.
    Styles may define different ways to initiate refresh upon dragging.
*/

StyledItem {
    id: control

    /*!
      */
    property Flickable target: parent

    /*!
      The property holds the model to be refreshed. The value is taken automatically
      from the \l target, if the \l target has \a model property specified.
      */
    property var model: target && target.hasOwnProperty("model") ? target.model : null

    /*!
      */
    property bool completeWhen: true

    /*!
      */
    readonly property bool refreshing: __styleInstance.refreshing

    /*!
      */
    property string pullText: i18n.tr("Pull to refresh...")

    /*!
      */
    property string releaseText: i18n.tr("Release to refresh...")

    /*!
      */
    signal refresh()

    style: Theme.createStyleComponent("RefreshControlStyle.qml", control)
    implicitHeight: __styleInstance.implicitHeight
    anchors {
        left: target.left
        right: target.right
    }
    y: -(internals.contentY + height)

    Item {
        id: internals
        property bool completed: false
        property real contentY: target.contentY - target.originY
    }

    Component.onCompleted: {
        internals.completed = true;
    }
}
