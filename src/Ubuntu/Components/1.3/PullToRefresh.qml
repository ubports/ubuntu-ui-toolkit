/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3

/*!
    \qmltype PullToRefresh
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \since Ubuntu.Components 1.1
    \brief Pull-to-refresh component for Flickables to reload a model upon pull.

    The component provides ability to initiate data model refresh by pulling the
    attached Flickable's content. The refresh can be initiated when the flickable
    content is at its top boundary. By dragging the content further, reaching the
    threshold value defined by the style will initiate the manual refresh by emitting
    the \l refresh signal. The progress of the refresh must be notified to the
    component by defining the completion clause to the \l refreshing property.

    \qml
    import QtQuick 2.4
    import QtQuick.XmlListModel 2.0
    import Ubuntu.Components 1.3
    import Ubuntu.Components.ListItems 1.3

    MainView {
        width: units.gu(40)
        height: units.gu(71)

        XmlListModel {
            id: listModel
            source: "http://feeds.reuters.com/reuters/topNews"
            query: "/rss/channel/item"
            XmlRole { name: "title"; query: "title/string()" }
        }

        Page {
            title: "Reuters"
            ListView {
                id: view
                anchors.fill: parent
                model: listModel
                delegate: Standard {
                    width: ListView.view.width
                    height: units.gu(5)
                    text: title
                }
                PullToRefresh {
                    refreshing: view.model.status === XmlListModel.Loading
                    onRefresh: view.model.reload()
                }
            }
        }
    }
    \endqml

    \note UbuntuListView has a built-in PullToRefresh, therefore it is recommended
    to use UbuntuListView instead of ListView.

    The component will also show the progress of the model's update when the refresh
    gets initiated by the model or from other party. Style implementations can decide
    whether to visualize that or not.

    As default, the component displays a Label visualizing the two states of the
    component, which is pull to refresh and release to refresh. As mentioned, this
    is driven by the threshold value specified by the style, and the state is reported
    by the \l releaseToRefresh property. The \l content specifies the visuals
    to be shown by the component. Custom implementations can hold any component,
    which will be anchor filled to the component itself.
    \qml
    import QtQuick 2.4
    import QtQuick.XmlListModel 2.0
    import Ubuntu.Components 1.3
    import Ubuntu.Components.ListItems 1.3

    MainView {
        width: units.gu(40)
        height: units.gu(71)

        XmlListModel {
            id: listModel
            source: "http://feeds.reuters.com/reuters/topNews"
            query: "/rss/channel/item"
            XmlRole { name: "title"; query: "title/string()" }
        }

        Page {
            title: "Reuters"
            ListView {
                id: view
                anchors.fill: parent
                model: listModel
                delegate: Standard {
                    width: ListView.view.width
                    height: units.gu(5)
                    text: title
                }
                PullToRefresh {
                    id: pullToRefresh
                    refreshing: view.model.status === XmlListModel.Loading
                    onRefresh: view.model.reload()
                    content: Item {
                        Icon {
                            name: pullToRefresh.releaseToRefresh ? "search" : ""
                            height: parent.height
                            width: height
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }
        }
    }
    \endqml

    \note When declared as child of Flickable, set parent to the flickable explicitly
    so the component does not land in the \c content of Flickable.
    \qml
    import QtQuick 2.4
    import QtQuick.XmlListModel 2.0
    import Ubuntu.Components 1.3
    import Ubuntu.Components.ListItems 1.3

    MainView {
        id: main
        width: units.gu(40)
        height: units.gu(71)

        XmlListModel {
            id: rssFeed
            source: "http://feeds.reuters.com/reuters/topNews"
            query: "/rss/channel/item"
            XmlRole { name: "title"; query: "title/string()" }
        }
        Page {
            title: "Reuters"
            Flickable {
                id: flickable
                anchors.fill: parent
                contentHeight: column.childrenRect.height
                contentWidth: column.childrenRect.width
                Column {
                    id: column
                    Repeater {
                        model: rssFeed
                        Standard {
                            width: main.width
                            height: units.gu(5)
                            text: title
                        }
                    }
                }

                PullToRefresh {
                    parent: flickable
                    refreshing: rssFeed.status === XmlListModel.Loading
                    onRefresh: rssFeed.reload()
                }
            }
        }
    }
    \endqml

    \section2 Styling
    The component style API is defined by the \l PullToRefreshStyle component.
    Styles may define different ways to initiate refresh upon dragging.
*/

StyledItem {
    id: control

    /*!
      The property specifies when the component is ready to trigger the refresh()
      signal. The logic is defined by the style and the value is transferred
      from the style's releaseToRefresh property. The property can be used to
      define custom visuals for \l content.
      */
    readonly property bool releaseToRefresh: __styleInstance.releaseToRefresh

    /*!
      The property holds the offset the component is pulled from the \l target
      Flickable's \a topMargin. The property can be used to provide animations
      in custom contents.
      */
    readonly property real offset: -(target.topMargin - (y + height))

    /*!
      The property holds the visuals to be displayed when the component is revealed
      upon manual refresh. The default value is a Label showing "Pull to refresh..."
      text when the component is pulled down till the activation threshold, and
      "Release to refresh..." after that.
      */
    property Component content: __styleInstance.defaultContent

    /*!
      The Flickable or derivate the component is attached to. This can only be
      the parent or a sibling of the component. Defaults to the parent.
      */
    property Flickable target: parent

    /*!
      The property notifies the component about the ongoing refresh operation.
      */
    property bool refreshing: false

    /*!
      Signal emitted when the model refresh is initiated by the component.
      */
    signal refresh()

    styleName: "PullToRefreshStyle"
    implicitHeight: __styleInstance.implicitHeight
    anchors {
        left: target.left
        right: target.right
    }
    y: -(internals.contentY + height)

    QtObject {
        id: internals
        property bool completed: false
        property real contentY: target.contentY - target.originY
    }

    Component.onCompleted: {
        internals.completed = true;
    }
}
