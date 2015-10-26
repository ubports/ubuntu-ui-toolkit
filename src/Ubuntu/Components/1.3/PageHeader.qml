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

    The colors for foreground, background and the divider are configured
    in the style, so they may be set using \l StyleHints:
    \qml
        PageHeader {
            title: "Colors"
            StyleHints {
                foregroundColor: UbuntuColors.orange
                backgroundColor: "black"
                dividerColor: UbuntuColors.darkGrey
            }
        }
    \endqml
*/
Header {
    id: header
    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
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
            leftMargin: leading.visible ? 0 : units.gu(1)
            rightMargin: trailing.visible ? 0 : units.gu(1)
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

        // When the style changes, make sure that the titleLoader loads
        //  the new titleComponent.
        property Item styleInstance: __styleInstance
        onStyleInstanceChanged: updateContents()
    }

    /*!
      The actions to be shown in the leading action bar.
      This property is automatically set by the
      \l AdaptivePageLayout and other navigation components to configure the
      back action for the \l Page.
      Application developers should not set this property, because the
      value may be overridden by Ubuntu components that have navigation.
      Instead, set \l leadingActionBar's actions property.
     */
    property list<Action> navigationActions

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
      The default value of \l leadingActionBar actions is
      \l navigationActions, but that value can be changed to show
      different actions in front of the title.
      See \l ActionBar.
     */
    readonly property alias leadingActionBar: leading
    ActionBar {
        id: leading
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: units.gu(1)
        }
        height: header.__styleInstance.contentHeight
        numberOfSlots: 1
        delegate: header.__styleInstance.defaultActionDelegate
        actions: header.navigationActions
        visible: leading.width > 0 // at least 1 visible action
        StyleHints {
            overflowIconName: "navigation-menu"
        }
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
            rightMargin: units.gu(1)
        }
        height: header.__styleInstance.contentHeight
        numberOfSlots: 3
        delegate: header.__styleInstance.defaultActionDelegate
        visible: trailing.width > 0 // at least 1 visible action
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

    styleName: "PageHeaderStyle"
}
