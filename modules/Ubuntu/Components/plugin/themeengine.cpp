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
 *
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 *          Florian Boucault <florian.boucault@canonical.com>
 */

#include "themeengine.h"
#include <QtQml/qqml.h>
#include <QtQml/QQmlEngine>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

/*!
  \qmltype Theme
  \inqmlmodule Ubuntu.Components 0.1
  \ingroup theming
  \brief The Theme element provides functionality to change the current theme.
*/

const QString DEFAULT_THEMES_PATH("/usr/share/themes");
const QString THEME_FOLDER_FORMAT("%1/%2/qmltheme/");
const QString PARENT_THEME_FILE("parent_theme");

ThemeEngine::ThemeEngine(QObject *parent) :
    QObject(parent)
{
    QObject::connect(&m_themeSettings, &ThemeSettings::themeNameChanged,
                     this, &ThemeEngine::onThemeNameChanged);
    m_path = pathFromThemeName(m_themeSettings.themeName());
}

void ThemeEngine::onThemeNameChanged()
{
    m_path = pathFromThemeName(m_themeSettings.themeName());
    Q_EMIT nameChanged();
}

QUrl ThemeEngine::pathFromThemeName(QString themeName)
{
    QString themesPath = QLatin1String(getenv("UBUNTU_UI_TOOLKIT_THEMES_PATH"));
    if (themesPath.isEmpty()) {
        themesPath = DEFAULT_THEMES_PATH;
    }

    QString themeFolder = THEME_FOLDER_FORMAT.arg(themesPath, themeName);
    return QUrl::fromLocalFile(themeFolder);
}


QString ThemeEngine::name() const
{
    return m_themeSettings.themeName();
}

void ThemeEngine::setName(QString name)
{
    m_themeSettings.setThemeName(name);
}

QUrl ThemeEngine::styleUrlForTheme(QString themeName, QString styleName)
{
    QUrl themePath = pathFromThemeName(themeName);
    QUrl styleUrl = themePath.resolved(styleName);

    if (!QFile::exists(styleUrl.toLocalFile())) {
        QString parent = parentThemeName(themeName);
        if (!parent.isEmpty()) {
            styleUrl = styleUrlForTheme(parent, styleName);
        }
    }

    return styleUrl;
}

QString ThemeEngine::parentThemeName(QString themeName)
{
    QString parentTheme;
    QUrl themePath = pathFromThemeName(themeName);
    QFile file(themePath.resolved(PARENT_THEME_FILE).toLocalFile());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        parentTheme = in.readLine();
    }
    return parentTheme;
}

QQmlComponent* ThemeEngine::createStyleComponent(QString styleName, QObject* parent)
{
    QQmlEngine* engine = qmlEngine(parent);
    QUrl styleUrl = styleUrlForTheme(m_themeSettings.themeName(), styleName);
    QQmlComponent *component = new QQmlComponent(engine, styleUrl, QQmlComponent::PreferSynchronous, parent);
    return component;
}
