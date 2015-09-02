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
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import "tree.js" as Tree

/*!
  \qmltype AdaptivePageLayout
  \inqmlmodule Ubuntu.Components 1.3
  \since Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief View with multiple columns of Pages.

  The component provides a flexible way of viewing a stack of pages in one or
  more columns. Unlike in PageStack, there can be more than one Page active at
  a time, depending on the number of the columns in the view.

  AdaptivePageLayout stores pages added in a tree. Pages are added relative to a
  given page, either as sibling (\l addPageToCurrentColumn) or as child
  (\l addPageToNextColumn). This means that removing a non-leaf page from the Page
  tree will remove all its children from the page tree.

  The columns are populated from left to right. The column a page is added to is
  detected based on the source page that is given to the functions adding the page.
  The pages can be added either to the same column the source page resides or to
  the column next to the source page. Giving a null value to the source page will
  add the page to the leftmost column of the view.

  The primary page, the very first page must be specified through the \l primaryPage
  property. The property cannot be changed after component completion and can hold
  a Page instance, a Component or a url to a document defining a Page. The page
  cannot be removed from the view.

  \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    MainView {
        width: units.gu(100)
        height: units.gu(60)

        AdaptivePageLayout {
            anchors.fill: parent
            primaryPage: page1
            Page {
                id: page1
                title: "Main page"
                Column {
                    Button {
                        text: "Add Page2 above " + page1.title
                        onClicked: page1.pageStack.addPageToCurrentColumn(page1, page2)
                    }
                    Button {
                        text: "Add Page3 next to " + page1.title
                        onClicked: page1.pageStack.addPageToNextColumn(page1, page3)
                    }
                }
            }
            Page {
                id: page2
                title: "Page #2"
            }
            Page {
                id: page3
                title: "Page #3"
            }
        }
    }
  \endqml

  \note Observe the use of the \c Page::pageStack property in the example above.
  The same property is used to share the AdaptivePageLayout instance the Page is
  used in, therefore the same page can be used in a PageStack or in an AdaptivePageLayout.
  However implementations must make sure the desired PageStack or AdaptivePageLayout
  function exists in the instance before using it.

  AdaptivePageLayout supports adaptive column handling. When the number of columns changes at
  runtime the pages are automatically rearranged.

  By default the component splits the layout in two columns when the width of the
  layout exceeds 80 grid units. The first column is sized to 40 grid unit width and
  the second one to fill the rest of the remaining space. When the 80 grid unit breakpoint
  is reached, the component will switch from one column to two, and vice versa.
  These defaults can be overridden through the \l layouts property by defining the
  possible layouts, their column sizing and the breakpoints when the layouts should
  be activated. PageColumn configurations must appear in the same order (from left
  to right) as the columns appear in the layout. If none of the layouts condition
  is met, a one column layout will be used.

  \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    MainView {
        width: units.gu(100)
        height: units.gu(60)

        AdaptivePageLayout {
            anchors.fill: parent
            primaryPage: page1
            layouts: PageColumnsLayout {
                when: width > units.gu(80)
                // column #0
                PageColumn {
                    minimumWidth: units.gu(30)
                    maximumWidth: units.gu(60)
                    preferredWidth: units.gu(40)
                }
                // column #1
                PageColumn {
                    fillWidth: true
                }
            }

            Page {
                id: page1
                title: "Main page"
                Column {
                    Button {
                        text: "Add Page2 above " + page1.title
                        onClicked: page1.pageStack.addPageToCurrentColumn(page1, page2)
                    }
                    Button {
                        text: "Add Page3 next to " + page1.title
                        onClicked: page1.pageStack.addPageToNextColumn(page1, page3)
                    }
                }
            }
            Page {
                id: page2
                title: "Page #2"
            }
            Page {
                id: page3
                title: "Page #3"
            }
        }
    }
  \endqml

  A column is considered to be resizable if the \l PageColumn::minimumWidth and
  \l PageColumn::maximumWidth configuration differs. This implies that if a column
  is not meant to be resized, it should have \l PageColumn::minimumWidth and
  \l PageColumn::maximumWidth set to the same value. In the example above, the
  first column can be resized to a minimum of 30, and a maximum of 60 grid units,
  and the preferred width is set to 40 grid units. This width is set every time
  the layout is activated.

  \sa PageStack, PageColumnsLayout, PageColumn
*/

