/*
 * Copyright 2012-2014 Canonical Ltd.
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
import Ubuntu.Unity.Action 1.1 as UnityActions

/*!
  \internal
  Documentation is in Page.qdoc
*/
PageTreeNode {
    id: page
    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
        bottom: parent ? parent.bottom : undefined
    }
    // avoid using parent.height because parent may be a Loader which does not have its height set.
    height: parentNode ? page.flickable ? parentNode.height : parentNode.height - internal.headerHeight : undefined

    isLeaf: true

    property string title: parentNode && parentNode.hasOwnProperty("title") ? parentNode.title : ""

    // deprecated
    property Item tools: toolsLoader.item

    Loader {
        id: toolsLoader
        source: internal.header && internal.header.useDeprecatedToolbar ? "ToolbarItems.qml" : ""
        asynchronous: true
    }

    /*!
      \internal
      \deprecated
      Set this property to replace the title label in the header by any Item.
      It will be automatically anchored to fill the title space in the header.
     */
    property Item __customHeaderContents: null

    property Flickable flickable: internal.getFlickableChild(page)

    /*! \internal */
    onActiveChanged: {
        internal.updateActions();
    }

    /*!
      \qmlproperty list<Action> actions
     */
    property alias actions: actionContext.actions

    Object {
        id: internal

        UnityActions.ActionContext {
            id: actionContext

            property var actionManager: page.__propagated &&
                                        page.__propagated.hasOwnProperty("actionManager") ?
                                            page.__propagated.actionManager : null

            onActionManagerChanged: addLocalContext(actionManager)
            Component.onCompleted: addLocalContext(actionManager)

            function addLocalContext(manager) {
                if (manager) manager.addLocalContext(actionContext);
            }
        }

        function updateActions() {
            actionContext.active = page.active;
        }

        property AppHeader header: page.__propagated && page.__propagated.header ? page.__propagated.header : null
        // Used to position the Page when there is no flickable.
        // When there is a flickable, the header will automatically position it.
        property real headerHeight: internal.header && internal.header.visible ? internal.header.height : 0

        Binding {
            target: tools
            property: "pageStack"
            value: page.pageStack
            when: tools && tools.hasOwnProperty("pageStack")
        }
        Binding {
            target: tools
            property: "visible"
            value: false
            when: internal.header && !internal.header.useDeprecatedToolbar &&
                  page.tools !== null
        }

        function isVerticalFlickable(object) {
            if (object && object.hasOwnProperty("flickableDirection") && object.hasOwnProperty("contentHeight")) {
                var direction = object.flickableDirection;
                if ( ((direction === Flickable.AutoFlickDirection) && (object.contentHeight !== object.height) )
                        || direction === Flickable.VerticalFlick
                        || direction === Flickable.HorizontalAndVerticalFlick) {
                    return true;
                }
            }
            return false;
        }

        /*!
          Return the first flickable child of this page.
         */
        function getFlickableChild(item) {
            if (item && item.hasOwnProperty("children")) {
                for (var i=0; i < item.children.length; i++) {
                    var child = item.children[i];
                    if (internal.isVerticalFlickable(child)) {
                        if (child.anchors.top === page.top || child.anchors.fill === page) {
                            return item.children[i];
                        }
                    }
                }
            }
            return null;
        }
    }
}
