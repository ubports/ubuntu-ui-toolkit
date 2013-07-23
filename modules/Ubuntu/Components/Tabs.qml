/*
 * Copyright 2012 Canonical Ltd.
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

/*!
    \qmltype Tabs
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The Tabs class provides an environment where multible \l Tab
    children can be added, and the user is presented with a tab
    bar with tab buttons to select different tab pages.

    Tabs must be placed inside a \l MainView so that it will automatically
    have a header that shows the tabs that can be selected, and the toolbar
    which contains the tools of the \l Page in the currently selected \l Tab.

    \l {http://design.ubuntu.com/apps/building-blocks/tabs}{See also the Design Guidelines on Tabs}.

    Example:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem

        MainView {
            width: units.gu(48)
            height: units.gu(60)

            Tabs {
                id: tabs
                Tab {
                    title: i18n.tr("Simple page")
                    page: Page {
                        Label {
                            id: label
                            anchors.centerIn: parent
                            text: "A centered label"
                        }
                        tools: ToolbarActions {
                            Action {
                                text: "action"
                                onTriggered: print("action triggered")
                            }
                        }
                    }
                }
                Tab {
                    id: externalTab
                    title: i18n.tr("External")
                    iconSource: "call_icon.png"
                    page: Loader {
                        parent: externalTab
                        anchors.fill: parent
                        source: (tabs.selectedTab === externalTab) ? Qt.resolvedUrl("MyCustomPage.qml") : ""
                    }
                }
                Tab {
                    title: i18n.tr("List view")
                    page: Page {
                        ListView {
                            clip: true
                            anchors.fill: parent
                            model: 20
                            delegate: ListItem.Standard {
                                icon: Qt.resolvedUrl("avatar_contacts_list.png")
                                text: "Item "+modelData
                            }
                        }
                    }
                }
            }
        }

    \endqml
    As the example above shows, an external \l Page inside a \l Tab can be loaded using a Loader.

    It is possible to use a Repeater to generate tabs, but when doing so, ensure that the Repeater
    is declared inside the Tabs at the end, because otherwise the shuffling of
    the order of children by the Repeater can cause incorrect ordering of the tabs.

    The \l {http://design.ubuntu.com/apps/global-patterns/navigation}{Navigation Patterns} specify that
    a tabs header should never be combined with the back button of a \l PageStack. The only way to
    combine Tabs and \l PageStack that avoids this is by pushing the Tabs as the first page on the
    \l PageStack, and pushing other pages on top of that, as is shown in the following example:

    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1

        MainView {
            id: mainView
            width: units.gu(38)
            height: units.gu(50)

            PageStack {
                id: pageStack
                Component.onCompleted: push(tabs)

                Tabs {
                    id: tabs
                    Tab {
                        title: "Tab 1"
                        page: Page {
                            Button {
                                anchors.centerIn: parent
                                onClicked: pageStack.push(page3)
                                text: "Press"
                            }
                        }
                    }
                    Tab {
                        title: "Tab 2"
                        page: Page {
                            Label {
                                anchors.centerIn: parent
                                text: "Use header to navigate between tabs"
                            }
                        }
                    }
                }
                Page {
                    id: page3
                    visible: false
                    title: "Page on stack"
                    Label {
                        anchors.centerIn: parent
                        text: "Press back to return to the tabs"
                    }
                }
            }
        }
    \endqml
*/

PageTreeNode {
    id: tabs
    anchors.fill: parent

    /*!
      \preliminary
      The index of the currently selected tab.
      The first tab is 0, and -1 means that no tab is selected.
      The initial value is 0 if Tabs has contents, or -1 otherwise.
     */
    property int selectedTabIndex: tabs.__tabs.length > 0 ? 0 : -1

    /*!
      \preliminary
      The currently selected tab.
     */
    readonly property Tab selectedTab: (selectedTabIndex < 0) || (__tabs.length <= selectedTabIndex) ?
                                           null : __tabs[selectedTabIndex]

    /*!
      The page of the currently selected tab.
     */
    readonly property Item currentPage: selectedTab ? selectedTab.page : null

    /*!
      \internal
      Header contents that will be used to override the default title inside the header,
      and provides scrollable tab buttons.
      FIXME: headerContents may be specified here directly, not taken from the style.
     */
    property Component __headerContents

    /*!
      \deprecated
      This property is deprecated. Pages will now automatically update the toolbar when activated.
     */
    property ToolbarActions tools: null
    /*!
      \deprecated
      \internal
     */
    onToolsChanged: print("Tabs.tools property was deprecated. "+
                          "Pages will automatically update the toolbar when activated. "+
                          "See CHANGES file, and use toolbar.tools instead when needed.");

    /*!
      \internal
      Used by the style to create the tabs header.
    */
    property alias __tabs: tabsModel.tabList

    /*!
      Children are placed in a separate item that has functionality to extract the Tab items.
      \qmlproperty list<Item> tabChildren
     */
    default property alias tabChildren: tabsModel.children

    /*!
      Used by the tabs style to update the tabs header with the titles of all the tabs.
      This signal is used in an intermediate step in transitioning the tabs to a new
      implementation and may be removed in the future.
     */
    signal modelChanged()

    /*!
      \internal
      required by TabsStyle
     */
    Item {
        anchors.fill: parent
        id: tabsModel

        property var tabList: []
        onChildrenChanged: {
            updateTabList();
        }

        function updateTabList() {
            var list = [];
            for (var i=0; i < children.length; i++) {
                if (isTab(tabsModel.children[i])) list.push(tabsModel.children[i]);
            }
            tabList = list;
            tabs.modelChanged();
        }

        function isTab(item) {
            if (item && item.hasOwnProperty("__isPageTreeNode")
                    && item.__isPageTreeNode && item.hasOwnProperty("title")
                    && item.hasOwnProperty("page")) {
                return true;
            } else {
                return false;
            }
        }
    }

    /*! \internal */
    onModelChanged: if (tabs.active && internal.header) internal.header.show()

    QtObject {
        id: internal
        property Header header: tabs.__propagated ? tabs.__propagated.header : null
    }

    Binding {
        target: internal.header
        property: "contents"
        value: tabs.active ? tabs.__headerContents : null
        when: internal.header && tabs.active
    }

    style: Theme.createStyleComponent("TabsStyle.qml", tabs)
}
