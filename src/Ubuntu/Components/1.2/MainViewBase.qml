/*
 * Copyright 2012-2015 Canonical Ltd.
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
import Ubuntu.Components 1.2 as Toolkit
import Ubuntu.PerformanceMetrics 1.0
import QtQuick.Window 2.0

/*! \internal */
// Documentation is in MainView.qdoc
PageTreeNode {
    id: mainView
    property string applicationName: ""
    property bool anchorToKeyboard: false
    property alias headerColor: background.headerColor
    property alias backgroundColor: background.backgroundColor
    property alias footerColor: background.footerColor

    // FIXME: Make sure that the theming is only in the background, and the style
    //  should not occlude contents of the MainView. When making changes here, make
    //  sure that bug https://bugs.launchpad.net/manhattan/+bug/1124076 does not come back.
    Toolkit.StyledItem {
        id: background
        anchors.fill: parent
        style: Theme.createStyleComponent("MainViewStyle.qml", background)

        property color headerColor: backgroundColor
        property color backgroundColor: Theme.palette.normal.background
        property color footerColor: backgroundColor

        /*
          As we don't know the order the property bindings and onXXXChanged signals are evaluated
          we should rely only on one property when changing the theme to avoid intermediate
          theme changes due to properties being evaluated separately.

          Qt bug: https://bugreports.qt-project.org/browse/QTBUG-11712
         */
        property string themeName: (Toolkit.ColorUtils.luminance(backgroundColor) >= 0.85) ?
                                   "Ambiance" : "SuruDark"
        onThemeNameChanged: {
            // only change the theme if the current one is a system one.
            if (themeName !== "" && (Theme.name.search("Ubuntu.Components.Themes") >= 0)) {
                Theme.name = "Ubuntu.Components.Themes.%1".arg(themeName);
            }
        }
    }

    active: true
    property alias actions: unityActionManager.actions
    property alias actionManager: unityActionManager
    Toolkit.ActionManager {
        id: unityActionManager
        onQuit: {
            // FIXME Wire this up to the application lifecycle management API instead of quit().
            Qt.quit()
        }
    }

    onApplicationNameChanged: {
        if (applicationName !== "") {
            i18n.domain = applicationName;
            UbuntuApplication.applicationName = applicationName
        }
    }

    PerformanceOverlay {
        id: performanceOverlay
        active: false
    }
}
