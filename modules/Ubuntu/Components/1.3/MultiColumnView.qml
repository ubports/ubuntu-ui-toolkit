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
import "stack.js" as Stack

/*!
  \qmltype MultiColumnView
  \inqmlmodule Ubuntu.Components 1.3
  \since Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief View with multiple columns of Pages.

  The component provides a flexible way of viewing a stack of pages in one or
  more columns. Unlike in PageStack, there can be more than one Page active at
  a time, depending on the number of the columns in the view.

  MultiColumnView stores pages added in a tree. Pages are added relative to a
  given page, either as sibling (\l addPageToCurrentColumn) or as child
  (\l addPageToNextColumn). This means that removing a non-leaf page from the Page
  tree will remove all its children from the page tree.

  The columns are populated from left to right. The column a page is added to is
  detected based on the source page that is given to the functions adding the page.
  The pages can be added either to the same column the source page resides or to
  the column next to the source page. Giving a null value to the source page will
  add the page to the leftmost column of the view.

  \note Unlike PageStack, the component does not fill its parent content.

  \qml
  import QtQuick 2.4
  import Ubuntu.Components 1.3

  MainView {
      width: units.gu(80)
      height: units.gu(71)

      MultiColumnView {
          anchors.fill: parent
          columns: 2
          primaryPage: page1
          Page {
              id: page1
              title: "Main page"
              Column {
                  Button {
                      text: "Add Page2 above " + title
                      onClicked: page1.pageStack.addPageToCurrentColumn(page1, page2)
                  }
                  Button {
                      text: "Add Page3 next to " + title
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

  Column widths are controlled by the \l defaultColumnWidth property and the \l columnMetrics
  properties. The \l columnMetrics contains a list of metrics configuring a specific
  column. If no metrics are set, the component will use \l defaultColumnWidth on
  each column. If the component is set to have one column only, the content will
  be stretched to the entire component area no matter of the metrics specified.
  When multiple columns are set, the last column is set to fill the available
  width and the rest are configured with the \l defaultColumnWidth. This behavior
  can be changed by specifying the \l ColumnMetrics::fillWidth for the column that
  needs to fill the available width. There can be more columns filling the available
  width at a time.

  Let's modify the example above, to have 3 columns, where columns 1 and 3
  should have fixed widths of 40 GU and column 2 should fill to the space
  available. The code handling this would look as follows:
  \qml
  MultiColumnView {
      columns: 3
      defaultColumnWidth: units.gu(40)
      columnMetrics: [
          ColumnMetrics {
              column: 2
              fillWidth: true
          },
          ColumnMetrics {
              column: 3
              fillWidth: false
          }
      ]
  }
  \endqml

  MultiColumnView supports adaptive column handling. When columns number changes
  runtime, the pages are automatically rearranged to the closest columns they were
  added to. To understand it better, let's take the following example:
  \qml
  import QtQuick 2.4
  import Ubuntu.Components 1.3

  MainView {
      width: units.gu(120)
      height: units.gu(71)

      MultiColumnView {
          anchors.fill: parent
          columns: width > units.gu(100) ? 3 :
                        (width >= units.gu(80) ? 2 : 1)
          primaryPage: page1
          Page {
              id: page1
              title: "Main page"
              Button {
                  text: "Add Page2 next to " + title
                  onClicked: page1.pageStack.addPageToNextColumn(page1, page2)
              }
          }
          Page {
              id: page2
              title: "Page #2"
              Button {
                  text: "Add Page3 next to " + title
                  onClicked: page2.pageStack.addPageToNextColumn(page2, page3)
              }
          }
          Page {
              id: page3
              title: "Page #3"
          }
      }
  }
  \endqml

  When the code is run on desktop, it will launch with a space for three columns.
  \c page1 is set to be the primary page, \c page2 will be added to column next to
  \c page1 (to column 2) and \c page3 next to \c page2 (column 3). When the window
  is resized to have its size below 100 GU, the component will switch to 2 column
  mode, and \c page3 will be placed in the last column, and the header for \c page2
  will have a back button, indicating that there is a page below it. If the window
  is resized to contain only one column, all pages will be shown in that column, so
  the component will act as PageStack. Resizing the window back to 2 respectively
  3 columns will place the pages side-by-side.

  \note In the above example if \c page2 is removed, that will remove all its child
  pages, meaning \c page3 will also be removed.

  \sa PageStack, ColumnMetrics
*/

