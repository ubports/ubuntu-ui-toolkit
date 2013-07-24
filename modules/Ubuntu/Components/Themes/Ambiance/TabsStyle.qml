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

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Item {
    // styling properties
    /*!
      If this optional property is specified, it will be positioned
      between the bar with tab buttons, and the tab pages to act
      as a separator.
     */
    property Item separator

    /*!
      Enable left/right swiping in the contents area to go to
      the next/previous tab.
     */
    property bool swipeToSwitchTabs
    /*!
      \deprecated
      \internal
     */
    onSwipeToSwitchTabsChanged: print("swipeToSwitchTabs property is DEPRECATED.")

    property color headerTextColor: Theme.palette.normal.backgroundText
    property color headerTextSelectedColor: Theme.palette.selected.backgroundText
    property int headerTextFadeDuration: 350
    property string headerFontSize: "x-large"
    property int headerFontWeight: Font.Light
    property real headerTextLeftMargin: units.gu(2)
    property real headerTextRightMargin: units.gu(2)
    property real headerTextBottomMargin: units.gu(2)
    property url indicatorImageSource: "artwork/chevron.png"
    property real tabBarHeight: units.gu(7.5)

    /*!
      The time of inactivity in ms before the tab bar is automatically deactivated
     */
    property int deactivateTime: 3000

    /*!
      The velocity at which buttons are repositioned when a new tab gets selected.
     */
    property real buttonPositioningVelocity: 1.0

    // visuals
    id: tabsStyle
    anchors.fill: parent

    property Tabs theTabs: styledItem
//    property Component headerContents: //Component {
    property Component headerContents:
        Rectangle {
            color: "pink"
            anchors.fill: parent
            height: 100
        }

//        TabBarStyle {
//            id: tabBar
//            tabs: theTabs
//            headerTextColor: tabsStyle.headerTextColor
//            headerTextSelectedColor: tabsStyle.headerTextSelectedColor
//            headerTextFadeDuration: tabsStyle.headerTextFadeDuration
//            indicatorImageSource: tabsStyle.indicatorImageSource
//            headerFontSize: tabsStyle.headerFontSize
//            headerFontWeight: tabsStyle.headerFontWeight
//            headerTextLeftMargin: tabsStyle.headerTextLeftMargin
//            headerTextRightMargin: tabsStyle.headerTextRightMargin
//            headerTextBottomMargin: tabsStyle.headerTextBottomMargin
//            buttonPositioningVelocity: tabsStyle.buttonPositioningVelocity
//            deactivateTime: tabsStyle.deactivateTime
//        }
//    }

//    Component.onCompleted: {
//        print("XXX")
//        styledItem.__headerContents = headerContents;
//    }
}
