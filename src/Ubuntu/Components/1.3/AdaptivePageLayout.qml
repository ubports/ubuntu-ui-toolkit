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
import Ubuntu.Components.Private 1.3

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

  The primary page, the very first page must be specified either through the
  \l primaryPage or \l primaryPageSource properties. \l primaryPage can only
  hold a Page instance, \l primaryPageSource can either be a Component or a
  url to a document defining a Page. \l primaryPageSource has precedence over
  \l primaryPage, and when set it will report the loaded Page through \l primaryPage
  property, and will replace any value set into that property.

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
                header: PageHeader {
                    id: header
                    title: "Main Page"
                }
                Column {
                    anchors.top: header.bottom
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
                header: PageHeader {
                    title: "Page #2"
                }
            }
            Page {
                id: page3
                header: PageHeader {
                    title: "Page #3"
                }
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
            primaryPageSource: page1Component
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

            Component {
                id: page1Component
                Page {
                    id: page1
                    header: PageHeader {
                        id: header
                        title: "Main page"
                    }
                    Column {
                        anchors.top: header.bottom
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
            }
            Page {
                id: page2
                header: PageHeader {
                    title: "Page #2"
                }
            }
            Page {
                id: page3
                header: PageHeader {
                    title: "Page #3"
                }
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

  When a \l Page with the \l Page::header property set is added to an
  AdaptivePageLayout, the AdaptivePageLayout will synchronize the height of that
  header with the height of the headers in other columns, i.e., the height of
  each of the headers will be set to the maximum implicitHeight of all the headers.

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
      The property can only hold a Page instance. When changed runtime (not by the
      AdaptivePageLayout component itself), the \l primaryPageSource property
      will be reset.
      */
    property Page primaryPage

    /*!
      The property specifies the source of the primaryPage in case the primary
      page is created from a Component or loaded from an external document. It
      has precedence over \l primaryPage.
      */
    property var primaryPageSource

    /*!
      The property drives the way the pages should be loaded, synchronously or
      asynchronously. Defaults to true.
      */
    property bool asynchronous: true

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
      added is a Component or a QML document and the \l asynchronous property is
      set to true. In this case the function returns an incubator which can be
      used to track the page creation. For more about incubation in QML and creating
      components asynchronously, see
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
                    header: PageHeader {
                        id: header
                        title: "Second Page"
                    }
                    Button {
                        anchors.top: header.bottom
                        text: "Close me"
                        onClicked: pageStack.removePages(pageStack.primaryPage);
                    }
                }
            }

            AdaptivePageLayout {
                id: pageLayout
                anchors.fill: parent
                primaryPage: Page {
                    header: PageHeader {
                        title: "Primary Page"
                        flickable: listView
                    }
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
        d.tree.prune(nextColumn);
        return d.addPageToColumn(nextColumn - 1, sourcePage, page, properties, function() {
            // Clear all following columns. If we do not do this, we have no way to
            //  determine which page needs to be on top when the view is resized
            //  to have a single column.
            for (var i = nextColumn; i < d.columns; i++) {
                d.updatePageForColumn(i);
            }
        });
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
        return d.addPageToColumn(nextColumn, sourcePage, page, properties, function() {
            for (var i = nextColumn; i < d.columns; i++) {
                d.updatePageForColumn(i);
            }
        });
    }

    /*!
      \qmlmethod void removePages(Item page)
      The function removes and deletes all pages up to and including \c page
      is reached. If the \a page is the same as the \l primaryPage, only its child
      pages will be removed.
      */
    function removePages(page) {
        d.removeAllPages(page, page != layout.primaryPage);
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
        if (primaryPageSource) {
            d.createPrimaryPage(primaryPageSource);
        } else if (primaryPage) {
            d.createPrimaryPage(primaryPage);
        }
        d.completed = true;
    }
    onPrimaryPageChanged: {
        if (!d.completed || d.internalUpdate) {
            return;
        }
        // reset the primaryPageSource
        d.internalPropertyUpdate("primaryPageSource", undefined);
        // clear the layout
        d.purgeLayout();
        // add the new page if valid
        if (primaryPage !== null) {
            d.createPrimaryPage(primaryPage);
        }
    }
    onPrimaryPageSourceChanged: {
        if (!d.completed || d.internalUpdate) {
            return;
        }
        // remove all pages first
        d.purgeLayout();
        // create the new primary page if a valid component is specified
        if (primaryPageSource) {
            d.createPrimaryPage(primaryPageSource);
        } else {
            d.internalPropertyUpdate("primaryPage", null);
        }
    }

    onLayoutsChanged: {
        if (d.completed) {
            // only deal with this if the layouts array changes after completion
            // to avoid unnecessary rendering
            d.prepareLayouts();
        }
    }

    /*! \internal - for testing purposes */
    readonly property alias __d:d
    QtObject {
        id: d

        property bool internalUpdate: false
        property bool completed: false
        property var tree: Tree{}

        property int columns: !layout.layouts.length ?
                                  (layout.width >= units.gu(80) ? 2 : 1) :
                                  (activeLayout ? activeLayout.data.length : 1)
        property PageColumnsLayout activeLayout: null
        property list<PageColumnsLayout> prevLayouts
        property Page prevPrimaryPage

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

        function internalPropertyUpdate(propertyName, value) {
            internalUpdate = true;
            layout[propertyName] = value;
            internalUpdate = false;
        }

        function createPrimaryPage(source) {
            var wrapper = d.createWrapper(source);
            if (wrapper.incubator) {
                wrapper.pageLoaded.connect(finalizeAddingPage.bind(wrapper));
                wrapper.incubator.onStatusChanged = function (status) {
                    if (status == Component.Ready) {
                        internalPropertyUpdate("primaryPage", wrapper.incubator.object);
                        prevPrimaryPage = wrapper.incubator.object;
                    }
                }
            } else {
                finalizeAddingPage(wrapper);
                prevPrimaryPage = wrapper.object;
            }
        }

        // remove all pages, including primaryPage
        function purgeLayout() {
            if (prevPrimaryPage) {
                removeAllPages(prevPrimaryPage, true);
                prevPrimaryPage = null;
            }
        }

        function createWrapper(page, properties) {
            var wrapperObject = pageWrapperComponent.createObject(hiddenPages, {synchronous: !layout.asynchronous});
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
            holder.attachPage(pageWrapper); // sets pageWrapper.pageHolder

            // set the back action for Page.header:
            var page = pageWrapper.object;
            if (page && page.hasOwnProperty("header") && page.header &&
                    page.header.hasOwnProperty("navigationActions")) {
                // Page.header is an instance of PageHeader.
                var backAction = backActionComponent.createObject(
                            pageWrapper, { 'wrapper': pageWrapper } );
                page.header.navigationActions = [ backAction ] ;
            }
        }

        function getWrapper(page) {
            if (page && page.hasOwnProperty("parentNode")) {
                var w = page.parentNode;
                if (w && w.hasOwnProperty("object") && w.hasOwnProperty("reference")) {
                    if (w.object == page) {
                        return w;
                    } else {
                        console.error("Page is not wrapped by its parentNode. This should not happen!");
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

        function finalizeAddingPage(newWrapper) {
            if (newWrapper === undefined) newWrapper = this;
            d.addWrappedPage(newWrapper);
        }

        function addPageToColumn(column, sourcePage, page, properties, cleanupFunc) {
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
            if (newWrapper.incubator) {
                newWrapper.pageLoaded.connect(cleanupFunc);
                newWrapper.pageLoaded.connect(finalizeAddingPage.bind(newWrapper));
            } else {
                cleanupFunc();
                finalizeAddingPage(newWrapper);
            }

            return newWrapper.incubator;
        }

        // removes all pages from the layout, and may include the page itself
        function removeAllPages(page, inclusive) {
            inclusive = typeof inclusive !== 'undefined' ? inclusive : true;
            var nodeToRemove = d.getWrapper(page);
            var removedNodes = d.tree.chop(nodeToRemove, inclusive);
            for (var i = removedNodes.length-1; i >= 0; i--) {
                var node = removedNodes[i];
                updatePageForColumn(node.column);
            }
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
                if (pageWrapper.parent == holder.hiddenPool || pageWrapper.parent == hiddenPages) {
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

    Component{
        id: pageWrapperComponent
        PageWrapper{
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

    // An instance will be added to each Page with
    Component {
        id: backActionComponent

        Action {
            // used when the Page has a Page.header property set.
            id: backAction
            objectName: "apl_back_action"
            iconName: "back"
            text: "Back"

            // set when backAction is created.
            property PageWrapper wrapper
            onTriggered: layout.removePages(wrapper.object)

            visible: {
                var parentWrapper = d.tree.parent(wrapper);

                if (!parentWrapper)
                    // Root node has no parent node.
                    return false;

                if (!wrapper.pageHolder) {
                    // columns are being re-arranged.
                    return false;
                }
                // wrapper.column is the virtual column, pageHolder.column the actual column.
                var column = wrapper.pageHolder.column;
                var nextInColumn = d.tree.top(column, column < d.columns - 1, 1);
                return parentWrapper === nextInColumn;
            }
        }
    }


    // Page holder component, can have only one Page as child at a time, all stacked pages
    // will be parented into hiddenPool
    Component {
        id: pageHolderComponent
        // Page uses the height of the parentNode for its height, so make
        //  the holder a PageTreeNode that determines the Page height.
        PageTreeNode {
            id: holder
            active: false
            objectName: "ColumnHolder" + column
            property var pageWrapper: pageWrapperComponent.createObject()
            property int column
            property alias config: subHeader.config
            property PageColumn metrics: getDefaultMetrics()
            readonly property real dividerThickness: units.dp(1)
            readonly property alias hiddenPool: hiddenItem

            Layout.fillWidth: metrics.fillWidth
            Layout.fillHeight: true
            Layout.preferredWidth: MathUtils.clamp(metrics.preferredWidth, metrics.minimumWidth, metrics.maximumWidth)
            Layout.minimumWidth: metrics.minimumWidth
            Layout.maximumWidth: metrics.maximumWidth

            property var page: pageWrapper ? pageWrapper.object : null
            property bool customHeader: page && page.hasOwnProperty("header") &&
                                        page.header
            onCustomHeaderChanged: {
                // do not change the holderBody anchors until the new page
                //  for the column has been set to prevent geometry changes in
                //  the current/previous page.
                if (page) {
                    if (customHeader) {
                        holderBody.anchors.top = holderBody.parent.top;
                    } else {
                        holderBody.anchors.top = subHeader.bottom;
                    }
                }
            }

            onPageChanged: body.updateHeaderHeight(0)
            Connections {
                target: page
                onHeaderChanged: body.updateHeaderHeight(0)
            }
            Connections {
                target: page ? page.header : null
                onImplicitHeightChanged: body.updateHeaderHeight(page.header.implicitHeight)
            }

            // prevent the pages from taking the app header height into account.
            __propagated: null
            Item {
                id: holderBody
                objectName: parent.objectName + "Body"
                anchors {
                    top: subHeader.bottom // updated in onCustomHeaderChanged
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    rightMargin: dividerThickness
                }
                Item {
                    id: hiddenItem
                    anchors.fill: parent
                    visible: false
                }
            }

            // subHeader is to be deprecated in UITK 1.4 and will be replaced
            //  by the Page.header property (introduced in 1.3).
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
                property color backgroundColor: layout.__propagated.header.backgroundColor

                visible: !customHeader && holder.pageWrapper && holder.pageWrapper.active

                // The multiColumn, page and showBackButton properties are used in
                //  PageHeadStyle to show/hide the back button.
                property var multiColumn: layout
                property alias page: holder.page // used in PageHeadStyle for the back button.
                property bool showBackButton: {
                    if (!page) {
                        return false;
                    }
                    var parentWrapper = d.tree.parent(holder.pageWrapper);

                    if (!parentWrapper) {
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
                color: theme.palette.normal.base
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
                        color: Qt.darker(theme.palette.normal.background, 1.7)
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

            function attachPage(wrapper) {
                pageWrapper = wrapper;
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
                wrapper.parent = hiddenPool;
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
    PageTreeNode {
        id: hiddenPages
        objectName: "HiddenPagePool"
        visible: false
        active: false
        // make sure nothing is shown eventually
        clip: true
        anchors.fill: parent
    }

    // Holds the columns holding the pages visible. Each column has only one page
    // as child, the invisible stacked ones are all stored in the hiddenPool
    // component within each column. The stack keeps the column index onto which
    // those should be moved once they become visible.
    RowLayout {
        id: body
        objectName: "body"
        anchors.fill: parent
        spacing: 0

        property real headerHeight: 0

        function updateHeaderHeight(newHeight) {
            var page;
            var i;
            if (newHeight > body.headerHeight) {
                body.headerHeight = newHeight;
            } else {
                var h = 0;
                var subHeight = 0;
                for (i = 0; i < children.length; i++) {
                    page = children[i].page;
                    if (page && page.hasOwnProperty("header") && page.header) {
                        subHeight = page.header.implicitHeight;
                    } else {
                        subHeight = children[i].head.preferredHeight;
                    }
                    if (subHeight > h) h = subHeight;
                }
                body.headerHeight = h;
            }

            // Update all the Page.header heights.
            for (i = 0; i < body.children.length; i++) {
                page = body.children[i].page;
                if (page && page.hasOwnProperty("header") && page.header) {
                    page.header.height = headerHeight;
                }
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
