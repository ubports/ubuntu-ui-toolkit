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

import QtQuick 1.1
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1
Template {
    title: "Drilldown"

//    Flickable {
//        anchors {
//            top: parent.top
//            bottom: parent.bottom
////            horizontalCenter: parent.horizontalCenter
//        }
//        width: 500

//        contentWidth: 500
//        contentHeight: theList.childrenRect.height
//        boundsBehavior: Flickable.DragAndOvershootBounds
//        flickableDirection: Flickable.VerticalFlick
//        clip: true

        PageStack {
            id: pageStack
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }

            width: 100

            Page {
                Component {
                    id: onOffButton
                    Button {
                        property bool isOn: false
                        text: isOn ? "Off" : "On"
                        onClicked: isOn = !isOn
                    }
                }

                title: "Settings"

                contents: Column {
                    anchors.fill: parent

                    Standard {
                        text: "Flight mode"
                        // TODO: replace by switch when that component is available.
                        control: onOffButton.createObject(parent)
                    }
                    Standard {
                        // TODO
                        text: "Internet connection"
                        progression: true
                    }
                    Standard {
                        // TODO
                        text: "Mobile network"
                        progression: true
                    }
                    Standard {
                        // TODO
                        control: Button {
                            anchors.fill: parent
                            text: "Brightness"
                        }
                    }
                    PageList {
                        pageStack: pageStack
                        Page {
                            title: "Device"
                            iconSource: "avatar_contacts_list.png"

                            contents: Column {
                                anchors.fill: parent
                                PageList {
                                    pageStack: pageStack
                                    Page {
                                        title: "Call"
                                    }
                                    Page {
                                        title: "SIM card"
                                    }
                                    Page {
                                        title: "Battery"
                                    }
                                    Page {
                                        title: "Display"
                                    }
                                    Page {
                                        title: "Data storage"
                                    }
                                    Page {
                                        title: "NFC"
                                    }
                                    Page {
                                        title: "GPS and positioning"
                                    }
                                    Page {
                                        title: "Counters"
                                    }
                                    Page {
                                        title: "Media sharing settings"
                                    }
                                }
                            }
                        }
                        Page {
                            title: "Bluetooth"
                            iconSource: "avatar_contacts_list.png"

                            contents: Column {
                                anchors.fill: parent
                                Standard {
                                    text: "Bluetooth"
                                    control: onOffButton.createObject(parent)
                                }
                                Header {
                                    text: "My device"
                                }
                                Subtitled {
                                    progression: true
                                    text: "Name and information"
                                    subText: "My device name"
                                }
                                Standard {
                                    text: "Visibility"
                                    control: onOffButton.createObject(parent)
                                }
                                Divider { }
                                Standard {
                                    text: "Buetooth devices"
                                    progression: true
                                }
                                Standard {
                                    text: "Always show Buetooth in status menu"
                                    control: onOffButton.createObject(parent)
                                }

                            }
                        }
                        Page {
                            title: "Sounds and vibration"
                            iconSource: "avatar_contacts_list.png"

                            contents: Column {
                                anchors.fill: parent
                                Standard {
                                    control: Button { text: "Profile"; anchors.fill: parent }
                                }
                                Standard {
                                    control: Button { text: "Volume"; anchors.fill: parent }
                                }
                                Header { text: "Vibration profile" }
                                Standard {
                                    text: "Ringing"
                                    control: onOffButton.createObject(parent)
                                }
                                Standard {
                                    text: "Beep"
                                    control: onOffButton.createObject(parent)
                                }
                                Standard {
                                    text: "Silent"
                                    control: onOffButton.createObject(parent)
                                }
                                Header {
                                    text: "Alert tones"
                                }
                                Subtitled {
                                    text: "Ringtone"
                                    subText: "Canonical tune"
                                    progression: true
                                }
                                Subtitled {
                                    text: "Internet call alert"
                                    subText: "Canonical tune"
                                    progression: true
                                }
                                Subtitled {
                                    text: "Mail alert tone"
                                    subText: "Email 1"
                                    progression: true
                                }
                                Subtitled {
                                    text: "Message alert tone"
                                    subText: "Message 1"
                                    progression: true
                                }
                            }
                        }
                        Page {
                            title: "Wallpaper"
                            iconSource: "avatar_contacts_list.png"
                        }
                        Page {
                            title: "Time and language"
                            iconSource: "avatar_contacts_list.png"

                            contents: Column {
                                anchors.fill: parent
                                Subtitled {
                                    text: "Time and date"
                                    subText: "28-08-12 12:05"
                                    progression: true
                                }
                                Subtitled {
                                    text: "Language"
                                    subText: "English (United Kingdom)"
                                    progression: true
                                }
                                Subtitled {
                                    text: "Regional format"
                                    subText: "Netherlands"
                                    progression: true
                                }
                                Standard {
                                    text: "Text input"
                                    progression: true
                                }
                            }
                        }
                        Page {
                            title: "Applications"
                            iconSource: "avatar_contacts_list.png"
                        }
                        Page {
                            title: "Notifications"
                            iconSource: "avatar_contacts_list.png"
                        }
                        Page {
                            title: "Security"
                            iconSource: "avatar_contacts_list.png"
                        }
                        Page {
                            title: "Sync and backup"
                            iconSource: "avatar_contacts_list.png"
                        }
                        Page {
                            title: "Accessories"
                            iconSource: "avatar_contacts_list.png"
                        }
                        Page {
                            title: "About product"
                            iconSource: "avatar_contacts_list.png"
                        }
                        Page {
                            title: "Reset"
                            iconSource: "avatar_contacts_list.png"
                        }
                    }
                }
//            }
        }
    }
}
