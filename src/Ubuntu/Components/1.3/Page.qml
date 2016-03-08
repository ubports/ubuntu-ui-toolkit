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
import "pageUtils.js" as Utils

/*!
    \qmltype Page
    \inqmlmodule Ubuntu.Components 1.3
    \inherits StyledItem
    \ingroup ubuntu
    \brief A page is the basic Item that represents a single view in
        an Ubuntu application. It is recommended to use the Page inside
        the \l MainView or \l AdaptivePageLayout.

        \l MainView provides a header for Pages it includes if no
        \l header property was set. However, the application header is deprecated
        and it is recommended to set the \l header property instead.

        Anchors and height of a Page are automatically determined to align with
        the header of the \l MainView, but can be overridden.
        Page contents does not automatically leave space for the Page \l header,
        so this must be taken into account when anchoring the contents of the Page.

        Example:
        \qml
            import QtQuick 2.4
            import Ubuntu.Components 1.3

            MainView {
                width: units.gu(48)
                height: units.gu(60)

                Page {
                    header: PageHeader {
                        id: pageHeader
                        title: i18n.tr("Example page")

                        trailingActionBar.actions: [
                            Action {
                                iconName: "search"
                                text: i18n.tr("Search")
                            }
                        ]
                    }

                    Label {
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            top: pageHeader.bottom
                            topMargin: units.gu(5)
                        }
                        text: i18n.tr("Hello world!")
                    }
                }
            }
        \endqml
*/
PageTreeNode {
    id: page
    anchors {
        left: parent ? parent.left : undefined
        bottom: parent ? parent.bottom : undefined
    }
    // Set width and height so that a parent Loader can be automatically resized
    // to the size of the loaded Page.
    width: parentNode ? parentNode.width - page.x : undefined
    // FIXME: We no longer need to take the internal header height into account
    //  when we remove MainView's AppHeader.
    height: parentNode ? page.flickable ? parentNode.height : parentNode.height - internal.headerHeight : undefined

    /*!
      \qmlproperty ActrionContext Page::actionContext
      \readonly
      \since Ubuntu.Components 1.3
      The action context of the page.
      */
    readonly property alias actionContext: localContext
    ActionContext {
        id: localContext
        active: page.active
        objectName: page.objectName + "Context"
    }

    /*!
      \since Ubuntu.Components 1.3
      The header property for this page. Setting this property will reparent the
      header to the page and disable the \l MainView's application header.
      \qml
        Page {
            id: page
            header: PageHeader {
                title: "Page with header"
                trailingActionBar.actions: [
                    Action { iconName: "settings" },
                    Action { iconName: "info" }
                ]
                flickable: myFlickable
            }
        }
      \endqml
      To avoid Page content being occluded by the header, the contents of the Page
      should anchor to the bottom of the header. When the Page contents is flickable,
      the contents does not need to be anchored to the header, but it is recommended
      to use a \l PageHeader or \l Header component as the Page header, and set its
      \l Header::flickable property so that the Flickable gets a top-margin that
      leaves enough space for the header.
      \sa PageHeader, Header
     */
    property Item header
    onHeaderChanged: internal.updateHeader()
    Component.onCompleted: internal.updateHeader()

    /*! \internal */
    isLeaf: true

    /*! \deprecated */
    property string title: parentNode && parentNode.hasOwnProperty("title") ? parentNode.title : ""
    /*! \deprecated */
    property Flickable flickable: Utils.getFlickableChild(page)
    /*! \deprecated */
    readonly property alias head: headerConfig
    PageHeadConfiguration {
        id: headerConfig
        title: page.title
        flickable: page.flickable
        onFlickableChanged: internal.printDeprecationWarning()
        onTitleChanged: internal.printDeprecationWarning()
        onActionsChanged: internal.printDeprecationWarning()
        onBackActionChanged: internal.printDeprecationWarning()
    }

    Object {
        id: internal

        property bool showDeprecationWarning: true
        function printDeprecationWarning() {
            if (internal.showDeprecationWarning) {
                var titleStr = page;
                if (page.title) {
                    titleStr += "\"" + page.title + "\"";
                }
                titleStr += ": "
                print(titleStr + "In Ubuntu.Components 1.3, the use of Page.title, Page.flickable and" +
                      " Page.head is deprecated. Use Page.header and the PageHeader component instead.");
                internal.showDeprecationWarning = false;
            }
        }

        property Item previousHeader: null
        property Item previousHeaderParent: null
        function updateHeader() {
            internal.showDeprecationWarning = false;
            if (internal.previousHeader) {
                internal.previousHeader.parent = internal.previousHeaderParent;
            }
            if (page.header) {
                internal.previousHeaderParent = page.header.parent;
                internal.previousHeader = page.header;
                page.header.parent = page;
            } else {
                internal.previousHeader = null;
                internal.previousHeaderParent = null;
            }
        }

        ///////////////////////////////
        // old header handling below //
        ///////////////////////////////
        property AppHeader header: page.__propagated && page.__propagated.header ? page.__propagated.header : null
        // Used to position the Page when there is no flickable.
        // When there is a flickable, the header will automatically position it.
        property real headerHeight: internal.header && internal.header.visible ?
                                        internal.header.height + internal.header.y : 0

        // Note: The bindings below need to check whether headerConfig.contents
        // is valid in the "value", even when that is required in the Binding's "when"
        // property, to avoid TypeErrors while/after a page becomes (in)active.
        //
        // Note 2: contents.parent binding is made by PageHeadStyle.
        property bool hasParent: headerConfig.contents &&
                                 headerConfig.contents.parent

        Binding {
            target: headerConfig.contents
            property: "visible"
            value: page.active
            when: headerConfig.contents
        }
        Binding {
            target: headerConfig.contents
            property: "anchors.verticalCenter"
            value: internal.hasParent ? headerConfig.contents.parent.verticalCenter :
                                        undefined
            when: headerConfig.contents
        }
        Binding {
            target: headerConfig.contents
            property: "anchors.left"
            value: internal.hasParent ? headerConfig.contents.parent.left : undefined
            when: headerConfig.contents
        }
    }
}
