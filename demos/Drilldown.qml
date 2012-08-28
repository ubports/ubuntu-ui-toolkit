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


    Item {
        // wrapper item because the template controls the anchors
        anchors.fill: parent

        PageStack {
            id: stack
            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }

            width: 300

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
                    PageList {
                        pageStack: stack
                        Page {
                            title: "Device"
                            iconSource: "avatar_contacts_list.png"

                            contents: PageList {
                                pageStack: stack
                                Page { title: "Call" }
                                Page { title: "SIM card" }
                                Page { title: "Battery" }
                                Page { title: "Display" }
                                Page { title: "Data storage" }
                                Page { title: "NFC" }
                                Page { title: "GPS and positioning" }
                                Page { title: "Counters" }
                                Page { title: "Media sharing settings" }
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
                            title: "Time and language"
                            iconSource: "avatar_contacts_list.png"

                            contents: Column {
                                anchors.fill: parent
                                Subtitled {
                                    text: "Time and date"
                                    subText: "28-08-12 12:05"
                                    progression: true
                                }
                                SingleValue {
                                    text: "Language"
                                    value: "English (United Kingdom)"
                                    progression: true
                                }
                                SingleValue {
                                    text: "Regional format"
                                    value: "Netherlands"
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

                            contents: Column {
                                anchors.fill: parent
                                Standard {
                                    control: Button {
                                        text: "Manage applications"
                                        anchors.fill: parent
                                    }
                                }
                                PageList {
                                    pageStack: stack
                                    Page {
                                        title: "Installations"
                                        contents: Column {
                                            anchors.fill: parent
                                            Standard {
                                                text: "Allow installations from\nnon-Store sources"
                                                control: onOffButton.createObject(parent)
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Calendar"
                                        contents: Column {
                                            anchors.fill: parent
                                            Header { text: "General" }
                                            ValueSelector {
                                                text: "Week starts on"
                                                values: ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"]
                                            }
                                            MultiValue {
                                                text: "Calendar working week"
                                                values: ["Mon", "Tue", "Wed", "Thu", "Fri"]
                                                progression: true
                                            }
                                            Header { text: "Events" }
                                            SingleValue {
                                                text: "Default reminder displayed"
                                                value: "15 minutes before"
                                                progression: true
                                            }
                                            SingleValue {
                                                text: "Default calendar"
                                                value: "Personal"
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Contacts"
                                        contents: Column {
                                            anchors.fill: parent
                                            ValueSelector {
                                                text: "Prefer avatars from"
                                                values: ["No preference", "Google Talk", "Skype"]
                                            }
                                            ValueSelector {
                                                text: "Show names"
                                                values: ["First name - Last name", "Last name - First name"]
                                            }
                                            Standard {
                                                text: "Prefer nickname"
                                                control: onOffButton.createObject(parent)
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Feeds"
                                        contents: Column {
                                            anchors.fill: parent
                                            ValueSelector {
                                                text: "Show Items"
                                                values: ["All items", "Unread items only"]
                                            }
                                            ValueSelector {
                                                text: "Update feeds"
                                                values: ["Manually only", "Automatically in WLAN", "Always automatically"]
                                            }
                                            ValueSelector {
                                                text: "No. of items to keep"
                                                values: ["Latest 50", "Latest 100", "Latest 200", "Latest 500"]
                                                selectedIndex: 1
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Gallery"
                                        contents: Column {
                                            anchors.fill: parent
                                            Standard {
                                                text: "Face recognition"
                                                control: onOffButton.createObject(parent)
                                            }
                                            Caption {
                                                text: "Save names to your photos using automatic face recognition"
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Mail"
                                        contents: Column {
                                            anchors.fill: parent
                                            Standard {
                                                text: "Favorite folders"
                                                control: onOffButton.createObject(parent)
                                            }
                                            Standard {
                                                text: "Recent messages"
                                                control: onOffButton.createObject(parent)
                                            }
                                            Standard {
                                                text: "Threads"
                                                control: onOffButton.createObject(parent)
                                            }
                                            Caption {
                                                text: "Handle your messages as threads"
                                            }
                                            Standard {
                                                text: "Send messages as plain text"
                                                control: onOffButton.createObject(parent)
                                            }
                                            ValueSelector {
                                                text: "Default mailbox"
                                                values: ["Gmail", "Yahoo mail", "Hotmail"]
                                            }
                                            ValueSelector {
                                                text: "Message preview"
                                                values: ["None", "1 line", "2 lines"]
                                                selectedIndex: 2
                                            }
                                            ValueSelector {
                                                text: "Show HTML messages as"
                                                values: ["HTML", "Plain text"]
                                            }
                                            Standard {
                                                text: "Download images"
                                                control: onOffButton.createObject(parent)
                                            }
                                            Caption {
                                                text: "Download images automatically"
                                            }
                                            ValueSelector {
                                                text: "Send messages"
                                                values: ["Immediately", "On next sync"]
                                            }
                                        }
                                    }
                                    Page { title: "Messages" }
                                    Page { title: "Music" }
                                    Page { title: "Videos" }
                                    Page { title: "Web" }
                                }
                            }
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
            }
        }
    }
}
