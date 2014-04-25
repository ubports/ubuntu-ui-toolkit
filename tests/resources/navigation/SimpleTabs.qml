/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Components 1.1

MainView {
    width: 800
    height: 600
    useDeprecatedToolbar: false

    Tabs {
        id: tabs
        selectedTabIndex: 1

        Tab {
            title: i18n.tr("One")
            page: Page {
                Label {
                    anchors.centerIn: parent
                    text: i18n.tr("First page")
                }
                tools: ToolbarItems {
                    ToolbarButton {
                        text: "1111"
                        iconSource: "call_icon.png"
                    }
                }
            }
        }
        Tab {
            title: i18n.tr("Two")
            page: Page {
                Label {
                    anchors.centerIn: parent
                    text: i18n.tr("Second page")
                }
                tools: ToolbarItems {
                    ToolbarButton {
                        text: "2222"
                        iconSource: "call_icon.png"
                    }
                }
            }
        }
    }
}
