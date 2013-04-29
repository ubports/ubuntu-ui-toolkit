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

Template {
    TemplateSection {
        title: "Checkbox"
        className: "CheckBox"

        TemplateRow {
            title: i18n.tr("Unchecked")

            CheckBox {
            }
        }

        TemplateRow {
            title: i18n.tr("Checked")

            CheckBox {
                checked: true
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")

            CheckBox {
                enabled: false
            }

            CheckBox {
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
            }
        }

        TemplateRow {
            title: i18n.tr("Checked")

            Switch {
                checked: true
            }
        }

        TemplateRow {
            title: i18n.tr("Disabled")

            Switch {
                enabled: false
            }

            Switch {
                enabled: false
                checked: true
            }
        }
    }
}
