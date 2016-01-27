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

import QtQuick 2.4
import Ubuntu.Components 1.3

/*!
    \qmlabstract ActionItem
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief A visual representation of an Action. The API of ActionItem is a
        copy of the API of \l Action, with additional properties to define
        visual aspects of the ActionItem.

        If \l action is set, the values of the other properties will by default
        be identical to the \l Action's property values. Setting the other properties
        will override the properties copied from the \l Action.

        See \l ToolbarItems for examples of how to use \l ToolbarButton or other ActionItems
        in a toolbar.
*/
StyledItem {
    id: actionItem

    /*!
      The \l Action associated with this ActionItem. If action is set,
      the values of the Action properties are copied to the values of
      the ActionItem properties.
     */
    property Action action: null

    visible: action ? action.visible : true
    enabled: action ? action.enabled : true

    /*!
      The title of the actionItem.
      Default value: action.text
     */
    property string text: action ? action.text : ""

    /*!
      The image associated with the actionItem.
      Default value: action.iconSource.

      This is the URL of any image file
      If both iconSource and iconName are defined, iconName will be ignored.
     */
    property url iconSource: action ? action.iconSource : (iconName ? "image://theme/" + iconName : "")

    /*!
      The icon associated with the actionItem in the suru icon theme.
      Default value: action.iconName.

      \note The complete list of icons available in Ubuntu is not published yet.
            For now please refer to the folders where the icon themes are installed:
            \list
              \li Ubuntu Touch: \l file:/usr/share/icons/suru
              \li Ubuntu Desktop: \l file:/usr/share/icons/ubuntu-mono-dark
            \endlist
            These 2 separate icon themes will be merged soon.

      If both iconSource and iconName are defined, iconName will be ignored.
     */
    property string iconName: action ? action.iconName : ""

    /*!
      Called when the actionItem is triggered.
     */
    signal triggered(var value)

    /*!
      If \l action is set, this will trigger it.
    */
    onTriggered: if (action) action.trigger(value)

    /*!
      Trigger this action item if it is enabled.
     */
    function trigger(value) {
        var passingValue = value ? value : null
        if (actionItem.enabled) actionItem.triggered(passingValue);
    }
}
