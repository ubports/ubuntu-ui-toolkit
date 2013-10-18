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

#include "uctheme.h"
#include "listener.h"
#include "quickutils.h"

#include <QtQml/qqml.h>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QLibraryInfo>

/*!
    \qmltype Theme
    \instantiates UCTheme
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

UCTheme::UCTheme(QObject *parent) :
    QObject(parent),
    m_palette(NULL)
{
    m_name = m_themeSettings.themeName();
    QObject::connect(&m_themeSettings, &UCThemeSettings::themeNameChanged,
                     this, &UCTheme::onThemeNameChanged);
    updateThemePaths();

    loadPalette();
    QObject::connect(this, &UCTheme::nameChanged,
                     this, &UCTheme::loadPalette, Qt::UniqueConnection);
}

void UCTheme::onThemeNameChanged()
{
    if (m_themeSettings.themeName() != m_name) {
        m_name = m_themeSettings.themeName();
        updateThemePaths();
        Q_EMIT nameChanged();
    }
}

QUrl UCTheme::pathFromThemeName(QString themeName)
{
    QString themesPath = QLatin1String(getenv("UBUNTU_UI_TOOLKIT_THEMES_PATH"));
    if (themesPath.isEmpty()) {
        themesPath = QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath);
    }

    QString themeFolder = THEME_FOLDER_FORMAT.arg(themesPath, themeName.replace('.', '/'));
    QString absoluteThemeFolder = QDir(themeFolder).absolutePath();
    // QUrl needs a trailing slash to understand it's a directory
    return QUrl::fromLocalFile(absoluteThemeFolder.append("/"));
}

void UCTheme::updateThemePaths()
{
    m_themePaths.clear();

    QString themeName = m_name;
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
QString UCTheme::name() const
{
    return m_name;
}

void UCTheme::setName(const QString& name)
{
    if (name != m_name) {
        QObject::disconnect(&m_themeSettings, &UCThemeSettings::themeNameChanged,
                            this, &UCTheme::onThemeNameChanged);
        m_name = name;
        updateThemePaths();
        Q_EMIT nameChanged();
    }
}

/*!
    \qmlproperty Palette Theme::palette

    The palette of the current theme.
*/
QObject* UCTheme::palette() const
{
    return m_palette;
}

QUrl UCTheme::styleUrl(const QString& styleName)
{
    QUrl styleUrl;

    Q_FOREACH (const QUrl& themePath, m_themePaths) {
        styleUrl = themePath.resolved(styleName);
        if (QFile::exists(styleUrl.toLocalFile())) {
            break;
        }
    }

    return styleUrl;
}

QString UCTheme::parentThemeName(const QString& themeName)
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
QQmlComponent* UCTheme::createStyleComponent(const QString& styleName, QObject* parent)
{
    QQmlComponent *component = NULL;

    if (parent != NULL) {
        QQmlEngine* engine = qmlEngine(parent);
        if (engine != NULL) {
            QUrl url = styleUrl(styleName);
            component = new QQmlComponent(engine, url, QQmlComponent::PreferSynchronous, parent);
            if (component->isError()) {
                delete component;
                component = NULL;
            }
        }
    }

    return component;
}

void UCTheme::registerToContext(QQmlContext* context)
{
    // register Theme
    context->setContextProperty("Theme", this);

    ContextPropertyChangeListener *themeChangeListener =
        new ContextPropertyChangeListener(context, "Theme");
    QObject::connect(this, SIGNAL(nameChanged()),
                     themeChangeListener, SLOT(updateContextProperty()));
}

void UCTheme::loadPalette()
{
    if (m_palette != NULL) {
        delete m_palette;
    }
    m_palette = QuickUtils::instance().createQmlObject((styleUrl("Palette.qml")));
    Q_EMIT paletteChanged();
}
