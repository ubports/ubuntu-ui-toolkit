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
    id: multiColumnView

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
        wrapper.parentPage = sourcePage;
        d.addPage(wrapper);
        return wrapper.object;
    }

    /*!
      \qmlmethod Item addToNextColumn(Item sourcePage, var page[, var properties])
      Same as \l addTotargetColumn except that the \c page is added to the column
      next to the one the \c sourcePage resides. If \c sourcePage is null, the new
      page will be added to the leftmost column. If \c sourcePage is located in the
      rightmost column, the new page will be pushed to the same column as \c sourcePage.
      */
    function addToNextColumn(sourcePage, page, properties) {
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
      \qmlmethod void removePagesUntil(Item page)
      The function removes and deletes all pages from the view columns until \c page
      is reached.
      */
    function removePagesUntil(page) {
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
    Component.onCompleted: d.relayout()

    QtObject {
        id: d

        property int prevColumns: 0
        property var stack: new Stack.Stack()

        function createWrapper(page, properties) {
            var wrapperComponent = Qt.createComponent("PageWrapper.qml");
            var wrapperObject = wrapperComponent.createObject(body);
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
            var prevPage = stack.topForColumn(node.column, effectiveColumn < columns - 1);
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

    // Page holder component, can have only one Page as child at a time, all stacked pages
    // will be parented into hiddenPages
    Component {
        id: pageHolderComponent
        Item {
            id: holder
            property PageWrapper pageWrapper
            property int column
            property alias config: header.config

            objectName: "PageWrapperHolder" + column

            Layout.fillWidth: column == (columns - 1)
            Layout.fillHeight: true
            Layout.preferredWidth: units.gu(40)

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
                width: (column == (columns - 1)) || !pageWrapper ? 0 : units.dp(4)
                color: theme.palette.normal.background
            }

            Item {
                id: holderBody
                objectName: parent.objectName + "Body"
                anchors {
                    fill: parent
                    topMargin: header.height
                    rightMargin: divider.width
                }
            }

            function attachPage(page) {
                if (!page) return;
                pageWrapper = page;
                pageWrapper.parent = holderBody;
                pageWrapper.pageHolder = holder;
                // configure header before we activate
                header.config = pageWrapper.object.head;
                pageWrapper.active = true;
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
        }
    }

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
            for (var i = 0; i <= children.length - 1; i++) {
                children[i].column = i;
            }
        }
    }
}
