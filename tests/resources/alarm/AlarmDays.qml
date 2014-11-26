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
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0

Dialog {
    id:root

    property var alarm

    title: "Choose days"

    ListModel {
        id: daysModel
        ListElement {
            day: "Monday"
            flag: Alarm.Monday
        }
        ListElement {
            day: "Tuesday"
            flag: Alarm.Tuesday
        }
        ListElement {
            day: "Wednesday"
            flag: Alarm.Wednesday
        }
        ListElement {
            day: "Thursday"
            flag: Alarm.Thursday
        }
        ListElement {
            day: "Friday"
            flag: Alarm.Friday
        }
        ListElement {
            day: "Saturday"
            flag: Alarm.Saturday
        }
        ListElement {
            day: "Sunday"
            flag: Alarm.Sunday
        }
    }

    Repeater {
        model: daysModel
        Standard {
            text: day
            control: CheckBox {
                checked: (alarm.daysOfWeek & flag) == flag
                onCheckedChanged: {
                    if (checked) {
                        alarm.daysOfWeek |= flag;
                    } else {
                        alarm.daysOfWeek &= ~flag;
                    }
                }
            }
        }
    }
    Button {
        text: "Done"
        onClicked: PopupUtils.close(root);
    }
}
