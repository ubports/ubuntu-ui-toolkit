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

Page {
    id: mainPage
    title: "Ubuntu UI Toolkit"

    header: PageHeader {
        title: mainPage.title
        flickable: layout.columns === 1 ? widgetList : null
        trailingActionBar.actions: [
            Action {
                text: i18n.tr('Right to Left')
                iconName: 'flash-on'
                visible: !gallery.rtl
                onTriggered: gallery.rtl = !gallery.rtl
            },
            Action {
                text: i18n.tr('Left to Right')
                iconName: 'flash-off'
                visible: gallery.rtl
                onTriggered: gallery.rtl = !gallery.rtl
            },
            Action {
                text: i18n.tr('Use dark theme')
                iconName: 'torch-on'
                visible: gallery.theme.name == 'Ubuntu.Components.Themes.Ambiance'
                onTriggered: gallery.theme.name = 'Ubuntu.Components.Themes.SuruDark'
            },
            Action {
                text: i18n.tr('Use light theme')
                iconName: 'torch-off'
                visible: gallery.theme.name == 'Ubuntu.Components.Themes.SuruDark'
                onTriggered: gallery.theme.name = 'Ubuntu.Components.Themes.Ambiance'
            },
            Action {
                id: aboutAction
                text: i18n.tr('About')
                iconName: "info"
                onTriggered: mainPage.pageStack.addPageToCurrentColumn(mainPage, Qt.resolvedUrl("About.qml"))
            },
            Action {
                text: i18n.tr("Deactivate mouse")
                iconName: "non-starred"
                visible: QuickUtils.mouseAttached
                onTriggered: QuickUtils.mouseAttached = false
            },
            Action {
                text: i18n.tr("Activate mouse")
                iconName: "starred"
                visible: !QuickUtils.mouseAttached
                onTriggered: QuickUtils.mouseAttached = true
            },
            Action {
                text: i18n.tr("Detach keyboard")
                iconName: "non-starred"
                visible: QuickUtils.keyboardAttached
                onTriggered: QuickUtils.keyboardAttached = false
            },
            Action {
                text: i18n.tr("Attach keyboard")
                iconName: "starred"
                visible: !QuickUtils.keyboardAttached
                onTriggered: QuickUtils.keyboardAttached = true
            }
        ]
    }

    onActiveChanged: {
        if (layout.columns < 2) {
            widgetList.currentIndex = -1;
        }
        if (active) {
            widgetList.openPage();
        }
    }

    UbuntuListView {
        id: widgetList
        objectName: "widgetList"
        anchors {
            fill: parent
            topMargin: mainPage.header.flickable ? 0 : mainPage.header.height
        }

        model: WidgetsModel {}
        currentIndex: -1

        onCurrentIndexChanged: openPage()

        function openPage() {
            if (!mainPage.active || currentIndex < 0) return;
            var modelData = model.get(currentIndex);
            var source = Qt.resolvedUrl(modelData.source);
            mainPage.pageStack.addPageToNextColumn(mainPage, source, {title: modelData.label});
        }

        delegate: ListItem {
            objectName: model.objectName
            enabled: source != ""
            // Used by Autopilot
            property string text: label
            onClicked: widgetList.currentIndex = index
            //follow ListItemLayout size
            height: layout.height + (divider.visible ? divider.height : 0)
            ListItemLayout {
                id: layout
                title.text: label
                ProgressionSlot {}
            }
        }
    }
}
