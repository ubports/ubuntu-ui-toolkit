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
                        tools: ToolbarItems {
                            ToolbarButton {
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
      \qmlproperty int selectedTabIndex
      The index of the currently selected tab.
      The first tab is 0, and -1 means that no tab is selected.
      The initial value is 0 if Tabs has contents, or -1 otherwise.
     */
    property alias selectedTabIndex: bar.selectedIndex

    /*!
      \preliminary
      The currently selected tab.
     */
    readonly property Tab selectedTab: (selectedTabIndex < 0) || (tabsModel.count <= selectedTabIndex) ?
                                           null : tabsModel.get(selectedTabIndex).tab

    /*!
      The page of the currently selected tab.
     */
    readonly property Item currentPage: selectedTab ? selectedTab.page : null

    /*!
      The \l TabBar that will be shown in the header
      and provides scrollable tab buttons.
     */
    property TabBar tabBar: TabBar {
        id: bar
        model: tabsModel
        visible: tabs.active
    }

    /*!
      Children are placed in a separate item that has functionality to extract the Tab items.
      \qmlproperty list<Item> tabChildren
     */
    default property alias tabChildren: tabStack.data

    /*!
      \qmlproperty int count
      Contains the number of tabs in the Tabs component.
      */
    readonly property alias count: tabsModel.count

    /*!
      \deprecated
      Used by the tabs style to update the tabs header with the titles of all the tabs.
      This signal is used in an intermediate step in transitioning the tabs to a new
      implementation and may be removed in the future.
     */
    signal modelChanged()

    /*!
      Appends a Tab dynamically to the list of tabs. The \a title specifies the
      title of the Tab. The \a component can be either a Component, a URL to
      the Tab component to be loaded or an instance of a pre-declared tab that
      has been previously removed. The Tab's title will be replaced with the given
      \a title, unless if the given value is empty string or undefined. The optional
      \a params defines parameters passed to the Tab.
      Returns the instance of the added Tab.
      */
    function addTab(title, component, params) {
        return insertTab(count, title, component, params);
    }

    /*!
      Inserts a Tab at the given index. If the \a index is less or equal than 0,
      the Tab will be added to the front, and to the end of the tab stack if the
      \a index is greater than \l count. \a title, \a component and \a params
      are used in the same way as in \l addTab(). Returns the instance of the
      inserted Tab.
      */
    function insertTab(index, title, component, params) {
        // check if the given component is a Tab instance
        var tab = null;
        if (component && component.hasOwnProperty("page") && component.hasOwnProperty("__protected")) {
            // dynamically added components are destroyed upon removal, so
            // in case we get a Tab as parameter, we can only have a predeclared one
            // therefore we simply restore the default state of the removedFromTabs property
            // and return the instance
            if (!component.__protected.removedFromTabs) {
                // exit if the Tab is not removed
                return null;
            }

            component.__protected.removedFromTabs = false;
            tab = component;
        } else {
            var tabComponent = null;
            if (typeof component === "string") {
                tabComponent = Qt.createComponent(component);
            } else {
                tabComponent = component;
            }
            if (tabComponent.status === Component.Error) {
                console.error(tabComponent.errorString());
                return null;
            }
            tab = tabComponent.createObject();
            tab.__protected.dynamic = true;
        }

        // fix title
        if (title !== undefined && title !== "") {
            tab.title = title;
        }

        // insert the created tab into the model
        index = MathUtils.clamp(index, 0, count);
        tab.__protected.inserted = true;
        tab.__protected.index = index;
        tabsModel.insert(index, tabsModel.listModel(tab));
        tabsModel.reindex(index);
        tab.parent = tabStack;
        if (tabs.selectedTabIndex >= index) {
            // move the selected index to the next index
            tabs.selectedTabIndex += 1;
        } else {
            internal.sync();
        }
        return tab;
    }

    /*!
      Moves the tab from the given \a from position to the position given in \a to.
      Returns true if the indexes were in 0..\l count - 1 boundary and if the operation
      succeeds, and false otherwise. The \l selectedTabIndex is updated if it is
      affected by the move (it is equal with \a from or falls between \a from and
      \a to indexes).
      */
    function moveTab(from, to) {
        if (from < 0 || from >= count || to < 0 || to >= count || from === to) return false;
        var tabFrom = tabsModel.get(from).tab;
        var tabTo = tabsModel.get(to).tab;

        // move tab
        tabsModel.move(from, to, 1);
        tabsModel.reindex();

        // fix selected tab
        if (selectedTabIndex === from) {
            selectedTabIndex = to;
        } else if (selectedTabIndex <= to && selectedTabIndex >= from) {
            selectedTabIndex -= 1;
        } else {
            internal.sync();
        }

        return true;
    }

    /*!
      Removes the Tab from the given \a index. Returns true if the \a index falls
      into 0..\l count - 1 boundary and the operation succeeds, and false on error.
      The function removes also the pre-declared tabs. These can be added back using
      \l addTab or \l insertTab by specifying the instance of the Tab to be added as
      component. The \l selectedTabIndex is updated if is affected by the removal
      (it is identical or greater than the tab index to be removed).
      */
    function removeTab(index) {
        if (index < 0 || index >= count) return false;
        var tab = tabsModel.get(index).tab;
        var activeIndex = (selectedTabIndex >= index) ? MathUtils.clamp(selectedTabIndex, 0, count - 2) : -1;

        tabsModel.remove(index);
        tabsModel.reindex();
        // move active tab if needed
        if (activeIndex >= 0) {
            selectedTabIndex = activeIndex;
        }

        if (tab.__protected.dynamic) {
            tab.destroy();
        } else {
            // pre-declared tab, mark it as removed, so we don't update it next time
            // the tabs stack children is updated
            tab.__protected.removedFromTabs = true;
        }

        if (activeIndex < 0) {
            internal.sync();
        }
        return true;
    }

    /*!
      \internal
      required by TabsStyle
     */
    ListModel {
        id: tabsModel

        function listModel(tab) {
            return {"title": tab.title, "tab": tab};
        }

        function updateTabList(tabsList) {
            for (var i in tabsList) {
                var tab = tabsList[i];
                if (internal.isTab(tab)) {
                    // make sure we have the right parent
                    tab.parent = tabStack;

                    if (!tab.__protected.inserted) {
                        tab.__protected.index = count;
                        tab.__protected.inserted = true;
                        append(listModel(tab));
                    } else if (!tab.__protected.removedFromTabs && tabsModel.count > tab.index) {
                        get(tab.index).title = tab.title;
                    }
                }
            }
            internal.sync();
        }

        function reindex(from) {
            var start = 0;
            if (from !== undefined) {
                start = from + 1;
            }

            for (var i = start; i < count; i++) {
                var tab = get(i).tab;
                tab.__protected.index = i;
            }
        }
    }

    Item {
        anchors.fill: parent
        id: tabStack

        onChildrenChanged: tabsModel.updateTabList(children)
    }

    QtObject {
        id: internal
        property Header header: tabs.__propagated ? tabs.__propagated.header : null

        function isTab(item) {
            if (item && item.hasOwnProperty("__isPageTreeNode")
                    && item.__isPageTreeNode && item.hasOwnProperty("title")
                    && item.hasOwnProperty("page")) {
                return true;
            } else {
                return false;
            }
        }

        function sync() {
            if (tabBar && tabBar.__styleInstance && tabBar.__styleInstance.hasOwnProperty("sync")) {
                tabBar.__styleInstance.sync();
            }
            if (tabs.active && internal.header) internal.header.show();
            // deprecated, however use it till we remove it completely
            tabs.modelChanged();
        }
    }

    Binding {
        target: internal.header
        property: "contents"
        value: tabs.active ? tabs.tabBar: null
        when: internal.header && tabs.active
    }
}
