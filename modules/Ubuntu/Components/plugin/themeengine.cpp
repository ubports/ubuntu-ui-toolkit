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
#include <QtCore/QLibraryInfo>

/*!
    \qmltype Theme
    \instantiates ThemeEngine
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup theming
    \brief The Theme class provides facilities to interact with the current theme.

    A global instance is exposed as the \b Theme context property.

    The theme defines the visual aspect of the Ubuntu components.

    Example changing the current theme:

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    Item {
        Button {
            onClicked: Theme.name = "Ubuntu.Components.Themes.Ambiance"
        }
    }
    \endqml

    Example creating a style component:

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    StyledItem {
        id: myItem
        style: Theme.createStyleComponent("MyItemStyle.qml", myItem)
    }
    \endqml

    \sa {StyledItem}
*/

const QString THEME_FOLDER_FORMAT("%1/%2/");
const QString PARENT_THEME_FILE("parent_theme");

ThemeEngine::ThemeEngine(QObject *parent) :
    QObject(parent)
{
    QObject::connect(&m_themeSettings, &ThemeSettings::themeNameChanged,
                     this, &ThemeEngine::onThemeNameChanged);
    updateThemePaths();
}

void ThemeEngine::onThemeNameChanged()
{
    updateThemePaths();
    Q_EMIT nameChanged();
}

QUrl ThemeEngine::pathFromThemeName(QString themeName)
{
    QString themesPath = QLatin1String(getenv("UBUNTU_UI_TOOLKIT_THEMES_PATH"));
    if (themesPath.isEmpty()) {
        themesPath = QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath);
    }

    QString themeFolder = THEME_FOLDER_FORMAT.arg(themesPath, themeName.replace('.', '/'));
    return QUrl::fromLocalFile(themeFolder);
}

void ThemeEngine::updateThemePaths()
{
    m_themePaths.clear();

    QString themeName = m_themeSettings.themeName();
    while (!themeName.isEmpty()) {
        QUrl themePath = pathFromThemeName(themeName);
        m_themePaths.append(themePath);
        themeName = parentThemeName(themeName);
    }
}

/*!
    \qmlproperty string Theme::name

    The name of the current theme.
*/
QString ThemeEngine::name() const
{
    return m_themeSettings.themeName();
}

void ThemeEngine::setName(QString name)
{
    m_themeSettings.setThemeName(name);
}

QUrl ThemeEngine::styleUrlForTheme(QString styleName)
{
    QUrl styleUrl;

    Q_FOREACH (QUrl themePath, m_themePaths) {
        styleUrl = themePath.resolved(styleName);
        if (QFile::exists(styleUrl.toLocalFile())) {
            break;
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

/*!
    \qmlmethod Component Theme::createStyleComponent(string styleName, object parent)

    Returns an instance of the style component named \a styleName.
*/
QQmlComponent* ThemeEngine::createStyleComponent(QString styleName, QObject* parent)
{
    QQmlEngine* engine = qmlEngine(parent);
    QUrl styleUrl = styleUrlForTheme(styleName);
    QQmlComponent *component = new QQmlComponent(engine, styleUrl, QQmlComponent::PreferSynchronous, parent);
    return component;
}
