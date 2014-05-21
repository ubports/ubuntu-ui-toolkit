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
    attached Flickable's content. The refresh can be initiated when the flickable
    content is at its top boundary. By dragging the content further, reaching the
    threshold value defined by the style will initiate the manual refresh by emitting
    the \l refresh signal. The progress of the refresh must be notified to the
    component by defining the completion clause to the \l refreshing property.

    \qml
    import QtQuick 2.2
    import QtQuick.XmlListModel 2.0
    import Ubuntu.Components 1.1
    import Ubuntu.Components.ListItems 1.0

    MainView {
        width: units.gu(40)
        height: units.gu(71)

        PageStack {
            Component.onCompleted: push(page)
        }

        Component {
            id: page
            Page {
                title: "Reuters"
                ListView {
                    anchors.fill: parent
                    model: XmlListModel {
                        source: "http://feeds.reuters.com/reuters/topNews"
                        query: "/rss/channel/item"
                        XmlRole { name: "title"; query: "title/string()" }
                    }
                    delegate: Standard {
                        width: ListView.view.width
                        height: units.gu(5)
                        text: title
                    }
                    PullToRefresh {
                        refreshing: model.status === XmlListModel.Loading
                        onRefresh: model.reload()
                    }
                }
            }
        }
    }
    \endqml

    \note UbuntuListView has a built-in RefreshControl, therefore it is recommended
    to use UbuntuListView instead of ListView.

    The component will also show the progress of the model's update when the refresh
    gets initiated by the model or from other party. Style implementations can decide
    whether to visualize that or not.

    \note When declared as child of Flickable, set parent to the flickable explicitly
    so the component does not land in the \c contentItem of Flickable.
    \qml
    Item {
        ListModel {
            id: someModel
            function refresh() {
                // [...]
            }
            property bool ready
        }
        Flickable {
            id: flicker
            width: units.gu(10)
            height: units.gu(10)

            RefreshControl {
                parent: flicker
                onRefresh: someModel.refresh()
                refreshing: !someModel.ready
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
    }
    \endqml

    \section2 Styling
    The component style API is defined by the \l RefreshControlStyle component.
    Styles may define different ways to initiate refresh upon dragging.
*/

StyledItem {
    id: control

    /*!
      \qmlproperty bool ready
      The property specifies when the component is fully functional.
      */
    readonly property alias ready: internals.completed

    /*!
      The property holds the text shown before refresh can be initiated. The limit
      is defined by the RefreshControlStyle \l RefreshControlStyle::activationThreshold property.
      */
    property string pullText: i18n.tr("Pull to refresh...")

    /*!
      The property holds the text shown indicating the refresh can be initiated
      upon touch/mouse release.
      \sa pullText, RefreshControlStyle::activationThreshold
      */
    property string releaseText: i18n.tr("Release to refresh...")

    /*!
      The Flickable or derivate the component is attached to. This can only be
      the parent or a sibling of the component. Defaults to the parent.
      */
    property Flickable target: parent

    /*!
      The property notifies the component about the refresh operation completion.
      Implementations must bind model's readyness checking in order to let the
      component to know when the operation is complete.
      */
    property bool refreshing: false

    /*!
      Signal emitted when the model refresh is initiated by the component.
      */
    signal refresh()

    style: Theme.createStyleComponent("PullToRefreshStyle.qml", control)
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
