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
import Ubuntu.Components.ListItems 1.0
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.Pickers 1.0

MainView {
    id: mainView
    width: units.gu(40)
    height: units.gu(71)
    objectName: "mainView"
    useDeprecatedToolbar: false

    AlarmModel{
        id: alarmModel
    }

    Alarm {
        id: alarm
        onStatusChanged: {
            print("operation " + operation + ", status= " + status + ", error=" + error);
            if (status !== Alarm.Ready)
                return;
            if ((operation > Alarm.NoOperation) && (operation < Alarm.Reseting)) {
                reset();
            }
        }
    }

    Page {
        title: "Alarm test"
        Column {
            id: layout
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            height: childrenRect.height

            Standard {
                text: "Label"
                control: TextField {
                    id: message
                    objectName: "alarm_message"
                    text: alarm.message
                }
            }

            Row {
                height: units.gu(6)
                spacing: units.gu(1)
                Button {
                    id: dateChooser
                    text: "Date: " + date.toDateString()
                    property date date: new Date(alarm.date)
                    onClicked: PickerPanel.openDatePicker(dateChooser, "date", "Years|Months|Days")
                }
                Button {
                    id: timeChooser
                    text: "Time: " + time.toTimeString()
                    property date time: new Date(alarm.date)
                    onClicked: PickerPanel.openDatePicker(timeChooser, "time", "Hours|Minutes")
                }
            }

            Standard {
                text: "Enabled"
                control: Switch {
                    id: enabled
                    objectName: "alarm_enabled"
                    checked: alarm.enabled
                    onCheckedChanged: {
                        if (checked != alarm.enabled) {
                            alarm.enabled = checked;
                        }
                    }
                }
            }
            ItemSelector {
                id: recurence
                text: "Recurence"
                model: ["OneTime", "Daily", "Weekly"]
                selectedIndex: {
                    if (alarm.type == Alarm.OneTime)
                        return 0;
                    else if (alarm.type == Alarm.Repeating) {
                        if (alarm.daysOfWeek === Alarm.Daily)
                            return 1;
                        else
                            return 2;
                    }
                }
                onSelectedIndexChanged: {
                    switch (selectedIndex) {
                    case 0:
                        alarm.type = Alarm.OneTime;
                        break;
                    case 1:
                        alarm.type = Alarm.Repeating;
                        alarm.daysOfWeek = Alarm.Daily;
                        break;
                    case 2:
                        alarm.type = Alarm.Repeating;
                        alarm.daysOfWeek = Alarm.AutoDetect;
                        break;
                    }
                }
            }

            MultiValue {
                id: days
                text: "Occurrence"
                values: getValues()
                visible: recurence.selectedIndex === 2
                onClicked: {
                    PopupUtils.open(Qt.resolvedUrl("AlarmDays.qml"), days, {"alarm": alarm});
                }
                function getValues() {
                    var v = [];
                    if (alarm.daysOfWeek & Alarm.Monday) v.push("Monday");
                    if (alarm.daysOfWeek & Alarm.Tuesday) v.push("Tuesday");
                    if (alarm.daysOfWeek & Alarm.Wednesday) v.push("Wednesday");
                    if (alarm.daysOfWeek & Alarm.Thursday) v.push("Thursday");
                    if (alarm.daysOfWeek & Alarm.Friday) v.push("Friday");
                    if (alarm.daysOfWeek & Alarm.Saturday) v.push("Saturday");
                    if (alarm.daysOfWeek & Alarm.Sunday) v.push("Sunday");
                    return v;
                }
            }

            Standard {
                text: "Save result="+alarm.error
                control: Button {
                    text: "Save"
                    onClicked: {
                        alarm.message = message.text;
                        var date = new Date();
                        date.setTime(timeChooser.time.getTime());
                        date.setDate(dateChooser.date.getDate());
                        print("DATE", date, dateChooser.date.toDateString())
                        alarm.date = date;
                        alarm.save();
                    }
                }
            }
            Standard {
                text: "Alarm count: " + alarmModel.count
                control: Button {
                    text: "Reset"
                    onClicked: {
                        alarm.reset();
                    }
                }
            }
            ThinDivider{}

            ListView {
                id: alarmList
                width: parent.width
                height: units.gu(20)
                clip: true
                model: alarmModel
                delegate: Standard {
                    text: message + recurring(model) + "\n" + model.date
                    function recurring(alarmData) {
                        return (alarmData.type === Alarm.Repeating) ? "[Repeating]" : "[Onetime]";
                    }

                    removable: true
                    control: Switch {
                        checked: model.enabled
                        onCheckedChanged: {
                            if (checked != model.enabled) {
                                // use a local variable, otherwise "model" role will always fetch the
                                // original data!
                                var alarmData = model;
                                alarmData.enabled = checked;
                                alarmData.save();
                            }
                        }
                    }
                    onItemRemoved: {
                        var data = alarmModel.get(index);
                        data.cancel();
                    }
                    onClicked: {
                        var data = alarmModel.get(index);
                        alarm.message = data.message;
                        alarm.date = data.date;
                        alarm.type = data.type;
                        alarm.daysOfWeek = data.daysOfWeek;
                        alarm.enabled = data.enabled;
                    }

                    Connections {
                        target: model
                        onStatusChanged: print("operation", operation, "status=", model.status, "error=", model.error)
                    }
                }
            }
        }
    }
}
