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

/*!
  \qmltype PageColumnsLayout
  \inqmlmodule Ubuntu.Components 1.3
  \since Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief Component configuring a layout in an AdaptivePageLayout component.

  The component specifies the column configuration of a specific layout. The layout
  will have as many columns as many PageColumn elements will be declared. The layout
  will be activated when the \l when property evaluates to \c true. There can be
  many layouts evaluated to true, only the first one evaluated to true in the
  \l AdaptivePageLayout::layouts list will be activated.

  \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    MainView {
        width: units.gu(100)
        height: units.gu(60)

        AdaptivePageLayout {
            anchors.fill: parent
            primaryPage: page1
            layouts: [
                PageColumnsLayout {
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
                },
                PageColumnsLayout {
                    when: true
                    PageColumn {
                        fillWidth: true
                        minimumWidth: units.gu(10)
                    }
                }
            ]

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
  In the example above the second PageColumnLayout's condition is always set to
  true, which means that that the layout will be always active unless the first
  layout's condition evaluates to true. The layout overrides the single column
  minimumWidth default value. Note that \l PageColumn::fillWidth must be also set.

  \note When none of the conditions is met, a single column layout will be used.

  \sa PageColumn
  */
QtObject {
    id: layout

    /*!
      Condition activating the layout. Defaults to false.
      */
    property bool when: false

    /*!
      \qmlproperty list<PageColumn> data
      \default
      Default property holding the PageColumn elements configuring each column.
      */
    default property alias data: layout.__data
    property list<PageColumn> __data
}
