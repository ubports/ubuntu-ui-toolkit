/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Components 1.1

Template {
    objectName: "colorsTemplate"

    TemplateSection {
        title: "UbuntuColors"
        className: "UbuntuColors"

        // Except orange, none of the colors from UbuntuComponents 1.0
        // are included because they may be deprecated soon.
        TemplateRow {
            title: i18n.tr("Orange")
            UbuntuShape {
                color: UbuntuColors.orange
            }
        }
        TemplateRow {
            title: i18n.tr("Light grey")
            UbuntuShape {
                color: UbuntuColors.lightGrey
            }
        }
        TemplateRow {
            title: i18n.tr("Dark grey")
            UbuntuShape {
                color: UbuntuColors.darkGrey
            }
        }
        TemplateRow {
            title: i18n.tr("Red")
            UbuntuShape {
                color: UbuntuColors.red
            }
        }
        TemplateRow {
            title: i18n.tr("Green")
            UbuntuShape {
                color: UbuntuColors.green
            }
        }
        TemplateRow {
            title: i18n.tr("Blue")
            UbuntuShape {
                color: UbuntuColors.blue
            }
        }
        TemplateRow {
            title: i18n.tr("Purple")
            UbuntuShape {
                color: UbuntuColors.purple
            }
        }
    }
}
