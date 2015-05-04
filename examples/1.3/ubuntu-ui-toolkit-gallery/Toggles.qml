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

Template {
    objectName: "togglesTemplate"

    TemplateSection {
        title: "Checkbox"
        className: "CheckBox"

        TemplateRow {
            title: i18n.tr("Unchecked")

            CheckBox {
                objectName: "checkbox_unchecked"
            }
        }

        TemplateRow {
            title: i18n.tr("Checked")

            CheckBox {
                objectName: "checkbox_checked"
                checked: true
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")

            CheckBox {
                objectName: "checkbox_disabled_unchecked"
                enabled: false
            }

            CheckBox {
                objectName: "checkbox_disabled_checked"
                enabled: false
                checked: true
            }
        }
    }


    TemplateSection {
        title: "Switch"
        className: "Switch"

        TemplateRow {
            title: i18n.tr("Unchecked")

            Switch {
                objectName: "switch_unchecked"
            }
        }

        TemplateRow {
            title: i18n.tr("Checked")

            Switch {
                objectName: "switch_checked"
                checked: true
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")

            Switch {
                objectName: "switch_disabled_unchecked"
                enabled: false
            }

            Switch {
                objectName: "switch_disabled_checked"
                enabled: false
                checked: true
            }
        }
    }
}
