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

Template {
    title: _("Progress Bar")

    Column {
        spacing: 30

        TemplateRow {
            title: _("Known") + " %"

            ProgressBar {
                id: progress
                value: progress.minimumValue

                SequentialAnimation on value {
                    loops: Animation.Infinite
                    NumberAnimation {
                        from: progress.minimumValue
                        to: progress.maximumValue
                        duration: 2000
                    }
                    PauseAnimation {duration: 1000}
                    ScriptAction { script: progress.value = progress.minimumValue; }
                    PauseAnimation {duration: 2000}
                }
            }
        }

        TemplateRow {
            title: _("Mixed") + " %"

            ProgressBar {
                id: progress2
                value: progress2.minimumValue

                SequentialAnimation on value {
                    loops: Animation.Infinite
                    NumberAnimation {
                        from: progress2.minimumValue
                        to: progress2.maximumValue
                        duration: 2000
                    }
                    PauseAnimation {duration: 1000}
                    ScriptAction { script: progress2.indeterminate = true; }
                    PauseAnimation {duration: 2000}
                    ScriptAction {
                        script: {
                            progress2.indeterminate = false
                            progress2.value = progress2.minimumValue
                        }
                    }
                }
            }
        }

        TemplateRow {
            title: _("Unknown") + " %"

            ProgressBar {
                indeterminate: true
            }
        }
    }
}
