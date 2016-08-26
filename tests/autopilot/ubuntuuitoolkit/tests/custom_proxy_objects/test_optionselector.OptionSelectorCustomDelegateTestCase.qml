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

import QtQuick 2.0
import Ubuntu.Components 1.3

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Page{

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.fill: parent

            spacing: units.gu(3)

            Component {
                id: valueSelectorDelegate
                OptionSelectorDelegate {
                    text: label
                    objectName: name
                }
            }

            ListModel {
                id: valueModel
                ListElement { name: "one"; label: "Value 1" }
                ListElement { name: "two"; label: "Value 2" }
                ListElement { name: "three"; label: "Value 3" }
                ListElement { name: "four"; label: "Value 4" }
                ListElement { name: "five"; label: "Value 5" }
            }

            OptionSelector {
                id: valueSelector
                objectName: "test_option_selector_collapsed"
                text: "Collapsed"
                delegate: valueSelectorDelegate
                model: valueModel
            }
        }
    }
}
