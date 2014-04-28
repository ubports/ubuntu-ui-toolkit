/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 1.0

Template {
    objectName: "iconsTemplate"

    TemplateSection {
        className: "Icon"

        TemplateRow {
            title: i18n.tr("Scaling")
            spacing: units.gu(2)

            Icon {
                name: "call-start"
                width: 16
                height: 16
                anchors.verticalCenter: parent.verticalCenter
            }

            Icon {
                name: "call-start"
                width: 48
                height: 48
                anchors.verticalCenter: parent.verticalCenter
            }

            Icon {
                name: "call-start"
                width: 96
                height: 96
            }
        }

        TemplateRow {
            title: i18n.tr("Colorization")
            spacing: units.gu(2)

            Icon {
                name: "computer-symbolic"
                width: 24
                height: 24
            }

            Icon {
                name: "computer-symbolic"
                color: UbuntuColors.orange
                keyColor: "#bebebe"
                width: 24
                height: 24
            }

            Icon {
                name: "computer-symbolic"
                color: UbuntuColors.lightAubergine
                keyColor: "#bebebe"
                width: 24
                height: 24
            }
        }

        TemplateRow {
            title: i18n.tr("Theme")
            spacing: units.gu(2)


            Flow {
                anchors.fill: parent
                spacing: units.gu(2)

                Repeater {
                    model: [ "add", "add-to-call", "back", "browser-tabs",
                             "browser-timeline", "calendar", "calendar-today",
                             "call-end", "call-start", "camcorder", "camera-flip",
                             "cancel", "chevron", "clear", "close", "compose",
                             "contact", "delete", "dialer", "dropdown-menu", "edit",
                             "email", "erase", "external-link", "favorite-selected",
                             "favorite-unselected", "filter", "find", "flash-auto",
                             "flash-off", "flash-on", "go-next", "go-previous",
                             "go-to", "help", "hud", "import-image", "incoming-call",
                             "keyboard-caps-active", "keyboard-caps-lock",
                             "keyboard-caps", "keyboard-return", "keypad",
                             "language-chooser", "location", "media-playback-pause",
                             "media-playback-start", "media-playlist-repeat",
                             "media-playlist-shuffle", "media-skip-backward",
                             "media-skip-forward", "messages", "microphone-mute",
                             "microphone", "missed-call", "navigation-menu",
                             "new-contact", "new-event", "new-message",
                             "outgoing-call", "properties", "redo", "reload",
                             "remove-from-call", "reset", "save", "search", "select",
                             "settings", "share", "speaker-mute", "speaker",
                             "switch", "system-log-out", "system-restart", "system-shutdown",
                             "torch-off", "torch-on", "undo", "view-fullscreen",
                             "view-restore", "voicemail" ]
                    Icon {
                        id: themedIcon
                        name: modelData
                        width: 24
                        height: 24
                        MouseArea {
                            anchors.fill: parent
                            onClicked: PopupUtils.open(iconPop, themedIcon, { 'icon': themedIcon.name })
                        }
                        Component {
                            id: iconPop
                            Popover {
                                id: iconPopover
                                property string icon: "N/A"

                                Column {
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    anchors.right: parent.right

                                    ListItem.Standard {
                                        iconName: iconPopover.icon
                                        text: iconPopover.icon
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
