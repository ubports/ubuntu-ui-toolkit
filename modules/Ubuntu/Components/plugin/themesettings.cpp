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
#include <QtCore/QDebug>

const bool traceThemeSettings = false;

#ifdef TRACE
#undef TRACE
#endif
#define TRACE \
    if (traceThemeSettings) \
        qDebug()


/*
  ThemeSettings class handles the selection of the application style based on
  global and application settings.

  User theme settings are stored in $HOME/.qmltheme/theme.ini file, which contains
  the current global theme name and the QML import paths. These settings are
  stored in "theme" and "imports" keywords.

  System themes are stored under /usr/share/themes/<theme-name>/qmltheme folder,
  where the common theme is named "default.qmltheme" and each application has its
  own theme, which can be either in the qmltheme folder or under a subfolder.

  OBS: we need to expose a QML element that handles these. Beside, applications
  should set up the theme before the plugin gets loaded by QML.
  */


// qmlviewer/qmlscene stores its settings in $HOME/.config/Nokia/QtQmlViewer.conf
// therefore it is possible to have application specific theme settings
// for it!

// user's theme settings are stored in ~/.qmltheme/theme.ini file and here are
// also stored the user specific themes
const char *PathFormat_GlobalThemeIniFile = "%1/.config/UITK/theme.ini";

#ifdef TARGET_DEMO
// for SDK demo we use the demo folder to store both global and private themes
const char *PathFormat_GlobalAppTheme = "demos/themes/%1/%2";
const char *PathFormat_GlobalThemeFile = "demos/themes/%1/default.qmltheme";
const char *systemThemePath = "demos/themes";

#else

const char *PathFormat_GlobalAppTheme = "/usr/share/themes/%1/qmltheme/%2";
const char *PathFormat_GlobalThemeFile = "/usr/share/themes/%1/qmltheme/default.qmltheme";
const char *systemThemePath = "/usr/share/themes";

#endif

const char *globalThemeKey = "theme";
const char *importPathsKey = "imports";
const char *appUseGlobalThemeKey = "UseSystemTheme";
const char *appThemeFileKey = "ThemeFile";


/*
 Instanciates the settins and connects the file system watcher
  */
ThemeSettings::ThemeSettings(QObject *parent) :
    QObject(parent),
    globalSettings(QString(PathFormat_GlobalThemeIniFile).arg(QDir::homePath()), QSettings::IniFormat),
    hasAppSettings(false)
{
    hasAppSettings = (QFile::exists(appSettings.fileName()) &&
            (appSettings.contains(appUseGlobalThemeKey) &&
             appSettings.contains(appThemeFileKey)));

    // check if we have system settings file, if not, create one
    if (!QFile::exists(globalSettings.fileName())) {
        // create the file by writing the default theme
#ifdef TARGET_DEMO
        globalSettings.setValue(globalThemeKey, "global-themes");
#else
        // TODO: figure out how to get the default theme for the release
        globalSettings.setValue(globalThemeKey, "Ambiance");
#endif
        globalSettings.sync();
    }

    if (!hasAppSettings || (hasAppSettings && appSettings.value(appUseGlobalThemeKey).toBool()))
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
  Returns the application's theme file as defined either in application or global
  theme settings file.
  An application can decide whether to use the global's default theme, the application
  specific theme defined by the global one or a private theme. To support this,
  applications must have the setting keys specified in \a appUseGlobalThemeKey and
  \a appThemeFileKey. If these are not defined, the global default theme will be loaded.
  */
QUrl ThemeSettings::themeFile() const
{
    QUrl result;
    if (hasAppSettings) {
        bool useGlobalTheme = appSettings.value(appUseGlobalThemeKey).toBool();
        if (useGlobalTheme) {
            QString appTheme = appSettings.value(appThemeFileKey).toString();
            if (!appTheme.isEmpty()) {
                // build theme file so it is taken from the global area
                result = QUrl::fromLocalFile(QString(PathFormat_GlobalAppTheme)
                               .arg(globalSettings.value(globalThemeKey).toString())
                               .arg(appTheme));
            }
        } else {
            // the theme specified in settings is a private one not dependent directly to
            // the global themes, and therefore can also be stored in qrc
            QString theme = appSettings.value(appThemeFileKey).toString();
            if (theme.startsWith("qrc:"))
                result = QUrl(theme);
            else
                result = QUrl::fromLocalFile(theme);
        }
    }

    // check if the application succeeded to set the URL, and if not, use the global defined
    // theme if possible
    if ((!result.isValid() || result.path().isEmpty())) {
        QString theme = globalSettings.value(globalThemeKey).toString();
        result = QUrl::fromLocalFile(QString(PathFormat_GlobalThemeFile).arg(theme));
        if (!QFile::exists(result.path()))
            result = QUrl();
    }

    TRACE << "file" << result.toString();

    return result;
}
/*
  This method sets the theme file for an application. It does not alter the global theme
  defined for the user. In case the application doesn't have theme settings set yet, it
  will try to create those aswell. Returns the URL (full path) to the theme file. The
  application will use the default theme setup if the \a url is invalid.
  */
QUrl ThemeSettings::setTheme(const QString &theme, bool global)
{
    // sync global theme
    if (theme.isEmpty() && !global) {
        ThemeEnginePrivate::setError("Invalid private file given for theme!");
        return QUrl();
    }

    TRACE << QString("appSettings.file() = %1").arg(appSettings.fileName());
    if (!hasAppSettings && QFile::exists(appSettings.fileName())) {
        // application is configured to accept settings, so force app settings
        hasAppSettings = true;
    }

    TRACE << QString("hasAppSettings = %1").arg(hasAppSettings);
    if (hasAppSettings) {

        // check whether the setting can be applied, if not, report error
        bool prevGlobal = appSettings.value(appUseGlobalThemeKey).toBool();
        QString prevTheme = appSettings.value(appThemeFileKey).toString();

        appSettings.setValue(appUseGlobalThemeKey, global);
        appSettings.setValue(appThemeFileKey, theme);
        QUrl theme = themeFile();
        TRACE << "trying to set theme to" << theme.toString();
        if (!theme.isValid() || !QFile::exists(theme.path())) {
            // roll back settings
            appSettings.setValue(appUseGlobalThemeKey, prevGlobal);
            appSettings.setValue(appThemeFileKey, prevTheme);
            ThemeEnginePrivate::setError("Invalid theme setting!");
            return QUrl();
        }

        if (hasAppSettings && !global) {
            TRACE << "stop watching user theme configuration changes";
            configWatcher.removePath(globalSettings.fileName());
        } else if (global && !prevGlobal) {
            TRACE << "start watching user theme configuration changes";
            configWatcher.addPath(globalSettings.fileName());
        }

        // sync settings
        return theme;
    }
    return QUrl();
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

    if (hasAppSettings) {
        imports = appSettings.value(importPathsKey).toString();
        if (!imports.isEmpty())
            result += imports.split(':');
    }

    return result;
}
