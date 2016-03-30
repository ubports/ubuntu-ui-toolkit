/*
 * Copyright 2016 Canonical Ltd.
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
 *
 * Author: Andrea Bernabei <andrea.bernabei@canonical.com>
 */

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Styles 1.3
import QtQml.Models 2.1

Item {
    id: main
    width: units.gu(50)
    height: units.gu(100)

    Component {
        id: flickableComp
        Item {
            id: item
            width: units.gu(20)
            height: units.gu(30)
            property alias flickable: freshFlickable
            property alias scrollbar: freshScrollbar
            property alias content: content
            property alias scrollbarAlignment: freshScrollbar.align

            //Don't change this to a ListView, this usecase has to be simple,
            //we don't want the difficulties added by ListView, i.e. delegates
            //size estimation, dynamic contentWidth/Height, etc
            Flickable {
                id: freshFlickable
                anchors.fill: parent
                contentHeight: content.height
                contentWidth: content.width
                clip: true
                Rectangle {
                    id: content
                    width: units.gu(40)
                    //make this much taller than the flickable to avoid timing issues when
                    //detecting if the flickable is flicking in the tests
                    height: units.gu(200)
                    color: "blue"
                }
            }
            Scrollbar {
                id: freshScrollbar
                flickableItem: parent.flickable
            }
        }
    }

    SignalSpy {
        id: signalSpy
    }
    SignalSpy {
        id: anotherSignalSpy
    }

    Flickable {
        anchors.fill: parent
        Column {
            id: column
            Repeater {
                model: layoutsModel
            }
        }
    }

    VisualItemModel {
        id: layoutsModel
        Item {
            width: childrenRect.width
            height: childrenRect.height
            Flickable {
                id: randomFlickable
            }

            //complex PageHeader example taken from the PageHeader tutorial
            //https://developer.ubuntu.com/en/blog/2016/02/24/pageheader-tutorial/
            MainView {
                id: mainView_movingHeaderTest
                width: units.gu(50)
                height: units.gu(80)
                clip: true

                property alias page: pageItem
                property alias standardHeader: standardHeaderItem
                property alias editHeader: editHeaderItem

                Page {
                    id: pageItem
                    header: standardHeaderItem

                    Flickable {
                        id: flickable_movingHeaderTest
                        anchors.fill: parent
                        //just make sure the scrollbar is scrollable
                        contentHeight: mainView_movingHeaderTest.height * 2
                        contentWidth: mainView_movingHeaderTest.width * 2
                        Label {
                            text: "Use the icons in the header."
                            visible: standardHeaderItem.visible
                        }
                    }
                    Scrollbar {
                        id: scrollbar_movingHeaderTest
                        flickableItem: flickable_movingHeaderTest
                    }

                    PageHeader {
                        id: standardHeaderItem
                        visible: pageItem.header === standardHeaderItem
                        title: "Default title"
                        flickable: flickable_movingHeaderTest
                        trailingActionBar.actions: [
                            Action {
                                iconName: "edit"
                                text: "Edit"
                                onTriggered: pageItem.header = editHeaderItem
                            }
                        ]
                    }
                    PageHeader {
                        id: editHeaderItem
                        visible: pageItem.header === editHeaderItem
                        flickable: flickable_movingHeaderTest
                        property Component delegate: Component {
                            AbstractButton {
                                id: button
                                action: modelData
                                width: label.width + units.gu(4)
                                height: parent.height
                                Rectangle {
                                    color: UbuntuColors.slate
                                    opacity: 0.1
                                    anchors.fill: parent
                                    visible: button.pressed
                                }
                                Label {
                                    anchors.centerIn: parent
                                    id: label
                                    text: action.text
                                    font.weight: text === "Confirm"
                                                 ? Font.Normal
                                                 : Font.Light
                                }
                            }
                        }
                        leadingActionBar {
                            anchors.leftMargin: 0
                            actions: Action {
                                text: "Cancel"
                                iconName: "close"
                                onTriggered: pageItem.header = standardHeaderItem
                            }
                            delegate: editHeaderItem.delegate
                        }
                        trailingActionBar {
                            anchors.rightMargin: 0
                            actions: Action {
                                text: "Confirm"
                                iconName: "tick"
                                onTriggered: pageItem.header = standardHeaderItem
                            }
                            delegate: editHeaderItem.delegate
                        }
                        extension: Toolbar {
                            anchors {
                                left: parent.left
                                right: parent.right
                                bottom: parent.bottom
                            }
                            trailingActionBar.actions: [
                                Action { iconName: "bookmark-new" },
                                Action { iconName: "add" },
                                Action { iconName: "edit-select-all" },
                                Action { iconName: "edit-copy" },
                                Action { iconName: "select" }
                            ]
                            leadingActionBar.actions: Action {
                                iconName: "delete"
                                text: "delete"
                                onTriggered: print("Delete action triggered")
                            }
                        }
                    }

                }
            }

        }
    }

    ScrollbarTestCase {
        name: "Scrollbar"

        function getMovingHeaderView() {
            var wrapper = movingHeaderHandlingItem.createObject(column)
            verify(wrapper !== null, "Error: dynamic item creation failed.")
            currComponent = wrapper
            return currComponent
        }

        function cleanup() {
            if (currComponent) {
                currComponent.destroy()
                currComponent = null
            }
            gc()
        }

        function checkScrollbarPositionRelativeToPage(scrollbar, page, expectedY, msgPrefix) {
            compare(scrollbar.mapToItem(page).y, expectedY, msgPrefix + ": Scrollbar does not start below the header.")
        }

        function test_handlingOfMovingHeader_data() {
            return [
                        { tag: "Standard header", header: standardHeaderItem },
                        { tag: "Edit header (with extensions)", header: editHeaderItem },
                        //Setting header to null means using the old header, and:
                        //- We don't support this behaviour when using the old header
                        //- There is no API to get the height of the old header, so we can't test it
                        //{ tag: "Null header", header: null }
                    ]
        }
        function test_handlingOfMovingHeader(data) {
            var page = pageItem
            var header = data.header

            page.header = header
            console.log(page.header, header.flickable)

            //make sure the currently tested header is the one driving the flickable changes
            //FIXME: this should not be needed after #1560458 is fixed
            header.flickable = null
            header.flickable = flickable_movingHeaderTest

            compare(page.header, header, "Handling of moving header: wrong header.")
            compare(scrollbar_movingHeaderTest.__styleInstance.isVertical, true, "Scrollbar is not vertical.")
            compare(scrollbar_movingHeaderTest.__styleInstance.isScrollable, true, "Scrollbar is not scrollable.")

            //don't do the rest of the checks on the null header, just check that it's aligning with the old header implementation
            //(page.head.contents.height)
            if (data.header === null) {
                checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.head.contents.height, data.tag)
                return
            } else {
                header.flickable = flickable_movingHeaderTest
                compare(header.flickable, flickable_movingHeaderTest, "Wrong PageHeader flickable.")
                checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag)
            }

            var tmpHeaderHeight = header.height
            page.header.height += units.gu(1)
            //make sure the header actually has a different height now
            verify(scrollbar_movingHeaderTest.height !== tmpHeaderHeight, "Header height changed value.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height,
                                                 data.tag + ", after changing height")

            //now link the header to a null flickable and check that scrollbar is covered
            //the header (yes, we're checking that it creates bad UX, because we're expecting the
            //developer to set the anchors accordingly so that we don't need to add  inter-components
            //dependencies. The test Page does not handle this on purpose, so the header
            //should cover the scrollbar)
            header.flickable = null
            compare(header.flickable, null, "Wrong PageHeader flickable.")
            expectFailContinue("Standard header", "Waiting on Header bug #1560458")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, 0, data.tag)

            //reassign the correct flickable and check again
            header.flickable = flickable_movingHeaderTest
            compare(header.flickable, flickable_movingHeaderTest, "Wrong PageHeader flickable.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag + ", scrollbar")

            header.visible = false
            compare(header.visible, false, "Header visibility did not change, should have been false.")
            expectFailContinue("", "Waiting on Header bug #1560458")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, 0, data.tag + ", invisible header")

            header.visible = true
            compare(header.visible, true, "Header visibility did not change, should have been true.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag + ", visible header")

            //even if the header has opacity 0, we should still take it into account. This is a standard in
            //QtQuick, you don't ignore a component just because opacity is 0, as that is also used for animations
            header.opacity = 0.0
            compare(header.opacity, 0.0, "Header opacity did not change, should have been 0.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag + ", 0.0 header opacity")

            header.opacity = 1.0
            compare(header.opacity, 1.0, "Header opacity did not change, should have been 1.")
            checkScrollbarPositionRelativeToPage(scrollbar_movingHeaderTest, page, page.header.height, data.tag + ", 1.0 header opacity")
        }
    }
}
