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
    objectName: "actionBarTemplate"

    property list<Action> actionList:  [
        Action {
            iconName: "alarm-clock"
            text: "Tick tock"
        },
        Action {
            iconName: "appointment"
            text: "Date"
        },
        Action {
            iconName: "attachment"
            text: "Attach"
        },
        Action {
            iconName: "contact"
            text: "Contact"
        },
        Action {
            iconName: "like"
            text: "Like"
        },
        Action {
            iconName: "lock"
            text: "Lock"
        }
    ]

    property list<Action> shortActionList: [
        Action {
            iconName: "share"
            text: "Share"
        },
        Action {
            iconName: "starred"
            text: "Favorite"
        }
    ]

    head.actions: actionList

    TemplateSection {
        title: "ActionBar"
        className: "ActionBar"

        TemplateRow {
            title: i18n.tr("Short list")
            titleWidth: units.gu(15)

            ActionBar {
                // no numberOfSlots specified. Using default value.
                id: shortBar
                actions: shortActionList
            }
        }

        TemplateRow {
            title: i18n.tr("Number of slots")
            titleWidth: units.gu(15)
            Slider {
                width: units.gu(20)
                id: numberOfActionsSlider
                minimumValue: 1
                maximumValue: 8
                value: 3
                live: true
            }
        }

        TemplateRow {
            title: i18n.tr("Long list")
            titleWidth: units.gu(15)
            ActionBar {
                id: bar
                numberOfSlots: numberOfActionsSlider.value.toFixed(0)
                actions: actionList
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")
            titleWidth: units.gu(15)
            ActionBar {
                enabled: false
                id: disabledBar
                numberOfSlots: numberOfActionsSlider.value.toFixed(0)
                actions: actionList
            }
        }
    }
}
