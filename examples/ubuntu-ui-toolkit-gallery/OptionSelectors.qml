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
import Ubuntu.Components 0.1

Template {
    TemplateSection {
        title: i18n.tr("Option Selector")
        className: "Option Selector"

        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: units.gu(3)

           OptionSelector {
               text: i18n.tr("Label")
               model: [i18n.tr("Value 1"),
                       i18n.tr("Value 2"),
                       i18n.tr("Value 3"),
                       i18n.tr("Value 4")]
           }

           OptionSelector {
                text: i18n.tr("Label")
                expanded: true
                model: [i18n.tr("Value 1"),
                         i18n.tr("Value 2"),
                         i18n.tr("Value 3"),
                         i18n.tr("Value 4")]
           }

           OptionSelector {
               text: i18n.tr("Label")
               model: customModel
               expanded: true
               colourImage: true
               delegate: selectorDelegate
           }

           Component {
               id: selectorDelegate
               OptionSelectorDelegate { text: name; subText: description; icon: image }
           }

           ListModel {
               id: customModel
               ListElement { name: "Name 1"; description: "Description 1"; image: "images.png" }
               ListElement { name: "Name 2"; description: "Description 2"; image: "images.png" }
               ListElement { name: "Name 3"; description: "Description 3"; image: "images.png" }
               ListElement { name: "Name 4"; description: "Description 4"; image: "images.png" }
           }
        }
    }
}
