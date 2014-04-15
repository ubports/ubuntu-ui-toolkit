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
import "mathUtils.js" as MathUtils

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
                    page: Loader {
                        parent: externalTab
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
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
                                iconName: "compose"
                                text: "Item "+modelData
                            }
                        }
                    }
                }
            }
        }

    \endqml
    As the example above shows, an external \l Page inside a \l Tab can be loaded using a Loader.
    Note that setting the top anchor or the height of the Loader would override the \l Page height.
    We avoid this because the \l Page automatically adapts its height to accommodate for the header.

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

    \section2 Dynamic tabs
    So far all Tab elements were pre-declared, but there can be situations when
    tabs need to be added dynamically. There are two ways to solve this, depending
    on the output needed.

    \section3 Using Repeaters
    A Repeater can be used to create the necessary tabs depending on a given model.
    In this way the number of tabs will be driven by the model itself.
    An example of such a dynamic tab:
    \qml
    // DynamicTab.qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    Tabs {
        property alias model: tabRepeater.model
        Repeater {
            id: tabRepeater
            model: 5
            Tab {
                title: "Tab #" + index
                page: Page {
                    // [...]
                }
            }
        }
    }
    \endqml
    Note that in the example above the Tabs will be re-created each time the model
    changes. This will cause state losing of each Tab, which depending on the
    content type can be solved at some extent using StateSaver. Using a Loader
    or specifying the Tab instance/component in the model the state can be preserved,
    however may increase code complexity.

    \section3 Dynamic tabs
    Tabs provides functions to add Tab elements dynamically on runtime, without
    destroying the state of the existing tabs. You can add, move and remove any
    kind of Tab element, pre-declared or dynamically created ones. When removing
    pre-declared tabs, those will all be held back and hidden by Tabs, and can be
    added back any time either to the same or to a different position.

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    MainView {
        width: units.gu(40)
        height: units.gu(71)

        Component {
            id: dynamicTab
            Tab {
                page: Page {
                    Label {
                        text: title
                        anchors.centerIn: parent
                    }
                }
            }
        }
        Tabs {
        id: tabs
            Tab {
                title: "Main tab"
                page: Page {
                    toolbar: ToolbarItems {
                        ToolbarButton {
                            text: "remove predeclared"
                            onTriggered: tabs.removeTab(preDeclared.index)
                        }
                        ToolbarButton {
                            text: "add new"
                            onTriggered: tabs.addTab("New tab", dynamicTab)
                        }
                        ToolbarButton {
                            text: "insert predeclared"
                            onTriggered: tabs.insertTab("", 0)
                        }
                    }
                }
            }
            Tab {
            id: preDeclared
                title: "Pre-declared tab"
                page: Page {
                    Label {
                        text: "This is a predeclared tab at index #" + index
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }
    \endqml

    \section3 Using Repeater and functions together
    Repeaters re-create their delegates as many times the model changes. Tabs added
    or moved in between the tabs maintained by the Repeater, as well as reordered
    through the Tabs functions will be re-arranged once the Repeater's model changes.
    This should be taken into account when designing the application, and the use
    of Repeater and functions toghether should be avoided if possible, or at least
    Repeater should always add tabs to te tail of the tab stack, and no tab insertion
    happens in that area.
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
      \deprecated
      Children are placed in a separate item that has functionality to extract
      the Tab items.
      Note: this property is deprecated. Tab components are directly parented
      to Tabs' data property.
      \qmlproperty list<Item> tabChildren
     */
    property alias tabChildren: tabs.data

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
      title of the Tab. The \a tab can be either a Component, a URL to a Tab
      component to be loaded or an instance of a pre-declared tab that has been
      previously removed. The Tab's title will be replaced with the given \a title,
      unless the value is an empty string or undefined.
      Returns the instance of the added Tab.
      */
    function addTab(title, tab) {
        return insertTab(count, title, tab);
    }

    /*!
      Inserts a Tab at the given index. If the \a index is less or equal than 0,
      the Tab will be added to the front, and to the end of the tab stack in case
      the \a index is greater than \l count. \a title and \a tab are used in the
      same way as with \l addTab().
      Returns the instance of the inserted Tab.
      */
    function insertTab(index, title, tab) {
        // check if the given component is a Tab instance
        var tabObject = null;

        if (typeof tab === "string") {
            // we have a URL
            var tabComponent = Qt.createComponent(tab);
            if (tabComponent.status === Component.Error) {
                console.error(tabComponent.errorString());
                return null;
            }
            tabObject = tabComponent.createObject();
            tabObject.__protected.dynamic = true;
        } else if (tab.hasOwnProperty("createObject")) {
            // we have a Component
            tabObject = tab.createObject();
            tabObject.__protected.dynamic = true;
        } else if (tab.hasOwnProperty("parent") && tab.parent === trashedTabs) {
            // we have a pre-declared tab that has been removed
            tabObject = tab;
        } else {
            console.error(i18n.tr("The object is not a URL, Component or a removed Tab: ") + tab);
            return null;
        }

        // fix title
        if (title !== undefined && title !== "") {
            tabObject.title = title;
        }

        // insert the created tab into the model
        index = MathUtils.clamp(index, 0, count);
        tabObject.__protected.inserted = true;
        tabObject.__protected.index = index;
        tabsModel.insertTab(tabObject, index);
        if (tabs.selectedTabIndex >= index) {
            // move the selected index to the next index
            tabs.selectedTabIndex += 1;
        } else {
            internal.sync();
        }
        return tabObject;
    }

    /*!
      The function returns the Tab from the given \a index, or null if the \a index
      is invalid (less than \c 0 and greater than \l count).
      */
    function getTab(index) {
        return (index >=0) && (index < count) ? tabsModel.get(index).tab : null;
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
        QuickUtils.moveItemBefore(tabFrom, tabTo);
        tabsModel.updateTabList(tabs.children);

        // fix selected tab
        if (selectedTabIndex === from) {
            selectedTabIndex = to;
        } else if (selectedTabIndex >= Math.min(from, to) && selectedTabIndex <= Math.max(from, to)) {
            selectedTabIndex--;
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

        // remove from Tabs; Tabs children change will remove the tab from the model
        tab.parent = null;
        if (tab.__protected.dynamic) {
            tab.destroy();
        } else {
            // pre-declared tab, mark it as removed, so we don't update it next time
            // the tabs stack children is updated
            tab.parent = trashedTabs;
        }

        // move active tab if needed
        if (activeIndex >= 0 && activeIndex !== selectedTabIndex) {
            selectedTabIndex = activeIndex;
        } else {
            internal.sync();
        }

        return true;
    }

    /*! \internal */
    onChildrenChanged: {
        internal.connectToRepeaters(tabs.children);
        tabsModel.updateTabList(tabs.children);
    }

    /*!
      \internal
      required by TabsStyle
     */
    ListModel {
        id: tabsModel

        property bool updateDisabled: false

        function listModel(tab) {
            return {"title": tab.title, "tab": tab};
        }

        function updateTabList(tabsList) {
            if (updateDisabled) return;
            var offset = 0;
            var tabIndex = -1;
            for (var i in tabsList) {
                var tab = tabsList[i];
                if (internal.isTab(tab)) {
                    tabIndex = i - offset;
                    // make sure we have the right parent
//                    tab.parent = tabs;

                    if (!tab.__protected.inserted) {
                        tab.__protected.index = tabIndex;
                        tab.__protected.inserted = true;
                        insert(tabIndex, listModel(tab));
                    } else {
                        get(tab.index).title = tab.title;
                    }

                    // always makes sure that tabsModel has the same order as tabsList
                    // but move only if there is more than one item in the list
                    if (count > 1) {
                        move(tab.__protected.index, tabIndex, 1);
                    }
                    reindex();
                } else {
                    // keep track of children that are not tabs so that we compute
                    // the right index for actual tabs
                    offset += 1;
                }
            }
            // remove deleted tabs, those should be at the end of the list by now
            if ((tabIndex >= 0) && (tabIndex + 1) < count) {
                remove(tabIndex + 1, count - tabIndex - 1);
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
                if (!tab) wait(1);
                tab = get(i).tab;
                if (tab) {
                    tab.__protected.index = i;
                } else {
                    console.error("Invalid Tab at index", i, get(i).title)
                }
            }
        }

        function insertTab(tab, index) {
            // fix index
            if (index < 0) {
                index = 0;
            }
            // get the tab before which the item will be inserted
            var itemAtIndex = ((index >= 0) && (index < count)) ? get(index).tab : null;
            // disable update only if we insert, append can keep the logic rolling
            updateDisabled = (itemAtIndex !== null);
            insert(index, listModel(tab));
            tab.parent = tabs;
            updateDisabled = false;
            if (itemAtIndex) {
                QuickUtils.moveItemBefore(tab, itemAtIndex);
                updateTabList(tabs.children);
            }
        }
    }

    // invisible component stacking removed pre-declared components
    Item {
        id: trashedTabs
        visible: false
        opacity: 0.0
    }

    /*
      This timer is used when tabs are created using Repeaters. Repeater model
      element moves (shuffling) are causing re-stacking of the tab stack children
      which may not be realized at the time the rowsMoved/columnsMoved or layoutChanged
      signals are triggered. Therefore we use an idle timer to update the tabs
      model, so the tab stack is re-stacked by then.
      */
    Timer {
        id: updateTimer
        interval: 1
        running: false
        onTriggered: {
            tabsModel.updateTabList(tabs.children);
            internal.sync();
        }
    }

    Object {
        id: internal
        property Header header: tabs.__propagated ? tabs.__propagated.header : null

        Binding {
            target: tabBar
            property: "animate"
            when: (internal.header !== null) && internal.header.hasOwnProperty("animate")
            value: internal.header ? internal.header.animate : "false"
        }

        /*
          List of connected Repeaters to avoid repeater "hammering" of itemAdded() signal.
          */
        property var repeaters: []

        function sync() {
            if (tabBar && tabBar.__styleInstance && tabBar.__styleInstance.hasOwnProperty("sync")) {
                tabBar.__styleInstance.sync();
            }
            if (tabs.active && internal.header) {
                internal.header.show();
            }
            // deprecated, however use it till we remove it completely
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

        function isRepeater(item) {
            return (item && item.hasOwnProperty("itemAdded"));
        }

        function connectToRepeaters(children) {
            for (var i = 0; i < children.length; i++) {
                var child = children[i];
                if (internal.isRepeater(child) &&
                        (internal.repeaters !== undefined) &&
                        (internal.repeaters.indexOf(child) < 0)) {
                    internal.connectRepeater(child);
                }
            }
        }
        /* When inserting a delegate into its parent the Repeater does it in 3
           steps:
           1) sets the parent of the delegate thus inserting it in the list of
              children in a position that does not correspond to the position of
              the corresponding item in the model. At that point the
              childrenChanged() signal is emitted.
           2) reorder the delegate to match the position of the corresponding item
              in the model.
           3) emits the itemAdded() signal.

           We need to update the list of tabs (tabsModel) when the children are in the
           adequate order hence the workaround below. It connects to the itemAdded()
           signal of any repeater it finds and triggers an update of the tabsModel.

           Somewhat related Qt bug report:
           https://bugreports.qt-project.org/browse/QTBUG-32438
        */
        function updateTabsModel() {
            tabsModel.updateTabList(tabs.children);
        }

        /*
          Connects a Repeater and stores it so further connects will not happen to
          the same repeater avoiding in this way hammering.
          */
        function connectRepeater(repeater) {
            // store repeater
            repeaters.push(repeater);

            // connect destruction signal so we have a proper cleanup
            repeater.Component.onDestruction.connect(internal.disconnectRepeater.bind(repeater));

            // connect repeater's itemAdded and itemRemoved signals
            repeater.itemAdded.connect(internal.updateTabsModel);
            repeater.itemRemoved.connect(internal.removeTabFromModel);

            // check if the repeater's model is set, if not, connect to modelChanged to catch that
            if (repeater.model === undefined) {
                repeater.modelChanged.connect(internal.connectRepeaterModelChanges.bind(repeater));
            } else {
                connectRepeaterModelChanges(repeater);
            }
        }

        /*
          Disconnects the given repeater signals.
          */
        function disconnectRepeater() {
            this.itemAdded.disconnect(internal.updateTabsModel);
            this.itemRemoved.disconnect(internal.removeTabFromModel);
            this.modelChanged.disconnect(internal.connectRepeaterModelChanges);
        }

        /*
          Connects a Repeater's model change signals so we get notified whenever those change.
          This can be called either by the Repeater's modelChanged() signal, in which case the
          parameter is undefined, or from the connectRepeater() in case the model is given for
          the Repeater.
          */
        function connectRepeaterModelChanges(repeater) {
            if (repeater === undefined) {
                repeater = this;
            }

            /*
              Omit model types which are not derived from object (i.e. are [object Number]
              or [object Array] typed).

              JS 'instanceof' operator does not return true for all types of arrays (i.e
              for property var array: [....] it returns false). The safest way to detect
              whether the model is really an object we use the toString() prototype of
              the generic JS Object.

              Inspired from http://perfectionkills.com/instanceof-considered-harmful-or-how-to-write-a-robust-isarray/
              */
            if (Object.prototype.toString.call(repeater.model) !== "[object Object]") {
                return;
            }

            // other models are most likely derived from QAbstractItemModel,
            // therefore we can safely connect to the signals to get notified about refreshes
            repeater.model.rowsMoved.connect(updateTimer.restart);
            repeater.model.columnsMoved.connect(updateTimer.restart);
            repeater.model.layoutChanged.connect(updateTimer.restart);
        }

        // clean items removed trough a repeater
        function removeTabFromModel(index, item) {
            // cannot use index as that one is relative to the Repeater's model, therefore
            // we need to look after the Tabs models' role to find out which item to remove
            for (var i = 0; i < tabsModel.count; i++) {
                if (tabsModel.get(i).tab === item) {
                    tabsModel.remove(i);
                    break;
                }
            }
            tabsModel.reindex();
        }
    }

    Binding {
        target: internal.header
        property: "contents"
        value: tabs.active ? tabs.tabBar: null
        when: internal.header && tabs.active
    }
}
