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


MainView {

    width: units.gu(40)
    height: units.gu(71)

    Tabs {
        id: tabsWithRepeater
        ListModel {
            id: inputModel
            Component.onCompleted: {
                append({ "name": "tab 1" });
                insert(0, { "name": "tab 0" });
                append({ "name": "tab 3" });
                insert(2, { "name": "tab 2" });
            }
        }
        Repeater {
            id: tabsRepeater
            model: inputModel
            Tab {
                title: name
                page: Page {
                    Label {
                        anchors.centerIn: parent
                        text: title
                        fontSize: "large"
                    }
                    tools: ToolbarItems {
                        ToolbarButton {
                            text: "shufle"
                            onTriggered: {
                                inputModel.move(1, 2, 1);
//                                inputModel.move(3, 0, 1);
//                                inputModel.move(1, 3, 1);
                            }
                        }
                        ToolbarButton {
                            text: "reset"
                            onTriggered: tabsRepeater.model = null;
                        }
                    }
                }
            }
        }
    }
}
