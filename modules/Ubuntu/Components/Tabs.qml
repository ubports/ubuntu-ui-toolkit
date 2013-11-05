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
      The index of the currently selected tab.
      The first tab is 0, and -1 means that no tab is selected.
      The initial value is 0 if Tabs has contents, or -1 otherwise.
     */
    property int selectedTabIndex: tabsListModel.count > 0 ? 0 : -1

    /*!
      \preliminary
      The currently selected tab.
     */
    readonly property Tab selectedTab: (selectedTabIndex < 0) || (tabsListModel.count <= selectedTabIndex) ?
                                           null : tabsListModel.get(selectedTabIndex).tab

    /*!
      The page of the currently selected tab.
     */
    readonly property Item currentPage: selectedTab ? selectedTab.page : null

    /*!
      The \l TabBar that will be shown in the header
      and provides scrollable tab buttons.
     */
    property TabBar tabBar: TabBar {
        tabsItem: tabs
        model: tabsListModel
        visible: tabs.active
    }

    /*!
      \internal
      Used by the style to create the tabs header.
    */
//    property alias __tabs: tabsListModel

    /*!
      Children are placed in a separate item that has functionality to extract the Tab items.
      \qmlproperty list<Item> tabChildren
     */
    default property alias tabChildren: tabStack.children

    /*!
      \qmlproperty int count
      Contains the number of tabs in the Tabs component.
      */
    readonly property alias count: tabsListModel.count

    /*!
      Used by the tabs style to update the tabs header with the titles of all the tabs.
      This signal is used in an intermediate step in transitioning the tabs to a new
      implementation and may be removed in the future.
     */
    signal modelChanged()

    /*!
      Appends a Tab dynamically to the list of tabs. The \a title specifies the
      title of the Tab. The \a component can be either a Component, a URL to
      the Tab component to be loaded or an instance of a pre-declared tab that has
      been previously removed. The Tab's title will be replaced with the
      given \a title, unless if the given value is empty or undefined. The optional
      \a params defines parameters passed to the Tab.
      Returns the instance of the created Tab.
      */
    function addTab(title, component, params) {
        return insertTab(count, title, component, params);
    }
    /*!
      Inserts a Tab at the given index. If the \a index is less than 0, the Tab will
      be added to the front, and to the end if the \a index is greater than \l count.
      \a title, \a component and \a params are used in the same way as in \l addTab().
      Returns the instance of the created Tab.
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
        } else {
            title = tab.title;
        }

        // insert the created tab into the model
        index = MathUtils.clamp(index, 0, count);
        tab.__protected.inserted = true;
        tab.__protected.index = index;
        tabsListModel.insert(index, tabsListModel.listModel(tab));
        tabsListModel.reindex(index);
        tab.parent = tabStack;
        if (tabs.selectedTabIndex >= index) {
            // move the selected index to the next index
            tabs.selectedTabIndex += 1;
        } else {
            tabs.modelChanged();
        }
        return tab;
    }

    /*!
      Moves the tab from the given \a from position to the position given in \a to.
      Returns true if the indexes were in [0..\a count) boundary and if the operation
      succeeds, and false otherwise.
      */
    function moveTab(from, to) {
        if (from < 0 || from >= count || to < 0 || to >= count || from === to) return false;
        var tabFrom = tabsListModel.get(from).tab;
        var tabTo = tabsListModel.get(to).tab;

        // move tab
        tabsListModel.move(from, to, 1);
        tabsListModel.reindex();

        // fix selected tab
        if (selectedTabIndex === from) {
            selectedTabIndex = to;
        } else if (selectedTabIndex <= to && selectedTabIndex >= from) {
            selectedTabIndex -= 1;
        } else {
            tabs.modelChanged();
        }

        return true;
    }

    /*!
      Removes the Tab from the given \a index. Returns true if the \a index falls
      into [0..\a count) boundary and the operation succeeds, and false on error.
      The function removes also the pre-declared tabs. These can be added back using
      \l addTab or \l insertTab by specifying the instance of the Tab to be added as
      component.
      */
    function removeTab(index) {
        if (index < 0 || index >= count) return false;
        var tab = tabsListModel.get(index).tab;
        var activeIndex = (selectedTabIndex >= index) ? MathUtils.clamp(selectedTabIndex, 0, count - 2) : -1;

        tabsListModel.remove(index);
        tabsListModel.reindex();
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
            tabs.modelChanged();
        }
        return true;
    }

    /*!
      \internal
      required by TabsStyle
     */
    ListModel {
        id: tabsListModel

        function listModel(tab) {
            return {"title": tab.title, "tab": tab};
        }

        function updateTabList(tabsList) {
            for (var i in tabsList) {
                var tab = tabsList[i];
                if (internal.isTab(tab)) {
                    if (tab.parent === tabs) {
                        // tabs parented to Tabs should be removed
                        tab.parent = tabStack;
                        tab.Component.onDestroyed.connect(tabStack.dynamicRemove.bind(tab));
                    }

                    if (!tab.__protected.inserted) {
                        tab.__protected.index = count;
                        tab.__protected.inserted = true;
                        append(listModel(tab));
                    } else if (!tab.__protected.removedFromTabs && tabsListModel.count > tab.index) {
                        get(tab.index).title = tab.title;
                    }
                }
            }
            tabs.modelChanged();
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

        onChildrenChanged: tabsListModel.updateTabList(children)

        function dymanicRemove() {
            for (var i in tabStack.children) {
                if (this === tabStack.children[i]) {
                    tabs.removeTab(i);
                    break;
                }
            }
        }
    }

    /*! \internal */
    onChildrenChanged: tabStack.updateTabList(children)

    /*! \internal */
    onModelChanged: if (tabs.active && internal.header) internal.header.show()

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
    }

    Binding {
        target: internal.header
        property: "contents"
        value: tabs.active ? tabs.tabBar: null
        when: internal.header && tabs.active
    }
}
