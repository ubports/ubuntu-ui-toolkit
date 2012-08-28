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
import Ubuntu.Components.ListItems 0.1 as LI
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

                    LI.Standard {
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
                                LI.Standard {
                                    text: "Bluetooth"
                                    control: onOffButton.createObject(parent)
                                }
                                LI.Header {
                                    text: "My device"
                                }
                                LI.Subtitled {
                                    progression: true
                                    text: "Name and information"
                                    subText: "My device name"
                                }
                                LI.Standard {
                                    text: "Visibility"
                                    control: onOffButton.createObject(parent)
                                }
                                LI.Divider { }
                                LI.Standard {
                                    text: "Buetooth devices"
                                    progression: true
                                }
                                LI.Standard {
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
                                LI.Standard {
                                    control: Button { text: "Profile"; anchors.fill: parent }
                                }
                                LI.Standard {
                                    control: Button { text: "Volume"; anchors.fill: parent }
                                }
                                LI.Header { text: "Vibration profile" }
                                LI.Standard {
                                    text: "Ringing"
                                    control: onOffButton.createObject(parent)
                                }
                                LI.Standard {
                                    text: "Beep"
                                    control: onOffButton.createObject(parent)
                                }
                                LI.Standard {
                                    text: "Silent"
                                    control: onOffButton.createObject(parent)
                                }
                                LI.Header {
                                    text: "Alert tones"
                                }
                                LI.Subtitled {
                                    text: "Ringtone"
                                    subText: "Canonical tune"
                                    progression: true
                                }
                                LI.Subtitled {
                                    text: "Internet call alert"
                                    subText: "Canonical tune"
                                    progression: true
                                }
                                LI.Subtitled {
                                    text: "Mail alert tone"
                                    subText: "Email 1"
                                    progression: true
                                }
                                LI.Subtitled {
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
                                LI.Subtitled {
                                    text: "Time and date"
                                    subText: "28-08-12 12:05"
                                    progression: true
                                }
                                LI.SingleValue {
                                    text: "Language"
                                    value: "English (United Kingdom)"
                                    progression: true
                                }
                                LI.SingleValue {
                                    text: "Regional format"
                                    value: "Netherlands"
                                    progression: true
                                }
                                LI.Standard {
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
                                LI.Standard {
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
                                            LI.Standard {
                                                text: "Allow installations from\nnon-Store sources"
                                                control: onOffButton.createObject(parent)
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Calendar"
                                        contents: Column {
                                            anchors.fill: parent
                                            LI.Header { text: "General" }
                                            LI.ValueSelector {
                                                text: "Week starts on"
                                                values: ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"]
                                            }
                                            LI.MultiValue {
                                                text: "Calendar working week"
                                                values: ["Mon", "Tue", "Wed", "Thu", "Fri"]
                                                progression: true
                                            }
                                            LI.Header { text: "Events" }
                                            LI.SingleValue {
                                                text: "Default reminder displayed"
                                                value: "15 minutes before"
                                                progression: true
                                            }
                                            LI.SingleValue {
                                                text: "Default calendar"
                                                value: "Personal"
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Contacts"
                                        contents: Column {
                                            anchors.fill: parent
                                            LI.ValueSelector {
                                                text: "Prefer avatars from"
                                                values: ["No preference", "Google Talk", "Skype"]
                                            }
                                            LI.ValueSelector {
                                                text: "Show names"
                                                values: ["First name - Last name", "Last name - First name"]
                                            }
                                            LI.Standard {
                                                text: "Prefer nickname"
                                                control: onOffButton.createObject(parent)
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Feeds"
                                        contents: Column {
                                            anchors.fill: parent
                                            LI.ValueSelector {
                                                text: "Show Items"
                                                values: ["All items", "Unread items only"]
                                            }
                                            LI.ValueSelector {
                                                text: "Update feeds"
                                                values: ["Manually only", "Automatically in WLAN", "Always automatically"]
                                            }
                                            LI.ValueSelector {
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
                                            LI.Standard {
                                                text: "Face recognition"
                                                control: onOffButton.createObject(parent)
                                            }
                                            LI.Caption {
                                                text: "Save names to your photos using automatic face recognition"
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Mail"
                                        contents: Column {
                                            anchors.fill: parent
                                            LI.Standard {
                                                text: "Favorite folders"
                                                control: onOffButton.createObject(parent)
                                            }
                                            LI.Standard {
                                                text: "Recent messages"
                                                control: onOffButton.createObject(parent)
                                            }
                                            LI.Standard {
                                                text: "Threads"
                                                control: onOffButton.createObject(parent)
                                            }
                                            LI.Caption {
                                                text: "Handle your messages as threads"
                                            }
                                            LI.Standard {
                                                text: "Send messages as plain text"
                                                control: onOffButton.createObject(parent)
                                            }
                                            LI.ValueSelector {
                                                text: "Default mailbox"
                                                values: ["Gmail", "Yahoo mail", "Hotmail"]
                                            }
                                            LI.ValueSelector {
                                                text: "Message preview"
                                                values: ["None", "1 line", "2 lines"]
                                                selectedIndex: 2
                                            }
                                            LI.ValueSelector {
                                                text: "Show HTML messages as"
                                                values: ["HTML", "Plain text"]
                                            }
                                            LI.Standard {
                                                text: "Download images"
                                                control: onOffButton.createObject(parent)
                                            }
                                            LI.Caption {
                                                text: "Download images automatically"
                                            }
                                            LI.ValueSelector {
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
