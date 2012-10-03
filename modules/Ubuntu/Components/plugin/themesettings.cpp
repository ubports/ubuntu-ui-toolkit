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

#include "themeengine.h"
#include "themeengine_p.h"
#include "style.h"
#include "styleditem.h"
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeItem>

#include <QSettings>

const char *globalThemeConfigFile = "demos/theme.cfg";
const char *defaultThemeFile = "modules/Ubuntu/Components/defaulttheme.css";

const char *globalThemeKey = "theme";
const char *appUseGlobalThemeKey = "UseSystemTheme";
const char *appThemeFileKey = "ThemeFile";

const char *globalThemePathFormat = "%1/qthm/apps/%2";
const char *globalThemeFileFormat = "%1/qthm/default.qthm";

ThemeSettings::ThemeSettings(QObject *parent) :
    configWatcher(parent),
    hasAppSettings(false),
    hasGlobalSettings(false),
    initialized(false)
{
    QSettings appSettings;
    hasAppSettings = ((appSettings.status() == QSettings::NoError) &&
            (appSettings.contains(appUseGlobalThemeKey) &&
             appSettings.contains(appThemeFileKey)));
    // check if we have system settings file
    hasGlobalSettings = QFile::exists(globalThemeConfigFile);

    if ((hasAppSettings && appSettings.value(appUseGlobalThemeKey).toBool()) || hasGlobalSettings)
        configWatcher.addPath(globalThemeConfigFile);

    QObject::connect(&configWatcher, SIGNAL(fileChanged(QString)), parent, SLOT(_q_updateTheme()));
}

/*!
  \internal
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
        QSettings settings;
        bool useGlobalTheme = settings.value(appUseGlobalThemeKey).toBool();
        if (useGlobalTheme) {
            QUrl appTheme = settings.value(appThemeFileKey).toUrl();
            if (appTheme.isValid()) {
                // build theme file so it is taken from the global area
                QSettings global(globalThemeConfigFile);
                result = global.value(globalThemeKey).toUrl();
                result.setPath(QString(globalThemePathFormat)
                               .arg(result.path())
                               .arg(appTheme.path()));
            }
        } else {
            // the theme specified in settings is a private one not dependent directly to
            // the global themes
            result = settings.value(appThemeFileKey).toUrl();
        }

    }

    // check if the application succeeded to set the URL, and if not, use the global defined
    // theme if possible
    if (hasGlobalSettings && !result.isValid()) {
        QSettings global(globalThemeConfigFile);
        QString path = global.value(globalThemeKey).toUrl().path();
        result.setPath(QString(globalThemeFileFormat).arg(path));
    }

    return result;
}
/*!
  \internal
  This method sets the theme file for an application. It does not alter the global theme
  defined for the user. In case the application doesn't have theme settings set yet, it
  will try to create those aswell. Returns the URL (full path) to the theme file. The
  application will use the default theme setup if the \a url is invalid.
  */
QUrl ThemeSettings::setThemeFile(const QUrl &url, bool global)
{
    if (!url.isValid() && !global) {
        ThemeEnginePrivate::setError("Invalid private URL given for theme!");
        return QUrl();
    }
    QSettings settings;
    if (!hasAppSettings && (settings.status() == QSettings::NoError)) {
        // application is configured to accept settings, so force app settings
        hasAppSettings = true;
    }
    if (hasAppSettings) {
        settings.setValue(appUseGlobalThemeKey, global);
        settings.setValue(appThemeFileKey, url);
        // call themeFile() to return the correct URL
        return themeFile();
    }
    return QUrl();
}
