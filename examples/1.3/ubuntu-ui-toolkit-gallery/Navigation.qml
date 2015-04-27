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
    objectName: "navigationTemplate"

    Label {
        anchors.left: parent.left
        anchors.right: parent.right

        text: "In Ubuntu we use three predominant navigation structures:" +
              "<br/>- <b>flat</b><br/>- <b>contextual</b><br/>- <b>deep</b>"
        wrapMode: Text.WordWrap
        textFormat: Text.RichText
        font.weight: Font.Light
    }

    TemplateSectionNavigation {
        title: "Tabs (flat)"
        className: "Tabs"
        description: "Use them to switch between views of equal importance."
        designUrl: "https://design.ubuntu.com/apps/patterns/navigation"
        screenshot: "nav_tabs.png"
    }

    TemplateSectionNavigation {
        title: "Expansion (contextual)"
        documentation: ""
        description: "Use this to show more content"
        designUrl: "https://design.ubuntu.com/apps/patterns/navigation"
        screenshot: "nav_expansion.png"
    }

    TemplateSectionNavigation {
        title: "Page stack (deep)"
        className: "PageStack"
        description: "Use this to move from a main page to a secondary page."
        designUrl: "https://design.ubuntu.com/apps/patterns/navigation"
        screenshot: "nav_pagestack.png"
    }
}
