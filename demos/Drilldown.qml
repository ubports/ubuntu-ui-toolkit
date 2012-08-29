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
import Ubuntu.Components.ListItems 0.1 as ListItem
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

                    ListItem.Standard {
                        text: "Flight mode"
                        // TODO: replace by switch when that component is available.
                        control: onOffButton.createObject(parent)
                    }
                    PageList {
                        pageStack: stack
                        Page {
                            title: "Device"

                            contents: PageList {
                                pageStack: stack
                                Page {
                                    title: "Call"
                                    contents: Column {
                                        anchors.fill: parent
                                        ListItem.Standard {
                                            text: "Call waiting"
                                            control: onOffButton.createObject(parent)
                                        }
                                        ListItem.Standard {
                                            text: "Noise cancellation"
                                            control: onOffButton.createObject(parent)
                                        }
                                        ListItem.ValueSelector {
                                            text: "Send my caller ID"
                                            values: ["Set by network", "Yes", "No"]
                                        }
                                    }
                                }
                                Page {
                                    title: "Battery"
                                    contents: Column {
                                        anchors.fill: parent
                                        ListItem.Caption {
                                            text: "Battery level: 18%\n\nRemaining talk time: 1 hour 34 minutes\nRemaining standby time: 20 hours 58 minutes"
                                        }
                                        ListItem.ValueSelector {
                                            text: "Power saving mode"
                                            values: ["Off", "On", "Automatic"]
                                        }
                                        ListItem.Caption {
                                            text: "Battery condition: Good"
                                        }
                                    }
                                }
                                Page {
                                    title: "Display"
                                    contents: Column {
                                        anchors.fill: parent
                                        ListItem.ValueSelector {
                                            text: "Display time-out"
                                            values: ["15 seconds", "30 seconds", "1 minute", "2 minutes", "3 minutes"]
                                            selectedIndex: 1
                                        }
                                        ListItem.Standard {
                                            text: "Stand-by screen"
                                            control: onOffButton.createObject(parent)
                                        }
                                        ListItem.ValueSelector {
                                            text: "Colour profile"
                                            values: ["Normal", "Vivid", "Subdued"]
                                        }
                                    }
                                }
                                Page {
                                    title: "NFC"
                                    contents: Column {
                                        anchors.fill: parent
                                        ListItem.Standard {
                                            text: "NFC"
                                            control: onOffButton.createObject(parent)
                                        }
                                        ListItem.Standard {
                                            text: "Confirm sharing and\nconnecting"
                                            control: onOffButton.createObject(parent)
                                        }
                                    }
                                }
                            }
                        }
                        Page {
                            title: "Sounds and vibration"

                            contents: Column {
                                anchors.fill: parent
                                ListItem.Header { text: "Vibration profile" }
                                ListItem.Standard {
                                    text: "Ringing"
                                    control: onOffButton.createObject(parent)
                                }
                                ListItem.Standard {
                                    text: "Beep"
                                    control: onOffButton.createObject(parent)
                                }
                                ListItem.Standard {
                                    text: "Silent"
                                    control: onOffButton.createObject(parent)
                                }
                                ListItem.Header {
                                    text: "Alert tones"
                                }
                                ListItem.ValueSelector {
                                    text: "Ringtone"
                                    values: ["Canonical tune", "Linux tune", "Penguin tune"]
                                }
                                ListItem.ValueSelector{
                                    text: "Message alert tone"
                                    values: ["Beep", "Bleep", "Blip"]
                                }
                            }
                        }
                        Page {
                            title: "Applications"

                            contents: Column {
                                anchors.fill: parent
                                ListItem.Standard {
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
                                            ListItem.Standard {
                                                text: "Allow installations from\nnon-Store sources"
                                                control: onOffButton.createObject(parent)
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Calendar"
                                        contents: Column {
                                            anchors.fill: parent
                                            ListItem.Header { text: "General" }
                                            ListItem.ValueSelector {
                                                text: "Week starts on"
                                                values: ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"]
                                            }
                                            ListItem.Header { text: "Events" }
                                            ListItem.ValueSelector{
                                                text: "Default reminder displayed"
                                                values: ["None", "0 minutes before", "10 minutes before", "15 minutes before", "1 hour before"]
                                                selectedIndex: 3
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Contacts"
                                        contents: Column {
                                            anchors.fill: parent
                                            ListItem.ValueSelector {
                                                text: "Prefer avatars from"
                                                values: ["No preference", "Google Talk", "Skype"]
                                            }
                                            ListItem.ValueSelector {
                                                text: "Show names"
                                                values: ["First name - Last name", "Last name - First name"]
                                            }
                                            ListItem.Standard {
                                                text: "Prefer nickname"
                                                control: onOffButton.createObject(parent)
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Feeds"
                                        contents: Column {
                                            anchors.fill: parent
                                            ListItem.ValueSelector {
                                                text: "Show Items"
                                                values: ["All items", "Unread items only"]
                                            }
                                            ListItem.ValueSelector {
                                                text: "Update feeds"
                                                values: ["Manually only", "Automatically in WLAN", "Always automatically"]
                                            }
                                            ListItem.ValueSelector {
                                                text: "No. of items to keep"
                                                values: ["Latest 50", "Latest 100", "Latest 200", "Latest 500"]
                                                selectedIndex: 1
                                            }
                                        }
                                    }
                                    Page {
                                        title: "Mail"
                                        contents: Column {
                                            anchors.fill: parent
                                            ListItem.Standard {
                                                text: "Favorite folders"
                                                control: onOffButton.createObject(parent)
                                            }
                                            ListItem.Standard {
                                                text: "Recent messages"
                                                control: onOffButton.createObject(parent)
                                            }
                                            ListItem.Standard {
                                                text: "Threads"
                                                control: onOffButton.createObject(parent)
                                            }
                                            ListItem.ValueSelector {
                                                text: "Default mailbox"
                                                values: ["Gmail", "Yahoo mail", "Hotmail"]
                                            }
                                            ListItem.ValueSelector {
                                                text: "Message preview"
                                                values: ["None", "1 line", "2 lines"]
                                                selectedIndex: 2
                                            }
                                            ListItem.ValueSelector {
                                                text: "Show HTML messages as"
                                                values: ["HTML", "Plain text"]
                                            }
                                            ListItem.Standard {
                                                text: "Download images"
                                                control: onOffButton.createObject(parent)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
