/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Components 1.1

// documentation in UbuntuListView11.qdoc
UbuntuListView {
    id: listView
    /*!
      \internal
      \qmlproperty PullToRefresh pullToRefresh
      */
    property alias pullToRefresh: refreshItem

    /*!
      \internal
      \qmlproperty ListItemOptions leadingOptions
      Use binding so we can set it to each ListItem as binding!
      */
    property ListItemOptions leadingOptions: stockLeadingOption
    ListItemOptions {
        id: stockLeadingOption
        options: Action {
            iconName: "delete"
            onTriggered: {
                // delete the index from model if possible
                if (Object.prototype.toString.call(listView.model) === "[object Number]") {
                    // the model is a number, decrement it
                    listView.model = listView.count - 1;
                } else if (Object.prototype.toString.call(listView.model) === "[object Array]") {
                    // the model is an array, remove the item from index
                    var array = listView.model;
                    array.splice(value, 1);
                    listView.model = array;
                } else {
                    // we can only have an object
                    if (listView.model.hasOwnProperty("remove")) {
                        listView.model.remove(value, 1);
                    }
                }
            }
        }
    }

    PullToRefresh {
        objectName: "listview_pulltorefresh"
        id: refreshItem
        enabled: false
    }
}
