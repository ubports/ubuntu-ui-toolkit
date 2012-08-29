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

Template {
    title: "Activity Indicator"
    id: indicators

    Column {
        spacing: 30
        TemplateRow {
            title: "Simple"

            TextField {
                placeholderText: "simple text field"
            }
        }

        TemplateRow {
            title: "Clear button"

            TextField {
                placeholderText: "clear always shown"
                hasClearButton: true
            }
        }

        TemplateRow {
            title: "Clear button"

            TextField {
                placeholderText: "type text to show clear button"
                hasClearButton: true
                alwaysShowClearButton: false
            }
        }

        TemplateRow {
            title: "Autoclear"

            TextField {
                placeholderText: "hint text"
                text: "This content will be cleared once focused"
                hasClearButton: true
                clearContentOnEditingStart: true
            }
        }
    }
}
