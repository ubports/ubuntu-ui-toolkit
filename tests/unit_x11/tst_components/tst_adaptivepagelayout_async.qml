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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

MainView {
    width: units.gu(90)
    height: units.gu(70)

    Component {
        id: page2Component
        Page {
            title: "Second Page"
            Button {
                text: "Close me"
                onClicked: pageStack.removePages(pageStack.primaryPage);
            }
        }
    }

    AdaptivePageLayout {
        id: pageLayout
        anchors.fill: parent
        primaryPage: Page {
            title: "Primary Page"
            ListView {
                id: listView
                anchors.fill: parent
                model: 10
                delegate: ListItem {
                    Label { text: modelData }
                    onClicked: {
                        var incubator = pageLayout.addPageToNextColumn(pageLayout.primaryPage, page2Component);
                        print(incubator, incubator.status)
                        if (incubator && incubator.status == Component.Loading) {
                            incubator.onStatusChanged = function(status) {
                                print("AHHA", status)
                                if (status == Component.Ready) {
                                    // connect page's destruction to decrement model
                                    incubator.object.Component.destruction.connect(function() {
                                        listView.model--;
                                    });
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
