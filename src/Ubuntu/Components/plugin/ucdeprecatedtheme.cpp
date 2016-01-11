/*
 * Copyright 2016 Canonical Ltd.
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
 */

#include "ucnamespace.h"
#include "ucdeprecatedtheme.h"
#include "uctheme.h"
#include "quickutils.h"
#include "listener.h"
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlEngine>

/*!
    \qmltype Theme
    \instantiates UCTheme
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup theming
    \brief The Theme class provides facilities to interact with the current theme.

    A global instance is exposed as the \b Theme context property.

    The theme defines the visual aspect of the Ubuntu components.

    Example changing the current theme:

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.2

    Item {
        Button {
            onClicked: Theme.name = "Ubuntu.Components.Themes.Ambiance"
        }
    }
    \endqml

    Example creating a style component:

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.2

    StyledItem {
        id: myItem
        style: Theme.createStyleComponent("MyItemStyle.qml", myItem)
    }
    \endqml

    \sa {StyledItem}
*/
UCDeprecatedTheme::UCDeprecatedTheme(UCTheme *theme)
    : QObject(theme)
{
    m_notes = QHash<QString, bool>();
    connect(theme, SIGNAL(nameChanged()),
            this, SIGNAL(nameChanged()));
    connect(theme, SIGNAL(paletteChanged()),
            this, SIGNAL(paletteChanged()));
}

UCTheme *UCDeprecatedTheme::defaultTheme()
{
    QQmlEngine *engine = qmlEngine(this);
    Q_ASSERT(engine);
    return UCTheme::defaultTheme(engine);
}

void UCDeprecatedTheme::showDeprecatedNote(QObject *onItem, const char *note)
{
    if (!QuickUtils::showDeprecationWarnings())
        return;

    QQmlContext ctx(QQmlEngine::contextForObject(onItem));
    // No warnings due to deprecated code used in the components themselves
    if (ctx.baseUrl().toString().contains("/Ubuntu/Components/"))
        return;
    // Warnings without a filename are not helpful
    if (ctx.baseUrl().isEmpty())
        return;

    QString noteId(QString("%1.%2").arg(note).arg(onItem->metaObject()->className()));
    if (m_notes.contains(noteId))
        return;
    QByteArray suppressNote = qgetenv("SUPPRESS_DEPRECATED_NOTE");
    qmlInfo(onItem) << note;
    m_notes.insert(noteId, true);
}

/*!
    \qmlproperty string Theme::name

    The name of the current theme.
*/
QString UCDeprecatedTheme::name()
{
    showDeprecatedNote(this, "Theme.name is deprecated. Use ThemeSettings instead.");
    return defaultTheme()->name();
}
void UCDeprecatedTheme::setName(const QString& name)
{
    showDeprecatedNote(this, "Theme.name is deprecated. Use ThemeSettings instead.");
    defaultTheme()->setName(name);
}
void UCDeprecatedTheme::resetName()
{
    showDeprecatedNote(this, "Theme.name is deprecated. Use ThemeSettings instead.");
    defaultTheme()->resetName();
}

/*!
    \qmlproperty Palette Theme::palette

    The palette of the current theme.
*/
QObject* UCDeprecatedTheme::palette()
{
    showDeprecatedNote(this, "Theme.palette is deprecated. Use ThemeSettings instead.");
    return defaultTheme()->palette();
}

/*!
    \qmlmethod Component Theme::createStyleComponent(string styleName, object parent)

    Returns an instance of the style component named \a styleName.
*/
QQmlComponent* UCDeprecatedTheme::createStyleComponent(const QString& styleName, QObject* parent)
{
    showDeprecatedNote(parent, "Theme.createStyleComponent() is deprecated. Use ThemeSettings instead.");
    return defaultTheme()->createStyleComponent(styleName, parent, BUILD_VERSION(1, 2));
}

void UCDeprecatedTheme::registerToContext(QQmlContext* context)
{
    // the default theme must be set up prior to this call
    QQmlEngine *engine = context->engine();
    UCTheme *theme = UCTheme::defaultTheme(engine);
    Q_ASSERT(theme);
    // initialize deprecated theme using the default theme from the engine
    UCDeprecatedTheme *oldTheme = new UCDeprecatedTheme(theme);
    QQmlEngine::setContextForObject(oldTheme, context);

    // register deprecated Theme property
    context->setContextProperty("Theme", oldTheme);

    ContextPropertyChangeListener *themeChangeListener =
        new ContextPropertyChangeListener(context, "Theme");
    QObject::connect(oldTheme, SIGNAL(nameChanged()),
                     themeChangeListener, SLOT(updateContextProperty()));
}
