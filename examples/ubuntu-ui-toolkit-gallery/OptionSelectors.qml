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
import Ubuntu.Components 1.3

Template {
    objectName: "optionSelectorsTemplate"

    TemplateSection {
        title: i18n.tr("Option Selector")
        className: "OptionSelector"

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: units.gu(3)

            property var model: [
                i18n.tr("Value 1"),
                i18n.tr("This is the second value"),
                i18n.tr("Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat."),
                i18n.tr("Once upon a time there was a story nobody told."),
            ]

            OptionSelector {
                objectName: "optionselector_collapsed"
                text: i18n.tr("Collapsed")
                model: parent.model
            }

            OptionSelector {
                objectName: "optionselector_expanded"
                text: i18n.tr("Expanded")
                expanded: true
                model: parent.model
            }

            OptionSelector {
                objectName: "optionselector_multipleselection"
                text: i18n.tr("Multiple Selection")
                expanded: false
                multiSelection: true
                model: parent.model
            }

            OptionSelector {
               objectName: "optionselector_custommodel"
               text: i18n.tr("Custom Model")
                model: customModel
                expanded: true
                colourImage: true
                delegate: selectorDelegate
            }

            Component {
                id: selectorDelegate
                OptionSelectorDelegate { text: name; subText: description; iconSource: image }
            }

            ListModel {
                id: customModel
                ListElement { name: "Name 1"; description: "Description 1"; image: "images.png" }
                ListElement { name: "Name 2"; description: "Description 2"; image: "images.png" }
                ListElement { name: "Name 3"; description: "Description 3"; image: "images.png" }
                ListElement { name: "Name 4"; description: "Description 4"; image: "images.png" }
            }

            OptionSelector {
                text: i18n.tr("Custom container height")
                model: [i18n.tr("Value 1"),
                        i18n.tr("Value 2"),
                        i18n.tr("Value 3"),
                        i18n.tr("Value 4"),
                        i18n.tr("Value 5"),
                        i18n.tr("Value 6"),
                        i18n.tr("Value 7"),
                        i18n.tr("Value 8")]
                containerHeight: itemHeight * 4
            }

            OptionSelector {
                text: i18n.tr("No initial selection, scrollable.")
                expanded: true
                selectedIndex: -1
                model: [i18n.tr("Value 1"),
                        i18n.tr("Value 2"),
                        i18n.tr("Value 3"),
                        i18n.tr("Value 4"),
                        i18n.tr("Value 5"),
                        i18n.tr("Value 6"),
                        i18n.tr("Value 7"),
                        i18n.tr("Value 8")]
                containerHeight: itemHeight * 4
            }
        }
    }
}
