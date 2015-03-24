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

import QtQuick 2.4
import Ubuntu.Components 1.1

Template {
    objectName: "stylesTemplate"

    Label {
        anchors.left: parent.left
        anchors.right: parent.right

        text: i18n.tr("Switch between old and new style header")
    }

    OptionSelector {
        model: [ i18n.tr('New Style Header'), i18n.tr('Header with Toolbar') ]
        expanded: true
        onSelectedIndexChanged: gallery.useDeprecatedToolbar = (selectedIndex == 0)
    }

    Label {
        anchors.left: parent.left
        anchors.right: parent.right

        text: i18n.tr("Switch between themes")
    }

    OptionSelector {
        model: [ 'Ambiance', 'SuruDark' ]
        expanded: true
        selectedIndex: (theme.name == 'Ubuntu.Components.Themes.SuruDark')
        onSelectedIndexChanged: theme.name = 'Ubuntu.Components.Themes.%1'.arg(model[selectedIndex])
    }
}
