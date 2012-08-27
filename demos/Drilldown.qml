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

    PageStack {
        id: pageStack
        anchors.fill: parent

        Page {
            title: "Settings"
            contents: Column {
                anchors.fill: parent

                Standard {
                    // TODO
                    text: "Flight mode"
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
                    Page {
                        title: "Device"
                        iconSource: "avatar_contacts_list.png"
                    }
                    Page {
                        title: "Bluetooth"
                        iconSource: "avatar_contacts_list.png"
                    }
                    Page {
                        title: "Sounds and vibration"
                        iconSource: "avatar_contacts_list.png"
                    }
                    Page {
                        title: "Wallpaper"
                        iconSource: "avatar_contacts_list.png"
                    }
                    Page {
                        title: "Time and language"
                        iconSource: "avatar_contacts_list.png"
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
        }
    }
}
