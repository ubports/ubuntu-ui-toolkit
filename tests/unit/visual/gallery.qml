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
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Qt.labs.folderlistmodel 2.1

MainView {
    width: units.gu(120)
    height: units.gu(75)

    AdaptivePageLayout {
        id: pageLayout
        anchors.fill: parent
        primaryPage: Page {
            header: PageHeader {
                flickable: testsListView
                title: 'Unit Tests'
            }

            UbuntuListView {
                id: testsListView
                anchors.fill: parent
                model: FolderListModel {
                    Component.onCompleted: folder += '/tests/unit/visual'
                    showDirs: false
                    showOnlyReadable: true
                    sortField: FolderListModel.Name
                    nameFilters: [ "tst_*.13.qml" ]
                }

                delegate: ListItem {
                    height: listItemLayout.height
                    ListItemLayout {
                        id: listItemLayout
                        title.text: fileBaseName
                        ProgressionSlot {}
                    }
                    onClicked: {
                        testsListView.currentIndex = index
                        pageLayout.addPageToNextColumn(pageLayout.primaryPage, testPage, { source: fileURL, title: fileBaseName })
                    }
                }
            }
        }
        asynchronous: false

        Page {
            id: testPage
            property alias source: testLoader.source
            header: PageHeader {
                id: testPageHeader
                title: testPage.title
            }

            Loader {
                id: testLoader
                anchors.fill: parent
                anchors.topMargin: testPage.header.height
                asynchronous: false
            }
        }
    }
}

