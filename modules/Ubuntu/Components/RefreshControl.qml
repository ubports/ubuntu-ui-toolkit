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
import Ubuntu.Components 1.1

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

    The component detects whether the \l target has a \a model property and whether
    that \a model has \a refresh() or \a reload() functions and whether it has \a
    ready property. When either \a refresh() or \a reload() functions are defined,
    the component will call those functions and will not emit \l refresh() signal.
    When \a ready property is defined, the \l refreshing property will be bount to
    that property.
    \qml
    import QtQuick 2.2
    import QtQuick.XmlListModel 2.0
    import Ubuntu.Components 1.1
    import Ubuntu.Components.ListItems 1.0

    ListView {
        model: XmlListModel {
            id: xmlModel
            property bool ready: status === XmlListModel.Ready
        }
        delegate: Standard {
            width: ListView.view.width
            height: units.gu(5)
            text:
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
      \qmplroperty bool ready
      The property specifies when the component is fully functional.
      */
    readonly property alias ready: internals.completed

    /*!
      The property holds the text shown before refresh can be initiated. The limit
      is defined by the RefreshControlStyle \l activationThreshold property.
      */
    property string pullText: i18n.tr("Pull to refresh...")

    /*!
      The property holds the text shown indicating the refresh can be initiated
      upon touch/mouse release.
      \sa pullText activationThreshold
      */
    property string releaseText: i18n.tr("Release to refresh...")

    /*!
      The Flickable or derivate the component is attached to. This can be the
      parent or sibling of the component.
      */
    property Flickable target: parent

    /*!
      The property notifies the component about the refresh operation completion.
      Implementations must bind model's readyness checking in order to let the
      component to know when the operation is complete. If the \l target has a
      \a model which has a \a ready property defined, the component will auto-bind
      that property, so implementations do not need to care of it themselves.
      */
    property bool refreshing: false//internals.autoComplete ? internals.model.ready : false

    /*!
      Signal emitted when the model refresh is initiated by the component. If the
      \l target has a \a model which has \a reload() function defined, the function
      will be called and the signal will not be emitted.
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
        property QtObject model: control.target && control.target.hasOwnProperty("model") ? control.target.model : null
        property QtObject prevModel: null
        onModelChanged: updateModel()

        property bool autoReload: false
        property bool autoComplete: false

        function updateModel() {
            if (prevModel && autoComplete) {
                prevModel.completed.disconnect(reloadComplete);
            }
            prevModel = null;
            autoReload = autoComplete = false;

            if (!completed || !model || !isObjectModel()) {
                return;
            }

            autoReload = model.hasOwnProperty("reload");
            autoComplete = model.hasOwnProperty("ready");
            if (autoComplete) {
                control.refreshing = Qt.binding(function() { return !model.ready; });
            }
        }
        function isObjectModel() {
            return (Object.prototype.toString.call(model) === "[object Object]");
        }

        // initiate refresh on model
        function initiateRefresh() {
            if (autoReload) {
                model.reload();
            } else {
                control.refresh();
            }
            print("refresh")
        }
    }

    Connections {
        target: __styleInstance
        onRefresh: {
            internals.initiateRefresh();
        }
    }

    Component.onCompleted: {
        internals.completed = true;
        internals.updateModel();
    }
}
