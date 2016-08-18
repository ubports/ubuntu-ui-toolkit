/*
 * Copyright 2016 Canonical Ltd.
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

MainView {
    width: units.gu(48)
    height: units.gu(120)
    objectName: "mainView"

    Column {
        anchors.fill: parent
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: units.gu(3)

        OptionSelector {
            objectName: "option_selector"
            text: i18n.tr("option_selector")
            expanded: true
            model: [i18n.tr("Red"),
                i18n.tr("Blue"),
                i18n.tr("Green"),
                i18n.tr("Yellow"),
                i18n.tr("Black")]
        }
    }
}
