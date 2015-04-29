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
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Template {
    objectName: "sheetsTemplate"

    Label {
        anchors.left: parent.left
        anchors.right: parent.right

        text: i18n.tr("The visual style of the Sheet is early work and thus clunky looking.")
        font.italic: true
        wrapMode: Text.WordWrap
        font.weight: Font.Light
    }

    TemplateSection {
        title: "Default sheet"
        className: "DefaultSheet"
        documentation: "qml-ubuntu-components-popups-%1.html".arg(className.toLowerCase())

        TemplateRow {
            title: i18n.tr("Standard")

            Button {
                text: i18n.tr("Open")
                width: units.gu(16)
                onClicked: PopupUtils.open(defaultSheet, null)
            }
        }

        Component {
            id: defaultSheet
            DefaultSheet {
                id: sheet
                title: i18n.tr("Title")
                Label {
                    anchors.fill: parent
                    wrapMode: Text.WordWrap
                    text:
                    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. " +
                    "Integer et ante at erat malesuada accumsan in eget mauris. " +
                    "Nunc ultrices tristique laoreet. In non tristique lorem. " +
                    "Donec in libero ut libero pretium lacinia. Proin dictum faucibus viverra. "
                }
            }
        }
    }

    TemplateSection {
        title: "Composer sheet"
        className: "ComposerSheet"
        documentation: "qml-ubuntu-components-popups-%1.html".arg(className.toLowerCase())

        TemplateRow {
            title: i18n.tr("Standard")

            Button {
                text: i18n.tr("Open")
                width: units.gu(16)
                onClicked: PopupUtils.open(composerSheet, null)
            }
        }

        Component {
            id: composerSheet
            ComposerSheet {
                id: sheet
                title: i18n.tr("Title")
                Label {
                    anchors.fill: parent
                    wrapMode: Text.WordWrap
                    text:
                    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. " +
                    "Integer et ante at erat malesuada accumsan in eget mauris. " +
                    "Nunc ultrices tristique laoreet. In non tristique lorem. " +
                    "Donec in libero ut libero pretium lacinia. Proin dictum faucibus viverra. "
                }
                onCancelClicked: PopupUtils.close(sheet)
                onConfirmClicked: PopupUtils.close(sheet)
            }
        }
    }
}
