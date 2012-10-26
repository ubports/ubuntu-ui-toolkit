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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "themeengine_p.h"
#include "themesettings_p.h"
#include "itemstyleattached.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

/*
  ThemeSettings class handles the selection of the application style based on
  global and application settings.

  User theme settings are stored in $HOME/.config/UITK/theme.ini file, which contains
  the current global theme name and the QML import paths. These settings are
  stored in "theme" and "imports" keywords.

  System themes are stored under the /usr/share/themes/<theme-name>/qmltheme folder,
  where the common theme is named "default.qmltheme" and each application has its
  own theme, which can be either in the qmltheme folder or under a subfolder.
  */


// qmlviewer/qmlscene stores its settings in $HOME/.config/Nokia/QtQmlViewer.conf
// therefore it is possible to have application specific theme settings
// for it!

// user's theme settings are stored in ~/.config/UITK/theme.ini file and here are
// also stored the user specific themes
const char *PathFormat_GlobalThemeIniFile = "%1/.config/UITK/theme.ini";

const char *PathFormat_GlobalThemeFile = "/%1/qmltheme/default.qmltheme";

const char *globalThemeKey = "theme";
const char *importPathsKey = "imports";

/*
 Instanciates the settins and connects the file system watcher
  */
ThemeSettings::ThemeSettings(QObject *parent) :
    QObject(parent),
    globalSettings(QString(PathFormat_GlobalThemeIniFile).arg(QDir::homePath()), QSettings::IniFormat)
{
    // check if we have system settings file, if not, create one
    if (!QFile::exists(globalSettings.fileName())) {
        // create the file by writing the default theme
        globalSettings.setValue(globalThemeKey, "Ambiance");
        globalSettings.sync();
    }

    configWatcher.addPath(globalSettings.fileName());

    // connect theme observer to the slot
    QObject::connect(&configWatcher, SIGNAL(fileChanged(QString)),
                     this, SLOT(refreshSettings()));
}

void ThemeSettings::refreshSettings()
{
    // sync settings
    globalSettings.sync();
    Q_EMIT themeSettingsChanged();
}

/*
  Returns the theme file as defined for the user.
  */
QUrl ThemeSettings::themeFile() const
{
    QUrl result;
    // returns the global theme file
    QString theme = globalSettings.value(globalThemeKey).toString();
    result = QUrl::fromLocalFile(themeFolder() + QString(PathFormat_GlobalThemeFile).arg(theme));
    if (!QFile::exists(result.path()))
        result = QUrl();
    return result;
}

/*
  Returns the QML import paths supporting the defined theme file.
  */
QStringList ThemeSettings::imports() const
{
    QStringList result;
    QString imports;

    imports = globalSettings.value(importPathsKey).toString();
    if (!imports.isEmpty())
        result += imports.split(':');

    return result;
}
