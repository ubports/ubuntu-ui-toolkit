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

Template {
    objectName: "sectionsTemplate"
    id: sectionsTemplate

    header: PageHeader {
        id: h
        title: sectionsTemplate.title
        extension: Sections {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            model: h.showManySections ? sectionsTemplate.manyActions
                                      : sectionsTemplate.fewActions
        }
        property bool showManySections: false
        trailingActionBar.actions: [
            Action {
                iconName: h.showManySections ? "view-collapse" : "view-expand"
                text: h.showManySections ? "less" : "more"
                onTriggered: h.showManySections = !h.showManySections;
            }
        ]
    }

    property list<Action> fewActions: [
        Action { text: "One" },
        Action { text: "Two" },
        Action { text: "Three" }
    ]

    property var manyActions: [
        "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
        "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen",
        "sixteen", "seventeen", "eighteen", "nineteen", "twenty"
    ]

    TemplateSection {
        title: "Sections"
        className: "Sections"

        TemplateRow {
            title: i18n.tr("Enabled")

            Sections {
                actions: sectionsTemplate.fewActions
            }
        }
        TemplateRow {
            title: i18n.tr("Disabled")

            Sections {
                model: sectionsTemplate.fewActions
                enabled: false
            }
        }
    }

    TemplateSection {
        title: "Scrollable sections"
        className: "Sections"

        TemplateRow {
            title: i18n.tr("Enabled")

            Sections {
                model: sectionsTemplate.manyActions
                width: parent.width
            }
        }
        TemplateRow {
            title: i18n.tr("Disabled")

            Sections {
                model: sectionsTemplate.manyActions
                enabled: false
                width: parent.width
            }
        }
    }
}
