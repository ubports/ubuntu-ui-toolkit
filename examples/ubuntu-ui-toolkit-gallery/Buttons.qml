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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3

Template {
    objectName: "buttonsTemplate"

    TemplateSection {
        className: "Button"

        TemplateRow {
            title: i18n.tr("Standard")

            Button {
                objectName: "button_text"
                text: i18n.tr("Call")
            }
        }

        TemplateRow {
            title: i18n.tr("Stroke")

            Button {
                objectName: "button_stroke"
                text: i18n.tr("Call")
                strokeColor: UbuntuColors.warmGrey
            }
        }

        TemplateRow {
            title: i18n.tr("Color")

            Button {
                objectName: "button_color"
                width: units.gu(20)
                action: Action {
                    text: i18n.tr("Call %1").arg(shortcut)
                    shortcut: 'Ctrl+C'
                    property bool flipped
                    onTriggered: flipped = !flipped
                }
                color: action.flipped ? UbuntuColors.blue : UbuntuColors.green
            }
        }

        TemplateRow {
            title: i18n.tr("Icon")

            Button {
                objectName: "button_iconsource"
                iconSource: "call.png"
            }
        }

        TemplateRow {
            title: i18n.tr("Icon+Text")

            Button {
                objectName: "button_iconsource_right_text"
                width: units.gu(10)
                text: i18n.tr("Call")
                iconSource: "call.png"
                iconPosition: "right"
            }

            Button {
                objectName: "button_iconsource_left_text"
                width: units.gu(10)
                text: i18n.tr("Call")
                iconSource: "call.png"
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")

            Button {
                objectName: "button_text_disabled"
                text: i18n.tr("Call")
                enabled: false
            }
        }
    }

    TemplateSection {
        className: "ComboButton"

        TemplateRow {
            title: i18n.tr("Collapsed")

            ComboButton {
                text: "Press me"
                objectName: "combobutton_collapsed"
                comboList:  UbuntuListView {
                    model: 10
                    delegate: Standard {
                        text: "item #" + modelData
                    }
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Icon")

            ComboButton {
                iconSource: "call.png"
                comboList:  UbuntuListView {
                    model: 10
                    delegate: Standard {
                        text: "item #" + modelData
                    }
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Icon+Text")

            ComboButton {
                text: "Answer"
                iconSource: "call.png"
                comboList:  UbuntuListView {
                    model: 10
                    delegate: Standard {
                        text: "item #" + modelData
                    }
                }
            }
        }
        TemplateRow {
            title: i18n.tr("Expanded")

            ComboButton {
                text: "Press me"
                objectName: "combobutton_expanded"
                expanded: true
                comboList:  UbuntuListView {
                    model: 10
                    delegate: Standard {
                        text: "item #" + modelData
                    }
                }
            }
        }
    }
}
