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
import Ubuntu.Components 1.3 as Toolkit13
import "pageUtils.js" as Utils

/*!
    \qmltype Page
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief A page is the basic Item that must be used inside the \l MainView,
        \l PageStack and \l Tabs.
        Anchors and height of a Page are automatically determined to align with
        the header of the \l MainView, but can be overridden.


    \l MainView provides a header for Pages it includes if no
    \l header property was set.
    The text and actions of the header are determined by the \l title
    and \l head properties of the page:

    \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.2

        MainView {
            width: units.gu(48)
            height: units.gu(60)

            Page {
                title: i18n.tr("Example page")

                Label {
                    anchors.centerIn: parent
                    text: i18n.tr("Hello world!")
                }

                head.actions: [
                    Action {
                        iconName: "search"
                        text: i18n.tr("Search")
                    },
                    Action {
                        iconName: "contacts"
                        text: i18n.tr("Contacts")
                    }
                ]
            }
        }
    \endqml

    The Page automatically anchors to the left and bottom of its parent. The width of the Page
    will be the full width of its parent \l MainView or \l PageStack or \l Tab,
    and the height will adapt to leave space for the header when needed. It is possible to
    use a Page inside a Loader, but in that case do not set the anchors or size of the Loader
    so that the Page can control its width and height.
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
    height: parentNode ? page.flickable ? parentNode.height : parentNode.height - internal.headerHeight : undefined

    /*!
      The header property for this page. Setting this property will reparent the
      header to the page and disable the \l MainView's application header.
      \qml
        Page {
            id: page
            title: "Page with header"
            header: PageHeader {
                title: page.title
                trailingActionBar.actions: [
                    Action { iconName: "settings" },
                    Action { iconName: "info" }
                ]
            }
        }
      \endqml
      \sa PageHeader
     */
    property Item header
    onHeaderChanged: internal.updateHeader()
    Component.onCompleted: internal.updateHeader()

    /*! \internal */
    isLeaf: true

    /*!
      The title of the page. Will be shown in the header of the \l MainView.
      If the page is used inside a \l Tab, the default title is the \l Tab title.
      For a Page not inside a \l Tab, the default title is an empty string.
     */
    property string title: parentNode && parentNode.hasOwnProperty("title") ? parentNode.title : ""

    /*!
      Optional flickable that controls the MainView header. This property
      is automatically set to the first child of the page that is Flickable
      and anchors to the top of the page or fills the page. For example:
      \qml
          import QtQuick 2.4
          import Ubuntu.Components 1.2

          MainView {
              width: units.gu(30)
              height: units.gu(50)
              Page {
                  id: page
                  title: "example"
                  //flickable: null // uncomment for a fixed header
                  Flickable {
                      id: content
                      anchors.fill: parent
                      contentHeight: units.gu(70)
                      Label {
                          text: "hello"
                          anchors.centerIn: parent
                      }
                  }
              }
          }
      \endqml
      In this example, page.flickable will automatically be set to content because it is
      a Flickable and it fills its parent. Thus, scrolling down in the Flickable will automatically
      hide the header.

      Set this property to null to avoid automatic flickable detection, which disables hiding
      of the header by scrolling in the Flickable. In cases where a flickable should control the header,
      but it is not automatically detected, the flickable property can be set.
     */
    property Flickable flickable: Utils.getFlickableChild(page)

    /*!
      \qmlproperty PageHeadConfiguration Page::head
      \readonly
      \deprecated
      Configuration of the header for this page.
      Deprecated: This configuration will be replaced by setting the \l header property.
     */
    readonly property alias head: headerConfig
    Toolkit13.PageHeadConfiguration {
        id: headerConfig
        title: page.title
        flickable: page.flickable
    }

    Toolkit13.Object {
        id: internal

        property Item previousHeader: null
        property Item previousHeaderParent: null
        function updateHeader() {
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
