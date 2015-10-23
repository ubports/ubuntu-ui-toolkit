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
        title: sectionsTemplate.title
        sections.model: ["first", "second", "third"]
    }

    TemplateSection {
        title: "Sections"
        className: "Sections"

        TemplateRow {
            title: i18n.tr("Enabled")

            Sections {
                actions: [
                    Action { text: "one" },
                    Action { text: "two" },
                    Action { text: "three" }
                ]
            }
        }
        TemplateRow {
            title: i18n.tr("Disabled")

            Sections {
                model: ["one", "two", "three"]
                enabled: false
            }
        }
    }
}
