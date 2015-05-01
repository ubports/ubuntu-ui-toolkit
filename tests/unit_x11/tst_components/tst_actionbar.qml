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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    id: root
    width: 400
    height: 600

    property list<Action> actionList:  [
        Action {
            iconName: "alarm-clock"
            text: "Tick tock"
        },
        Action {
            iconName: "appointment"
            text: "Appointment"
        },
        Action {
            iconName: "attachment"
            text: "Attach"
        },
        Action {
            iconName: "contact"
            text: "Contact"
        }
    ]

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: childrenRect.height

        Repeater {
            model: 7
            Item {
                width: parent.width
                height: childrenRect.height

                Label {
                    anchors {
                        left: parent.left
                        verticalCenter: bar.verticalCenter
                    }

                    text: "" + index + " slot(s):"
                }

                ActionBar {
                    id: bar
                    anchors.right: parent.right
                    numberOfSlots: index
                    actions: root.actionList
                }
            }
        }

        //        ActionBar {
        ////            anchors {
        ////                right: parent.right
        ////            }
        //            numberOfSlots: 3
        //            actions: root.actionList
        //        }
    }

    //    UbuntuTestCase {
    //        id: testCase
    //        name: "ActionBarApi"
    //        when: windowShown

    //        function init() {
    //        }
    //    }
}
