/*
 * Copyright 2013-2015 Canonical Ltd.
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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 *          Florian Boucault <florian.boucault@canonical.com>
 */

#include "ucdefaulttheme_p.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>

#include "uctheme_p.h"

UT_NAMESPACE_BEGIN

// User theme settings are stored in
// $XDG_CONFIG_HOME/ubuntu-ui-toolkit/theme.ini, which contains the current
// global theme name.
static const QString settingsFileFormat = QStringLiteral("%1/ubuntu-ui-toolkit/theme.ini");
static const QString defaultTheme = QStringLiteral("Ubuntu.Components.Themes.Ambiance");
static const QString themeKey = QStringLiteral("theme");

UCDefaultTheme::UCDefaultTheme(QObject *parent) :
    QObject(parent),
    m_settings(settingsFileFormat.arg(
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)), QSettings::IniFormat)
{
    // fundamental features rely on the default theme, so bail out if it's absent
    if (!pathFromThemeName(defaultTheme).isValid()) {
        qWarning() << "Mandatory default theme" << defaultTheme << "missing!";
    }

    // check if there is a theme settings file, if not, create one
    if (!QFile::exists(m_settings.fileName())) {
        // create the file by writing the default theme
        m_settings.setValue(themeKey, defaultTheme);
        m_settings.sync();
    }

    QObject::connect(&m_settingsFileWatcher, &QFileSystemWatcher::fileChanged,
                     this, &UCDefaultTheme::reloadSettings);
    reloadSettings();
}

void UCDefaultTheme::reloadSettings()
{
    m_settings.sync();
    m_settingsFileWatcher.addPath(m_settings.fileName());

    QString themeName = m_settings.value(themeKey).toString();
    if (themeName != m_themeName) {
        m_themeName = themeName;
        Q_EMIT themeNameChanged();
    }
}

QString UCDefaultTheme::themeName() const
{
    return m_themeName;
}

void UCDefaultTheme::setThemeName(const QString &themeName)
{
    if (themeName != m_themeName) {
        m_themeName = themeName;
        m_settings.setValue(themeKey, themeName);
        Q_EMIT themeNameChanged();
    }
}

UT_NAMESPACE_END