PageTreeNode {
    id: layout

    Page {
        // AdaptivePageLayout has its own split headers, so
        //  disable the application header.
        id: appHeaderControlPage
        head {
            locked: true
            visible: false
        }
        // title is set in attachPage() when the attached Page.column === 0
    }

    /*!
      The property holds the first Page which will be added to the view. If the
      view has more than one column, the page will be added to the leftmost column.
      The property can hold either a Page instance, a component holding a Page
      or a QML document defining the Page. The property cannot be changed after
      component completion.
      */
    property Page primaryPage

    /*!
      \qmlproperty int columns
      \readonly
      The property holds the number of columns shown in the layout.
      */
    readonly property alias columns: d.columns

    /*!
      The property holds the different layout configurations overriding the default
      configurations. Defaults to an empty list.
      \sa PageColumnsLayout
      */
    property list<PageColumnsLayout> layouts

    /*!
      \qmlmethod object addPageToCurrentColumn(Item sourcePage, var page[, var properties])
      Adds a \c page to the column the \c sourcePage resides in and removes all pages
      from the higher columns. \c page can be a Component or a file.
      \c properties is a JSON object containing properties to be set when page
      is created. \c sourcePage must be active.

      The function creates the new page asynchronously if the new \c page to be
      added is a Component or a QML document. In this case the function returns
      an incubator which can be used to track the page creation.For more about
      incubation in QML and creating components asynchronously, see
      \l {http://doc.qt.io/qt-5/qml-qtqml-component.html#incubateObject-method}
      {Component.incubateObject()}.
      The following example removes an element from the list model whenever the
      page opened in the second column is closed. Note, the example must be run
      on desktop or on a device with at least 90 grid units screen width.
      \qml
      import QtQuick 2.4
      import Ubuntu.Components 1.3

      MainView {
          width: units.gu(90)
          height: units.gu(70)

          Component {
              id: page2Component
              Page {
                  title: "Second Page"
                  Button {
                      text: "Close me"
                      onClicked: pageStack.removePages(pageStack.primaryPage);
                  }
              }
          }

          AdaptivePageLayout {
              id: pageLayout
              anchors.fill: parent
              primaryPage: Page {
                  title: "Primary Page"
                  ListView {
                      id: listView
                      anchors.fill: parent
                      model: 10
                      delegate: ListItem {
                          Label { text: modelData }
                          onClicked: {
                              var incubator = pageLayout.addPageToNextColumn(pageLayout.primaryPage, page2Component);
                              if (incubator && incubator.status == Component.Loading) {
                                  incubator.onStatusChanged = function(status) {
                                      if (status == Component.Ready) {
                                          // connect page's destruction to decrement model
                                          incubator.object.Component.destruction.connect(function() {
                                              listView.model--;
                                          });
                                      }
                                  }
                              }
                          }
                      }
                  }
              }
          }
      }
      \endqml

      \sa {http://doc.qt.io/qt-5/qml-qtqml-component.html#incubateObject-method}{Component.incubateObject}
      */
    function addPageToCurrentColumn(sourcePage, page, properties) {
        var nextColumn = d.columnForPage(sourcePage) + 1;
        // Clear all following columns. If we do not do this, we have no way to
        //  determine which page needs to be on top when the view is resized
        //  to have a single column.
        d.tree.prune(nextColumn);
        for (var i = nextColumn; i < d.columns; i++) {
            d.updatePageForColumn(i);
        }
        return d.addPageToColumn(nextColumn - 1, sourcePage, page, properties);
    }

    /*!
      \qmlmethod Item addPageToNextColumn(Item sourcePage, var page[, var properties])
      Remove all previous pages from the next column (relative to the column that
      holds \c sourcePage) and all following columns, and then add \c page to the next column.
      If \c sourcePage is located in the
      rightmost column, the new page will be pushed to the same column as \c sourcePage.
      The return value is the same as in \l addPageToCurrentColumn case.
      */
    function addPageToNextColumn(sourcePage, page, properties) {
        var nextColumn = d.columnForPage(sourcePage) + 1;
        d.tree.prune(nextColumn);
        for (var i = nextColumn; i < d.columns; i++) {
            d.updatePageForColumn(i);
        }
        return d.addPageToColumn(nextColumn, sourcePage, page, properties);
    }

    /*!
      \qmlmethod void removePages(Item page)
      The function removes and deletes all pages up to and including \c page
      is reached. If the \a page is the same as the \l primaryPage, only its child
      pages will be removed.
      */
    function removePages(page) {
        var nodeToRemove = d.getWrapper(page);
        var removedNodes = d.tree.chop(nodeToRemove, page != layout.primaryPage);
        for (var i = removedNodes.length-1; i >= 0; i--) {
            var node = removedNodes[i];
            d.updatePageForColumn(node.column);
        }
    }

    /*
      internals
      */

    Component.onCompleted: {
        // check layout configuration
        if (layouts.length > 0) {
            d.prepareLayouts();
        } else {
            d.relayout();
        }
        d.completed = true;
        if (primaryPage) {
            var wrapper = d.createWrapper(primaryPage);
            d.addWrappedPage(wrapper);
        } else {
            console.warn("No primary page set. No pages can be added without a primary page.");
        }
    }
    onPrimaryPageChanged: {
        if (d.completed) {
            console.warn("Cannot change primaryPage after completion.");
            return;
        }
    }
    onLayoutsChanged: {
        if (d.completed) {
            // only deal with this if the layouts array changes after completion
            // to avoid unnecessary rendering
            d.prepareLayouts();
        }
    }

    QtObject {
        id: d

        property bool completed: false
        property var tree: new Tree.Tree()

        property int columns: !layout.layouts.length ?
                                  (layout.width >= units.gu(80) ? 2 : 1) :
                                  (activeLayout ? activeLayout.data.length : 1)
        property PageColumnsLayout activeLayout: null
        property list<PageColumnsLayout> prevLayouts

        /*! internal */
        onColumnsChanged: {
            if (columns <= 0) {
                console.warn("There must be a minimum of one column set.");
                columns = 1;
            }
            d.relayout();
        }
        property real defaultColumnWidth: units.gu(40)
        onDefaultColumnWidthChanged: body.applyMetrics()

        function createWrapper(page, properties) {
            var wrapperComponent = Qt.createComponent("PageWrapper.qml");
            var wrapperObject = wrapperComponent.createObject(hiddenPages, {synchronous: false});
            wrapperObject.pageStack = layout;
            wrapperObject.properties = properties;
            // set reference last because it will trigger creation of the object
            //  with specified properties.
            wrapperObject.reference = page;
            return wrapperObject;
        }

        function addWrappedPage(pageWrapper) {
            pageWrapper.parentWrapper = d.getWrapper(pageWrapper.parentPage);
            tree.add(pageWrapper.column, pageWrapper.parentWrapper, pageWrapper);
            var targetColumn = MathUtils.clamp(pageWrapper.column, 0, d.columns - 1);
            // replace page holder's child
            var holder = body.children[targetColumn];
            holder.detachCurrentPage();
            if ((pageWrapper.incubator && pageWrapper.incubator.status == Component.Ready) || pageWrapper.object) {
                holder.attachPage(pageWrapper);
            } else {
                // asynchronous, connect to page load completion and attach when page is available
                pageWrapper.pageLoaded.connect(function () {
                    holder.attachPage(pageWrapper);
                });
            }
        }

        function getWrapper(page) {
            if (page && page.hasOwnProperty("parentNode")) {
                var w = page.parentNode;
                if (w && w.hasOwnProperty("object") && w.hasOwnProperty("reference")) {
                    if (w.object == page) {
                        return w;
                    } else {
                        print("Page is not wrapped by its parentNode. This should not happen!");
                        return null;
                    }
                } else {
                    // invalid wrapper
                    return null;
                }
            } else {
                // invalid page
                return null;
            }
        }

        function columnForPage(page) {
            var wrapper = d.getWrapper(page);
            return wrapper ? wrapper.column : 0;
        }

        function addPageToColumn(column, sourcePage, page, properties) {
            if (column < 0) {
                console.warn("Column must be >= 0.");
                return;
            }
            if (!sourcePage) {
                console.warn("No sourcePage specified. Page will not be added.");
                return;
            }
            var sourceWrapper = d.getWrapper(sourcePage);
            if (d.tree.index(sourceWrapper) === -1) {
                console.warn("sourcePage must be added to the view to add new page.");
                return;
            }

            // Check that the Page was not already added.
            if (typeof page !== "string" && !page.createObject) {
                // page is neither a url or a Component so it must be a Page object.

                var oldWrapper = getWrapper(page);
                if (oldWrapper && d.tree.index(oldWrapper) !== -1) {
                    console.warn("Cannot add a Page that was already added.");
                    return null;
                }
            }

            var newWrapper = d.createWrapper(page, properties);
            newWrapper.parentPage = sourcePage;
            newWrapper.column = column;
            d.addWrappedPage(newWrapper);
            return newWrapper.incubator;
        }

        // update the page for the specified column
        function updatePageForColumn(column) {
            var effectiveColumn = MathUtils.clamp(column, 0, d.columns - 1);
            var columnHolder = body.children[effectiveColumn];
            var newWrapper = tree.top(effectiveColumn, effectiveColumn < d.columns - 1);
            var oldWrapper = columnHolder.pageWrapper;

            if (newWrapper != oldWrapper) {
                columnHolder.detachCurrentPage();
                oldWrapper.parent = null;
                if (newWrapper) {
                    columnHolder.attachPage(newWrapper);
                }
                if (oldWrapper.canDestroy) {
                    oldWrapper.destroyObject();
                }
            }
        }

        // prepares layout management, listens on layout condition changes and performs re-layouting
        function prepareLayouts() {
            // disconnect from the previous layouts
            for (var i = 0; i < prevLayouts.length; i++) {
                prevLayouts[i].whenChanged.disconnect(updateLayout);
            }
            prevLayouts = layouts;
            for (var i = 0; i < layouts.length; i++) {
                layouts[i].whenChanged.connect(updateLayout);
            }
            // first time evaluation
            updateLayout();
        }

        // function called when one of the layout condition is satisfied
        function updateLayout() {
            var newLayout = null;
            for (var i = 0; i < layouts.length; i++) {
                // get the first affirmative condition
                if (layouts[i].when) {
                    newLayout = layouts[i];
                    break;
                }
            }
            d.activeLayout = newLayout;
        }

        // relayouts when column count changes
        function relayout() {
            if (body.children.length == d.columns) return;
            if (body.children.length > d.columns) {
                // need to remove few columns, the last ones
                while (body.children.length > d.columns) {
                    var holder = body.children[body.children.length - 1];
                    holder.detachCurrentPage();
                    holder.parent = null;
                    holder.destroy();
                }
            } else {
                var prevColumns = body.children.length;

                // add columns
                for (var i = 0; i < d.columns - prevColumns; i++) {
                    pageHolderComponent.createObject(body);
                }
            }
            rearrangePages();
        }

        function rearrangePages() {
            for (var column = d.columns - 1; column >= 0; column--) {
                var holder = body.children[column];
                var pageWrapper = tree.top(column, column < (d.columns - 1));
                if (!pageWrapper) {
                    continue;
                }
                if (!pageWrapper.parent) {
                    // this should never happen, so if it does, we have a bug!
                    console.error("Found a page which wasn't parented anywhere!", pageWrapper.object.title);
                    continue;
                }
                // detach current page from holder if differs
                if (holder.pageWrapper != pageWrapper) {
                    holder.detachCurrentPage();
                }
                if (pageWrapper.parent == hiddenPages) {
                    // add the page to the column
                    holder.attachPage(pageWrapper);
                } else if (pageWrapper.pageHolder != holder) {
                    // detach the pageWrapper from its holder
                    if (pageWrapper.pageHolder) {
                        pageWrapper.pageHolder.detachCurrentPage();
                    }
                    // then attach to this holder
                    holder.attachPage(pageWrapper);
                }
            }
        }
    }

    // default metrics
    Component {
        id: defaultMetrics
        PageColumn {
            fillWidth: __column == d.columns
            minimumWidth: d.defaultColumnWidth
        }
    }

    // Page holder component, can have only one Page as child at a time, all stacked pages
    // will be parented into hiddenPages
    Component {
        id: pageHolderComponent
        // Page uses the height of the parentNode for its height, so make
        //  the holder a PageTreeNode that determines the Page height.
        PageTreeNode {
            id: holder
            active: false
            objectName: "ColumnHolder" + column
            property PageWrapper pageWrapper
            property int column
            property alias config: subHeader.config
            property PageColumn metrics: getDefaultMetrics()
            readonly property real dividerThickness: units.dp(1)

            Layout.fillWidth: metrics.fillWidth
            Layout.fillHeight: true
            Layout.preferredWidth: MathUtils.clamp(metrics.preferredWidth, metrics.minimumWidth, metrics.maximumWidth)
            Layout.minimumWidth: metrics.minimumWidth
            Layout.maximumWidth: metrics.maximumWidth

            // prevent the pages from taking the app header height into account.
            __propagated: null
            Item {
                id: holderBody
                objectName: parent.objectName + "Body"
                anchors {
                    top: subHeader.bottom
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    rightMargin: dividerThickness
                }
                // we need to clip because the header does not have a background
                clip: true
            }

            property alias head: subHeader
            StyledItem {
                id: subHeader
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                }
                height: body.headerHeight

                styleName: "PageHeadStyle"
                theme.version: Ubuntu.toolkitVersion
                objectName: "Header" + column

                property real preferredHeight: subHeader.__styleInstance ?
                                                   subHeader.__styleInstance.implicitHeight :
                                                   0
                onPreferredHeightChanged: {
                    body.updateHeaderHeight(preferredHeight);
                }

                property PageHeadConfiguration config: null
                property Item contents: null

                property color dividerColor: layout.__propagated.header.dividerColor
                property color panelColor: layout.__propagated.header.panelColor

                visible: holder.pageWrapper && holder.pageWrapper.active

                // The multiColumn, page and showBackButton properties are used in
                //  PageHeadStyle to show/hide the back button.
                property var multiColumn: layout
                property var page: holder.pageWrapper ? holder.pageWrapper.object : null
                property bool showBackButton: {
                    if (!page) {
                        return false;
                    }
                    var parentWrapper;
                    try {
                        parentWrapper = d.tree.parent(holder.pageWrapper);
                    } catch(err) {
                        // Root node has no parent node.
                        return false;
                    }
                    var nextInColumn = d.tree.top(holder.column, holder.column < d.columns - 1, 1);
                    return parentWrapper === nextInColumn;
                }
            }

            Rectangle {
                id: verticalDivider
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                    rightMargin: dividerThickness
                }
                width: (column == (d.columns - 1)) || !pageWrapper ? 0 : units.dp(1)
                color: theme.palette.selected.background
                MouseArea {
                    id: resizerSensing
                    objectName: "Divider"
                    enabled: verticalDivider.width > 0
                    anchors {
                        fill: parent
                        leftMargin: enabled ? -units.gu(1) : 0
                        rightMargin: enabled ? -units.gu(1) : 0
                    }
                    cursorShape: Qt.SizeHorCursor
                    drag {
                        axis: Drag.XAxis
                        target: resizer
                        smoothed: false
                        minimumX: holder.Layout.minimumWidth
                        maximumX: holder.Layout.maximumWidth
                    }
                    onPressed: resizer.x = holder.Layout.preferredWidth
                }
                states: State {
                    name: "active"
                    when: resizerSensing.pressed
                    PropertyChanges {
                        target: verticalDivider
                        color: Qt.darker(theme.palette.normal.background, 1.5)
                    }
                }
                transitions: Transition {
                    from: ""
                    to: "*"
                    reversible: true
                    ColorAnimation {
                        target: verticalDivider
                        property: "color"
                        duration: UbuntuAnimation.SlowDuration
                    }
                }
            }
            Item {
                id: resizer
                height: parent.height
                onXChanged: holder.Layout.preferredWidth = x
            }

            function attachPage(page) {
                pageWrapper = page;
                pageWrapper.parent = holderBody;
                pageWrapper.pageHolder = holder;
                pageWrapper.active = true;

                if (pageWrapper.object.hasOwnProperty("head")) {
                    subHeader.config = pageWrapper.object.head;
                }
                if (pageWrapper.column === 0) {
                    // set the application title
                    appHeaderControlPage.title = pageWrapper.object.title;
                }
            }
            function detachCurrentPage() {
                if (!pageWrapper) return undefined;
                var wrapper = pageWrapper;
                // remove header
                wrapper.active = false;
                subHeader.config = null;
                pageWrapper = null;
                wrapper.parent = hiddenPages;
                wrapper.pageHolder = null;
                return wrapper;
            }

            function getDefaultMetrics() {
                var result = defaultMetrics.createObject(holder);
                result.__column = Qt.binding(function() { return holder.column + 1; });
                return result;
            }
        }
    }

    /*! \internal */
    // Pages declared as children will be placed directly into hiddenPages
    default property alias data: hiddenPages.data
    Item {
        id: hiddenPages
        objectName: "HiddenPagePool"
        visible: false
        // make sure nothing is shown eventually
        clip: true
    }

    // Holds the columns holding the pages visible. Each column has only one page
    // as child, the invisible stacked ones are all stored in the hiddenPages
    // component. The stack keeps the column index onto which those should be moved
    // once they become visible.
    RowLayout {
        id: body
        objectName: "body"
        anchors.fill: parent
        spacing: 0

        property real headerHeight: 0

        function updateHeaderHeight(newHeight) {
            if (newHeight > body.headerHeight) {
                body.headerHeight = newHeight;
            } else {
                var h = 0;
                var subHeight = 0;
                for (var i = 0; i < children.length; i++) {
                    subHeight = children[i].head.preferredHeight;
                    if (subHeight > h) h = subHeight;
                }
                body.headerHeight = h;
            }
        }

        onChildrenChanged: {
            // all children should have Layout.fillWidth false, except the last one
            for (var i = 0; i < children.length; i++) {
                children[i].column = i;
            }
            applyMetrics();
        }

        function applyMetrics() {
            for (var i = 0; i < children.length; i++) {
                var holder = children[i];
                // search for the column metrics
                var metrics = d.activeLayout ? d.activeLayout.data[i] : null;
                if (!metrics) {
                    metrics = holder.getDefaultMetrics();
                }
                holder.metrics = metrics;
                updateHeaderHeight(0);
            }
        }
    }
}
