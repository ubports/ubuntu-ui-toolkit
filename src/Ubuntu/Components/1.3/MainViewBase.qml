/*
 * Copyright 2015 Canonical Ltd.
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
import Ubuntu.Components 1.3 as Toolkit
import Ubuntu.PerformanceMetrics 1.0
import QtQuick.Window 2.0

/*!
  \internal
  \qmlabstract MainViewBase
  \inqmlmodule Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief The base class for MainView and MultiColumnView.
*/
PageTreeNode {
    id: mainView
    styleName: "MainViewStyle"

    /*!
      The property holds the application's name, which must be the same as the
      desktop file's name.
      The name also sets the name of the QCoreApplication and defaults for data
      and cache folders that work on the desktop and under confinement, as well as
      the default gettext domain.
      C++ code that writes files may use QStandardPaths::writableLocation with
      QStandardPaths::DataLocation or QStandardPaths::CacheLocation.
    */
    property string applicationName: ""

    /*!
      The property holds if the application should automatically resize the
      contents when the input method appears

      The default value is false.
    */
    property bool anchorToKeyboard: false

    /*!
      \qmlproperty color MainView::headerColor
      Color of the header's background.

      \sa backgroundColor, footerColor
    */
    property color headerColor: backgroundColor

    /*!
      \qmlproperty color MainView::backgroundColor
      Color of the background.

      The background is usually a single color. However if \l headerColor
      or \l footerColor are set then a gradient of colors will be drawn.

      For example, in order for the MainView to draw a color gradient beneath
      the content:
      \qml
          import QtQuick 2.4
          import Ubuntu.Components 1.2

          MainView {
              width: units.gu(40)
              height: units.gu(60)

              headerColor: "#343C60"
              backgroundColor: "#6A69A2"
              footerColor: "#8896D5"
          }
      \endqml

      \sa footerColor, headerColor
    */
    property color backgroundColor: theme.palette.normal.background

    /*!
      \qmlproperty color MainView::footerColor
      Color of the footer's background.

      \sa backgroundColor, headerColor
    */
    property color footerColor: backgroundColor

    Toolkit.Object {
        id: autoTheme
        // FIXME: Define the background colors in MainViewStyle and get rid of the properties
        //  in MainViewBase. That removes the need for auto-theming.

        /*
          As we don't know the order the property bindings and onXXXChanged signals are evaluated
          we should rely only on one property when changing the theme to avoid intermediate
          theme changes due to properties being evaluated separately.

          Qt bug: https://bugreports.qt-project.org/browse/QTBUG-11712
         */

        Connections {
            target: mainView

            onBackgroundColorChanged: {
                if (mainView.backgroundColor != theme.palette.normal.background) {
                    // custom color, proceed with auto-theming
                    autoTheme.themeName = (ColorUtils.luminance(backgroundColor) >= 0.85) ?
                                "Ambiance" : "SuruDark";
                }
            }
        }

        property string themeName
        onThemeNameChanged: {
            // only change the theme if the current one is a system one.
            if (themeName !== "" && (theme.name.search("Ubuntu.Components.Themes") == 0)) {
                mainView.theme.name = "Ubuntu.Components.Themes.%1".arg(themeName);
            }
        }
    }

    /*!
      \qmlproperty bool MainViewBase::active
      Root views are active by default.
    */
    active: true

    /*!
      \qmlproperty list<Action> MainViewBase::actions
      A global list of actions that will be available to the system (including HUD)
      as long as the application is running. For actions that are not always available to the
      system, but only when a certain \l Page is active, see the actions property of \l Page.
    */
    property alias actions: unityActionManager.actions

    /*!
      \qmlproperty ActionManager MainView::actionManager
      \readonly

      The ActionManager that supervises the global and local ActionContexts.
      The \l actions property should be used preferably since it covers most
      use cases. The ActionManager is accessible to have a more refined control
      over the actions, e.g. if one wants to add/remove actions dynamically, create
      specific action contexts, etc.
    */
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
