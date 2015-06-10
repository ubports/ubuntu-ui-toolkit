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
import "../1.2/stack.js" as Stack

/*!
  \qmltype MultiColumnView
  \inqmlmodule Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief View with multiple columns.
*/

/*
  TODO:
  - add column sizing configuration properties, maybe embedd conditions when these should
    be applied together with the number of columns
  */

PageTreeNode {

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
    property int columns: 0

    /*!
      \qmlmethod Item addToCurrentColumn(Item sourcePage, var page[, var properties])
      Adds a \c page to the column the \c sourcePage resides in. If \c sourcePage
      is null, the \c page will be added to the leftmost column. \c page can be a
      Component or a file. \c properties is a JSon object containing properties
      to be set when page is created. \c sourcePage must be active. Returns the
      instance of the page created.
      */
    function addToCurrentColumn(sourcePage, page, properties) {
        if (columns <= 0) {
            return;
        }
        var wrapper = d.createWrapper(page, properties);
        wrapper.column = d.columnForPage(sourcePage);
        if (wrapper.column <= 0) {
            wrapper.column = 1;
        }
        wrapper.parentPage = sourcePage;
        d.addPage(wrapper);
        return wrapper.object;
    }

    /*!
      \qmlmethod Item addToNextColumn(Item sourcePage, var page[, var properties])
      Same as \l addToCurrentColumn except that the \c page is added to the column
      next to the one the \c sourcePage resides. If \c sourcePage is null, the new
      page will be added to the leftmost column. If \c sourcePage is located in the
      rightmost column, the new page will be pushed to the same column as \c sourcePage.
      */
    function addToNextColumn(sourcePage, page, properties) {
        if (columns <= 0) {
            return;
        }
        var wrapper = d.createWrapper(page, properties);
        wrapper.column = d.columnForPage(sourcePage) + 1;
        if (wrapper.column <= 0) {
            wrapper.column = 1;
        }
        wrapper.parentPage = sourcePage;
        d.addPage(wrapper);
        return wrapper.object;
    }

    /*!
      \qmlmethod void removeUntilPage(Item page)
      The function removes and deletes all pages from the view columns until \c page
      is reached.
      */
    function removeUntilPage(page) {
        while (d.stack.top().parentPage == page) {
            d.setPreviousPageForColumn(d.stack.top());
        }
        while (d.stack.top() == page) {
            d.setPreviousPageForColumn(d.stack.top());
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
    Component.onCompleted: d.relayout()

    QtObject {
        id: d

        property int prevColumns: 0
        property var stack: new Stack.Stack()

        function createWrapper(page, properties) {
            var wrapperComponent = Qt.createComponent("PageWrapper.qml");
            var wrapperObject = wrapperComponent.createObject(body);
            wrapperObject.pageStack = null;
            wrapperObject.properties = properties;
            // set reference last because it will trigger creation of the object
            //  with specified properties.
            wrapperObject.reference = page;
            return wrapperObject;
        }

        function addPage(pageWrapper) {
            stack.push(pageWrapper);
            var column = (pageWrapper.column >= columns) ? columns - 1 : pageWrapper.column;
            if (column >= 0) {
                // replace page holder's child
                if (body.children[column].pageWrapper) {
                    print('body.children[column].children[0]', body.children[column].children[0])
                    body.children[column].pageWrapper.active = false;
                    body.children[column].pageWrapper.parent = hiddenPages;
                }
                body.children[column].pageWrapper = pageWrapper;
                pageWrapper.active = true;
            }
        }

        function columnForPage(page) {
            var wrapper = stack.find(page);
            return wrapper ? wrapper.column : -1;
        }

        // node is a triplet of {page, column, parentPage}
        function setPreviousPageForColumn(node) {
            node.active = false;
            node.parent = null;
            var prevPage = stack.lastPushedPageToColumn(node.column);
            if (prevPage) {
                prevPage.parent = body.children[node.column - 1];
                prevPage.active = true;
            }
            if (node.canDestroy) {
                node.destroyObject();
            }
        }

        // relayouts when column count changes
        function relayout() {
            if (body.children.length > columns) {
                // need to remove few columns, the first ones
                while (body.children.length > columns) {
                    var holder = body.children[0];
                    var pageWrapper = holder.pageWrapper;
                    print(pageWrapper)
                    pageWrapper.parent = hiddenPages;
                    holder.destroy();
                }
            } else {
                while (body.children.length < columns) {
                    var holder = pageHolderComponent.createObject(body);
                    holder.parent = body;
                    holder.column = body.children.length;
                    print(holder)
                    rearrangeColumnPages();
                }
            }
        }

        function rearrangeColumnPages() {
            for (var i = body.children.length - 1; i >= 0; i--) {
                var pageWrapper = stack.topForColumn(i);
                var holder = body.children[i];

                if (!pageWrapper) {
                    return;
                }
                pageWrapper.parent = holder;
            }
        }

    }

    // Page holder component, can have only one Page as child at a time, all stacked pages
    // will be parented into hiddenPages
    Component {
        id: pageHolderComponent
        PageTreeNode {
            id: holder
            property PageWrapper pageWrapper
            property int column

            objectName: "PageWrapperHolder" + column

            Layout.fillHeight: true
            Layout.preferredWidth: units.gu(40)

            Rectangle {
                id: divider
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }
                width: 1
                color: theme.palette.normal.background
            }
            onPageWrapperChanged: {
                if (!pageWrapper) {
                    return;
                }

//                holder.Layout.minimumWidth = Qt.binding(function (){ return pageWrapper.object.Layout.minimumWidth });
//                holder.Layout.maximumWidth = Qt.binding(function (){ return pageWrapper.object.Layout.maximumWidth });
//                holder.Layout.preferredWidth = Qt.binding(function(){ return units.gu(40)});
                pageWrapper.parent = holder;
                pageWrapper.anchors.left = holder.left;
                pageWrapper.anchors.top = holder.top;
                pageWrapper.anchors.bottom = holder.bottom;
                pageWrapper.anchors.right = divider.left;
            }
            onColumnChanged: holder.Layout.fillWidth = (column == columns)
        }
    }

    Item {
        id: hiddenPages
        visible: false
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
            for (var i = 0; i < children.length - 1; i++) {
                children[i].Layout.fillWidth = false;
            }
            if (i >= 0) {
                children[i].Layout.fillWidth = true;
            }
        }
    }
}
