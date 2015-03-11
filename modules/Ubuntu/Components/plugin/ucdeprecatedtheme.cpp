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
 *
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucdeprecatedtheme.h"
#include "ucstyleset.h"
#include "listener.h"
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlInfo>

/*!
    \qmltype Theme
    \instantiates UCStyleSet
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
UCDeprecatedTheme::UCDeprecatedTheme(QObject *parent)
    : QObject(parent)
{
    connect(&UCStyleSet::defaultSet(), &UCStyleSet::nameChanged,
            this, &UCDeprecatedTheme::nameChanged);
    connect(&UCStyleSet::defaultSet(), &UCStyleSet::paletteChanged,
            this, &UCDeprecatedTheme::paletteChanged);
}

void UCDeprecatedTheme::showDeprecatedNote(const char *note)
{
    QByteArray suppressNote = qgetenv("SUPPRESS_DEPRECATED_NOTE");
    if (suppressNote.isEmpty() || suppressNote != "yes") {
        qmlInfo(this) << note;
    }
}

/*!
    \qmlproperty string Theme::name

    The name of the current theme.
*/
QString UCDeprecatedTheme::name()
{
    showDeprecatedNote("Theme.name is deprecated. Use StyleSet instead.");
    return UCStyleSet::defaultSet().name();
}
void UCDeprecatedTheme::setName(const QString& name)
{
    showDeprecatedNote("Theme.name is deprecated. Use StyleSet instead.");
    UCStyleSet::defaultSet().setName(name);
}
void UCDeprecatedTheme::resetName()
{
    showDeprecatedNote("Theme.name is deprecated. Use StyleSet instead.");
    UCStyleSet::defaultSet().resetName();
}

/*!
    \qmlproperty Palette Theme::palette

    The palette of the current theme.
*/
QObject* UCDeprecatedTheme::palette()
{
    showDeprecatedNote("Theme.palette is deprecated. Use StyleSet instead.");
    return UCStyleSet::defaultSet().palette();
}

/*!
    \qmlmethod Component Theme::createStyleComponent(string styleName, object parent)

    Returns an instance of the style component named \a styleName.
*/
QQmlComponent* UCDeprecatedTheme::createStyleComponent(const QString& styleName, QObject* parent)
{
    showDeprecatedNote("Theme.createStyleComponent() is deprecated. Use StyleSet instead.");
    return UCStyleSet::defaultSet().createStyleComponent(styleName, parent);
}

void UCDeprecatedTheme::registerToContext(QQmlContext* context)
{
    UCStyleSet::defaultSet().m_engine = context->engine();
    UCStyleSet::defaultSet().updateEnginePaths();
    // register deprecated Theme property
    context->setContextProperty("Theme", this);

    ContextPropertyChangeListener *themeChangeListener =
        new ContextPropertyChangeListener(context, "Theme");
    QObject::connect(this, SIGNAL(nameChanged()),
                     themeChangeListener, SLOT(updateContextProperty()));
}