PageTreeNode {
    id: multiColumnView

    /*!
      The property holds the first Page which will be added to the view. If the
      view has more than one column, the page will be added to the leftmost column.
      The proeprty can hold either a Page instance, a component holding a Page
      or a QML document defining the Page. The property cannot be changed after
      component completion.
      */
    property var primaryPage

    /*!
      Specifies the number of columns in the view. A condition must be set to
      control the number of columns depending on the space available.
      \qml
      MultiColumnView {
           id: view
           columns: view.width > units.gu(50) ? 2 : 1
      }
      \endqml
      */
    property int columns: 1

    /*!
      The property specifies the default width of each column. The property is
      applied on each column. If the \a minimumWidth specified for the column is
      bigger than this value, the minimum width will be applied.
      */
    property real defaultColumnWidth: units.gu(40)

    /*!
      The property configures the size constraints and area filling for columns.
      If a column is not specified, the default sizing and filling will be applied.
      By default, only the last column is filling the available width, al other
      columns are sized to \l defaultColumnWidth as maximum. By default the list
      is empty. Only columns requiring special handling than the default should
      be specified.
      */
    property list<ColumnMetrics> columnMetrics

    /*!
      \qmlmethod Item addPageToCurrentColumn(Item sourcePage, var page[, var properties])
      Adds a \c page to the column the \c sourcePage resides in. If \c sourcePage
      is null, the \c page will be added to the leftmost column. \c page can be a
      Component or a file. \c properties is a JSon object containing properties
      to be set when page is created. \c sourcePage must be active. Returns the
      instance of the page created.
      */
    function addPageToCurrentColumn(sourcePage, page, properties) {
        if (columns <= 0) {
            return;
        }
        var wrapper = d.createWrapper(page, properties);
        wrapper.column = d.columnForPage(sourcePage);
        wrapper.parentPage = sourcePage;
        d.addPage(wrapper);
        return wrapper.object;
    }

    /*!
      \qmlmethod Item addPageToNextColumn(Item sourcePage, var page[, var properties])
      Same as \l addPageToCurrentColumn except that the \c page is added to the column
      next to the one the \c sourcePage resides. If \c sourcePage is null, the new
      page will be added to the leftmost column. If \c sourcePage is located in the
      rightmost column, the new page will be pushed to the same column as \c sourcePage.
      */
    function addPageToNextColumn(sourcePage, page, properties) {
        if (columns <= 0) {
            return;
        }
        var wrapper = d.createWrapper(page, properties);
        wrapper.column = (!sourcePage) ? 0 : d.columnForPage(sourcePage) + 1;
        wrapper.parentPage = sourcePage;
        d.addPage(wrapper);
        return wrapper.object;
    }

    /*!
      \qmlmethod void removePages(Item page)
      The function removes and deletes all pages from the view columns until \c page
      is reached.
      */
    function removePages(page) {
        // remove nodes which have page as ascendant
        var node = d.stack.top();
        while (node && node.childOf(page)) {
            d.popAndSetPageForColumn(node);
            node = d.stack.top();
        }
        node = d.stack.top();
        while (node.object == page) {
            d.popAndSetPageForColumn(node);
            node = d.stack.top();
        }
    }

    /*
      internals
      */

    /*! internal */
    onColumnsChanged: {
        if (columns <= 0) {
            console.warn("There must me a minimum of one column set.");
        }
        d.relayout();
    }
    Component.onCompleted: {
        d.relayout();
        d.completed = true;
        if (primaryPage) {
            addPageToCurrentColumn(null, primaryPage);
        }
    }
    onPrimaryPageChanged: {
        if (d.completed) {
            console.error("Cannot change primaryPage after completion.");
            return;
        }
    }
    onDefaultColumnWidthChanged: body.applyMetrics()

    QtObject {
        id: d

        property bool completed: false
        property var stack: new Stack.Stack()

        function createWrapper(page, properties) {
            var wrapperComponent = Qt.createComponent("PageWrapper.qml");
            var wrapperObject = wrapperComponent.createObject(hiddenPages);
            wrapperObject.pageStack = multiColumnView;
            wrapperObject.properties = properties;
            // set reference last because it will trigger creation of the object
            //  with specified properties.
            wrapperObject.reference = page;
            return wrapperObject;
        }

        function addPage(pageWrapper) {
            stack.push(pageWrapper);
            pageWrapper.parentWrapper = stack.find(pageWrapper.parentPage);
            var targetColumn = MathUtils.clamp(pageWrapper.column, 0, columns - 1);
            // replace page holder's child
            var holder = body.children[targetColumn];
            holder.detachCurrentPage();
            holder.attachPage(pageWrapper)
        }

        function columnForPage(page) {
            var wrapper = stack.find(page);
            return wrapper ? wrapper.column : 0;
        }

        // node is a triplet of {page, column, parentPage}
        function popAndSetPageForColumn(node) {
            stack.pop();
            var effectiveColumn = MathUtils.clamp(node.column, 0, columns - 1);
            var columnHolder = body.children[effectiveColumn];
            // is the page in a column?
            if (node == columnHolder.pageWrapper) {
                // detach page from column
                columnHolder.detachCurrentPage();
            }
            node.parent = null;
            var prevPage = stack.topForColumn(effectiveColumn, effectiveColumn < columns - 1);
            if (prevPage) {
                columnHolder.attachPage(prevPage);
            }
            if (node.canDestroy) {
                node.destroyObject();
            }
        }

        // relayouts when column count changes
        function relayout() {
            if (body.children.length == columns) return;
            if (body.children.length > columns) {
                // need to remove few columns, the last ones
                while (body.children.length > columns) {
                    var holder = body.children[body.children.length - 1];
                    holder.detachCurrentPage();
                    holder.parent = null;
                    holder.destroy();
                }
            } else {
                var prevColumns = body.children.length;

                // add columns
                for (var i = 0; i < columns - prevColumns; i++) {
                    pageHolderComponent.createObject(body);
                }
            }
            rearrangePages();
        }

        function rearrangePages() {
            for (var column = columns - 1; column >= 0; column--) {
                var holder = body.children[column];
                var pageWrapper = stack.topForColumn(column, column < (columns - 1));
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
        ColumnMetrics {
            fillWidth: column == columns
            minimumWidth: defaultColumnWidth
        }
    }

    // Page holder component, can have only one Page as child at a time, all stacked pages
    // will be parented into hiddenPages
    Component {
        id: pageHolderComponent
        Item {
            id: holder
            objectName: "PageWrapperHolder" + column
            property PageWrapper pageWrapper
            property int column
            property alias config: header.config
            property ColumnMetrics metrics: setDefaultMetrics()

            Layout.fillWidth: metrics.fillWidth
            Layout.fillHeight: true
            Layout.preferredWidth: metrics.maximumWidth > 0 ?
                                       MathUtils.clamp(defaultColumnWidth, metrics.minimumWidth, metrics.maximumWidth) :
                                       defaultColumnWidth
            Layout.minimumWidth: metrics.minimumWidth
            Layout.maximumWidth: metrics.maximumWidth

            // header
            StyledItem {
                id: header
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                }
                implicitHeight: units.gu(8)
                styleName: config ? "PageHeadStyle" : ""

                property PageHeadConfiguration config: null
                property Item contents: null

                property color dividerColor: theme.palette.normal.background
                property color panelColor
            }

            Rectangle {
                id: divider
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }
                width: (column == (columns - 1)) || !pageWrapper ? 0 : units.dp(2)
                color: theme.palette.normal.background
            }

            Item {
                id: holderBody
                objectName: parent.objectName + "Body"
                anchors {
                    fill: parent
//                    topMargin: header.height
                    rightMargin: divider.width
                }
            }

            function attachPage(page) {
                if (!page) return;
                pageWrapper = page;
                pageWrapper.parent = holderBody;
                pageWrapper.pageHolder = holder;
                pageWrapper.active = true;
                if (pageWrapper.object.hasOwnProperty("head")) {
                    header.config = pageWrapper.object.head;
                }
            }
            function detachCurrentPage() {
                if (!pageWrapper) return undefined;
                var wrapper = pageWrapper;
                // remove header
                wrapper.active = false;
                header.config = null;
                pageWrapper = null;
                wrapper.parent = hiddenPages;
                wrapper.pageHolder = null;
                return wrapper;
            }

            function setDefaultMetrics() {
                var result = defaultMetrics.createObject(holder);
                result.column = Qt.binding(function() { return holder.column + 1; });
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
        anchors.fill: parent
        spacing: 0

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
                var metrics = null;
                for (var j = 0; j < columnMetrics.length; j++) {
                    if (columnMetrics[j].column == (i + 1)) {
                        metrics = columnMetrics[j];
                        break;
                    }
                }
                print(holder)
                if (!metrics) {
                    metrics = holder.setDefaultMetrics();
                }
                holder.metrics = metrics;
            }
        }
    }
}
