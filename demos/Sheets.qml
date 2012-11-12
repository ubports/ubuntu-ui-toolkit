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
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Template {
    title: i18n.tr("Sheets & Dialogues")

    Item {
        Component {
            id: closeButtonSheet
            DefaultSheet {
                id: sheet
                title: "Default sheet with close button"
                TextCustom {
                    text: "The Default Sheet allows an application to insert a content view over the focused view without disrupting the navigation pattern (tabs state or drill-down path are maintained. When the sheet is dismissed the user continues the journey from the point he left it)."
                    wrapMode: Text.WordWrap
                    width: units.gu(70)
                }
                onCloseClicked: PopupUtils.close(sheet)
            }
        }

        Component {
            id: doneButtonSheet
            DefaultSheet {
                id: sheet
                title: "Default sheet with done button"
                doneButton: true
                TextCustom {
                    text: "The Default Sheet allows an application to insert a content view over the focused view without disrupting the navigation pattern (tabs state or drill-down path are maintained. When the sheet is dismissed the user continues the journey from the point he left it)."
                    wrapMode: Text.WordWrap
                    width: units.gu(50)
                }
                onDoneClicked: PopupUtils.close(sheet)
            }
        }

        Component {
            id: composerSheet
            ComposerSheet {
                id: sheet
                title: "Composer sheet"
                TextCustom {
                    text: "A composer sheet has cancel and confirm buttons."
                }
                onCancelClicked: PopupUtils.close(sheet)
                onConfirmClicked: PopupUtils.close(sheet)
            }
        }

        Component {
             id: dialog
             Dialog {
                 id: dialogue

                 title: "Save file"
                 text: "Are you sure that you want to save this file?"

                 Button {
                     text: "cancel"
                     color: "silver"
                     onClicked: PopupUtils.close(dialogue)
                 }
                 Button {
                     text: "overwrite previous version"
                     color: "orange"
                     onClicked: PopupUtils.close(dialogue)
                 }
                 Button {
                     text: "save a copy"
                     color: "orange"
                     onClicked: PopupUtils.close(dialogue)
                 }
             }
         }

        Column {
            spacing: units.gu(4)

            TemplateRow {
                title: i18n.tr("Default\nSheet")

                Button {
                    text: i18n.tr("close button")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(closeButtonSheet)
                }

                Button {
                    text: i18n.tr("done button")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(doneButtonSheet)
                }
            }

            TemplateRow {
                title: i18n.tr("Composer\nSheet")

                Button {
                    text: i18n.tr("Click")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(composerSheet)
                }
            }

            TemplateRow {
                title: i18n.tr("Dialog")

                Button {
                    id: saveButton
                    text: i18n.tr("Save")
                    width: units.gu(16)
                    onClicked: PopupUtils.open(dialog, saveButton)
                }
            }
        }
    }
}
