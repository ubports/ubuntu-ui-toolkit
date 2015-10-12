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
import Ubuntu.Components 1.3

/*!
    \qmltype PageHeader
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \brief The PageHeader shows a title with a leading and a trailing
        \l ActionBar that add action buttons to the header.
*/
Header {
    id: header
    anchors {
        left: parent.left
        right: parent.right
    }

    /*!
      The title to display in the header.
      Note that the title will be hidden if the \l contents Item is set.
     */
    property string title

    /*!
      The contents item to display in the header. By default the contents
      is undefined, and setting it will disable showing of the title in
      the header.

      Example:
      \qml
      PageHeader {
          id: header
          title: "Welcome"
          contents: Rectangle {
              anchors.fill: parent
              color: UbuntuColors.red
              Label {
                  anchors.centerIn: parent
                  text: header.title
                  color: "white"
              }
          }
      }
      \endqml
     */
    property Item contents

    Component.onCompleted: holder.updateContents()
    onContentsChanged: holder.updateContents()

    Item {
        id: holder
        anchors {
            left: leading.right
            right: trailing.left
            top: parent.top
            leftMargin: leading.width > 0 ? 0 : units.gu(1)
        }
        height: __styleInstance.contentHeight
        Loader {
            id: titleLoader
            anchors.fill: parent
        }
        property Item previousContents: null
        property Item previousContentsParent: null

        function updateContents() {
            if (holder.previousContents) {
                holder.previousContents.parent = holder.previousContentsParent;
            }
            if (contents) {
                titleLoader.sourceComponent = null;
                holder.previousContents = header.contents;
                holder.previousContentsParent = header.contents.parent;
                header.contents.parent = holder;
            } else {
                holder.previousContents = null;
                holder.previousContentsParent = null;
                titleLoader.sourceComponent = __styleInstance.titleComponent;
            }
        }
    }

    /*!
      \qmlproperty ActionBar leadingActionBar
      The \l ActionBar for the leading navigation actions.
      Example:
      \qml
      PageHeader {
          leadingActionBar.actions: [
              Action {
                  iconName: "back"
                  text: "Back"
              }
          ]
      }
      \endqml
      See \l ActionBar.
     */
    readonly property alias leadingActionBar: leading
    ActionBar {
        id: leading
        anchors {
            left: parent.left
            top: parent.top
        }
        height: header.__styleInstance.contentHeight
        numberOfSlots: 1
        delegate: header.__styleInstance.defaultActionDelegate
    }

    /*!
      \qmlproperty ActionBar trailingActionBar
      The \l ActionBar with trailing actions.
      Example:
      \qml
      PageHeader {
          trailingActionBar {
              actions: [
                  Action {
                      iconName: "settings"
                      text: "first"
                  },
                  Action {
                      iconName: "info"
                      text: "second"
                  },
                  Action {
                      iconName: "search"
                      text: "third"
                  }
             ]
             numberOfSlots: 2
          }
      }
      \endqml
      See \l ActionBar.
      */
    readonly property alias trailingActionBar: trailing
    ActionBar {
        id: trailing
        anchors {
            right: parent.right
            top: parent.top
        }
        height: header.__styleInstance.contentHeight
        numberOfSlots: 3
        delegate: header.__styleInstance.defaultActionDelegate
    }

    /*!
      \qmlproperty Sections sections
      Sections shown at the bottom of the header. By default,
      the sections will only be visible if its actions or model
      is set. See \l Sections.
     */
    readonly property alias sections: sectionsItem
    Sections {
        id: sectionsItem
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
            top: holder.bottom
        }
        visible: model && model.length > 0
        height: visible ? implicitHeight : 0
    }

    theme.version: Ubuntu.toolkitVersion
    styleName: "PageHeaderStyle"
}
