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
    title: "Check Box"

    Column {
        spacing: units.dp(30)

        TemplateRow {
            title: "Unchecked"
            CheckBox {
            }
        }

        TemplateRow {
            title: "Checked"
            CheckBox {
                checked: true
            }
        }

        TemplateRow {
            title: "Disabled"
            CheckBox {
                enabled: false
            }
        }

    }
}
