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

#include "uctheme.h"
#include "listener.h"
#include "quickutils.h"
#include "i18n.h"
#include "ucfontutils.h"
#include "ucstyleditembase_p.h"

#include <QtQml/qqml.h>
#include <QtQml/qqmlinfo.h>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QLibraryInfo>
#include <QtCore/QStandardPaths>
#include <QtGui/QGuiApplication>
#include <QtGui/QFont>

#include <QtQml/private/qqmlproperty_p.h>
#include <QtQml/private/qqmlabstractbinding_p.h>
#define foreach Q_FOREACH
#include <QtQml/private/qqmlbinding_p.h>
#undef foreach

/*!
    \qmltype ThemeSettings
    \instantiates UCTheme
    \inqmlmodule Ubuntu.Components 1.3
    \since Ubuntu.Components 1.3
    \ingroup theming
    \brief The ThemeSettings class provides facilities to define the theme of a
    StyledItem.

    A global instance is exposed as the \b theme context property.

    The theme or theme defines the visual aspect of the Ubuntu components. An
    application can use one or more theme the same time. The ThemeSettings component
    provides abilities to change thye theme used by the component and all its
    child components.

    Changing the theme of the entire application can be achieved by changing
    the name of the root StyledItem's, i.e. MainView's current theme.

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    MainWindow {
        width: units.gu(40)
        height: units.gu(71)

        theme.name: "Ubuntu.Components.Themes.Ambiance"
    }
    \endqml
    \note Changing the style set name in this way will result in a change of the
    inherited style set. In case a different style set is desired, a new instance
    of the ThemeSettings must be created.

    The \l createStyleComponent function can be used to create the style for a
    component. The following example will create the style with the inherited
    style set.
    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3
    StyledItem {
        id: myItem
        style: theme.createStyleComponent("MyItemStyle.qml", myItem)
    }
    \endqml

    When declared, the ThemeSettings's name points to the system defined theme. There
    can be cases when the parent defined style set is needed but with small modifications.
    In these situations the \l parent property can be used to get the parent
    style set, and so the name can be bound to the parent's name.
    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3
    StyledItem {
        id: myItem
        theme: ThemeSettings {
            name: parent ? parent.name : undefined
        }
        style: theme.createStyleComponent("MyItemStyle.qml", myItem)
    }
    \endqml
    \note Observe the way the name is set to \c undefined when the parent is not
    defined. Setting \c undefined to name will reset the property to the system
    theme defined one.

    \sa {StyledItem}
*/

const QString THEME_FOLDER_FORMAT("%1/%2/");
const QString PARENT_THEME_FILE("parent_theme");

QStringList themeSearchPath() {
    QString envPath = QLatin1String(getenv("UBUNTU_UI_TOOLKIT_THEMES_PATH"));
    QStringList pathList = envPath.split(':', QString::SkipEmptyParts);
    if (pathList.isEmpty()) {
        // get the default path list from generic data location, which contains
        // XDG_DATA_DIRS
        QString xdgDirs = QLatin1String(getenv("XDG_DATA_DIRS"));
        if (!xdgDirs.isEmpty()) {
            pathList << xdgDirs.split(':', QString::SkipEmptyParts);
        }
        // ~/.local/share
        pathList << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    }

    // append QML import path(s); we must explicitly support env override here
    QString qml2ImportPath(getenv("QML2_IMPORT_PATH"));
    if (!qml2ImportPath.isEmpty()) {
        pathList << qml2ImportPath.split(':', QString::SkipEmptyParts);
    }
    pathList << QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath).split(':', QString::SkipEmptyParts);

    // fix folders
    QStringList result;
    Q_FOREACH(const QString &path, pathList) {
        if (QDir(path).exists()) {
            result << path + '/';
        }
    }
    // prepend current folder
    result.prepend(QDir::currentPath());
    return result;
}

QUrl pathFromThemeName(QString themeName)
{
    themeName.replace('.', '/');
    QStringList pathList = themeSearchPath();
    Q_FOREACH(const QString &path, pathList) {
        QString themeFolder = THEME_FOLDER_FORMAT.arg(path, themeName);
        // QUrl needs a trailing slash to understand it's a directory
        QString absoluteThemeFolder = QDir(themeFolder).absolutePath().append('/');
        if (QDir(absoluteThemeFolder).exists()) {
            return QUrl::fromLocalFile(absoluteThemeFolder);
        }
    }
    return QUrl();
}

QString parentThemeName(const QString& themeName)
{
    QString parentTheme;
    QUrl themePath = pathFromThemeName(themeName);
    if (!themePath.isValid()) {
        qWarning() << qPrintable(UbuntuI18n::instance().tr("Theme not found: \"%1\"").arg(themeName));
    } else {
        QFile file(themePath.resolved(PARENT_THEME_FILE).toLocalFile());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            parentTheme = in.readLine();
        }
    }
    return parentTheme;
}

UCTheme::UCTheme(QObject *parent)
    : QObject(parent)
    , m_palette(UCTheme::defaultTheme().m_palette)
    , m_engine(UCTheme::defaultTheme().m_engine)
    , m_defaultStyle(false)
{
    init();
}

UCTheme::UCTheme(bool defaultStyle, QObject *parent)
    : QObject(parent)
    , m_palette(NULL)
    , m_engine(NULL)
    , m_defaultStyle(defaultStyle)
{
    init();
    // set the default font
    QFont defaultFont;
    defaultFont.setFamily("Ubuntu");
    defaultFont.setPixelSize(UCFontUtils::instance().sizeToPixels("medium"));
    defaultFont.setWeight(QFont::Light);
    QGuiApplication::setFont(defaultFont);
}

void UCTheme::init()
{
    m_completed = false;
    QObject::connect(&m_defaultTheme, &UCDefaultTheme::themeNameChanged,
                     this, &UCTheme::onThemeNameChanged);
    updateThemePaths();
}

void UCTheme::classBegin()
{
    m_engine = qmlEngine(this);
    updateEnginePaths();
}

void UCTheme::updateEnginePaths()
{
    if (!m_engine) {
        return;
    }

    QStringList paths = themeSearchPath();
    Q_FOREACH(const QString &path, paths) {
        if (QDir(path).exists() && !m_engine->importPathList().contains(path)) {
            m_engine->addImportPath(path);
        }
    }
}

// slot called when the ThemeSettings uses the system theme
void UCTheme::onThemeNameChanged()
{
    updateThemePaths();
    Q_EMIT nameChanged();
}

void UCTheme::updateThemePaths()
{
    m_themePaths.clear();

    QString themeName = name();
    while (!themeName.isEmpty()) {
        QUrl themePath = pathFromThemeName(themeName);
        if (themePath.isValid()) {
            m_themePaths.append(themePath);
        }
        themeName = parentThemeName(themeName);
    }
}

/*!
 * \qmlproperty ThemeSettings ThemeSettings::parentTheme
 * The property specifies the parent ThemeSettings instance. The property only
 * has a valid value when assigned to \l StyledItem::theme property.
 */
UCTheme *UCTheme::parentTheme()
{
    UCStyledItemBase *owner = qobject_cast<UCStyledItemBase*>(parent());
    UCStyledItemBasePrivate *pOwner = owner ? UCStyledItemBasePrivate::get(owner) : NULL;
    if (pOwner && pOwner->theme == this && pOwner->parentStyledItem) {
        return UCStyledItemBasePrivate::get(pOwner->parentStyledItem)->getTheme();
    }
    return NULL;
}

/*!
    \qmlproperty string ThemeSettings::name

    The name of the current theme. The name can be set only at creation time, runtime
    changes will be omitted.

    \qml
    import QtQuick 2.4
    import Ubuntu.Componenst 1.3

    StyledItem {
        style: ThemeSettings {
            // this is right
            name: "Ubuntu.Components.Themes.Ambiance"
        }
        // this is not allowed, and will be omitted
        Components.onCompleted: theme.name = "Ubuntu.Components.Themes.SuruDark"
    }
    \endqml
*/
QString UCTheme::name() const
{
    return !m_name.isEmpty() ? m_name : m_defaultTheme.themeName();
}
void UCTheme::setName(const QString& name)
{
    if (name == m_name) {
        return;
    }
    m_name = name;
    if (name.isEmpty()) {
        init();
    } else {
        QObject::disconnect(&m_defaultTheme, &UCDefaultTheme::themeNameChanged,
                            this, &UCTheme::onThemeNameChanged);
        updateThemePaths();
    }
    updateEnginePaths();
    loadPalette();
    Q_EMIT nameChanged();
}
void UCTheme::resetName()
{
    setName(QString());
}

/*!
    \qmlproperty Palette ThemeSettings::palette

    The palette of the current theme.
*/
QObject* UCTheme::palette()
{
    if (!m_palette) {
        loadPalette(false);
    }
    return m_palette;
}

/*!
 * \qmlproperty Palette ThemeSettings::paletteConfiguration
 * \default
 */
QObject *UCTheme::paletteConfiguration() const
{
    return m_paletteConfiguration;
}
void UCTheme::setPaletteConfiguration(QObject *config)
{
    if (m_paletteConfiguration == config) {
        return;
    }
    if (m_paletteConfiguration && config) {
        qmlInfo(config) << UbuntuI18n::instance().tr("ThemeSettings can have only one Palette instance to configure the palette.");
        return;
    }
    if (config && !QuickUtils::inherits(config, "Palette")) {
        qmlInfo(config) << UbuntuI18n::instance().tr("Not a Palette component.");
        return;
    }
    m_paletteConfiguration = config;
    if (!m_paletteConfiguration) {
        loadPalette();
    } else {
        // take ownership over the Palette set
        m_paletteConfiguration->setParent(this);
        // if not complete yet, we must wait till it gets completed
        // apply palette changes on the theme palette
        connect(m_paletteConfiguration, SIGNAL(__completed()), this, SLOT(_q_configurePalette()), Qt::DirectConnection);
        _q_configurePalette();
    }
    Q_EMIT paletteConfigurationChanged();
}

// applies a palette configuration
void UCTheme::applyPaletteConfiguration(const QString &valueSet)
{
    if (!m_palette || !m_paletteConfiguration) {
        return;
    }

    QObject *configObject = m_paletteConfiguration->property(valueSet.toUtf8()).value<QObject*>();
    const QMetaObject *mo = configObject->metaObject();
    QQmlContext *configContext = qmlContext(m_paletteConfiguration);

    // we must detect the validity of the palette properties one by one
    for (int i = mo->propertyOffset(); i < mo->propertyCount(); i++) {
        const QMetaProperty prop = mo->property(i);
        QString propertyName = QString("%1.%2").arg(valueSet).arg(prop.name());
        QQmlProperty configProperty(m_paletteConfiguration, propertyName, configContext);
        QQmlProperty paletteProperty(m_palette, propertyName, qmlContext(m_palette));

        // is it a binding?
        QQmlAbstractBinding *binding = QQmlPropertyPrivate::binding(configProperty);
        if (binding) {
            if (binding->bindingType() == QQmlAbstractBinding::Binding) {
                QQmlBinding *qmlBinding = static_cast<QQmlBinding*>(binding);
                qmlBinding->setTarget(paletteProperty);
            }
            QQmlAbstractBinding *prev = QQmlPropertyPrivate::setBinding(paletteProperty, binding);
            if (prev && prev != binding) {
                prev->destroy();
            }
        } else {
            QColor color = configProperty.read().value<QColor>();
            if (color.isValid()) {
                // do not use QQmlProperty for this, setting the property in this way seems to be faster
                paletteProperty.write(configProperty.read());
            }
        }
    }
}

QUrl UCTheme::styleUrl(const QString& styleName)
{
    Q_FOREACH (const QUrl& themePath, m_themePaths) {
        QUrl styleUrl = themePath.resolved(styleName);
        if (styleUrl.isValid() && QFile::exists(styleUrl.toLocalFile())) {
            return styleUrl;
        }
    }

    return QUrl();
}

// registers the default theme property to the root context
void UCTheme::registerToContext(QQmlContext* context)
{
    UCTheme *defaultTheme = &UCTheme::defaultTheme();
    defaultTheme->m_engine = context->engine();
    defaultTheme->updateEnginePaths();

    context->setContextProperty("theme", defaultTheme);
    ContextPropertyChangeListener *listener =
        new ContextPropertyChangeListener(context, "theme");
    QObject::connect(defaultTheme, &UCTheme::nameChanged,
                     listener, &ContextPropertyChangeListener::updateContextProperty);
}

/*!
    \qmlmethod Component ThemeSettings::createStyleComponent(string styleName, object parent)

    Returns an instance of the style component named \a styleName and parented
    to \a parent.
*/
QQmlComponent* UCTheme::createStyleComponent(const QString& styleName, QObject* parent)
{
    QQmlComponent *component = NULL;

    if (parent != NULL) {
        QQmlEngine* engine = qmlEngine(parent);
        if (engine != m_engine && !m_engine) {
            m_engine = engine;
            updateEnginePaths();
        }
        // make sure we have the paths
        if (engine != NULL) {
            QUrl url = styleUrl(styleName);
            if (url.isValid()) {
                component = new QQmlComponent(engine, url, QQmlComponent::PreferSynchronous, parent);
                if (component->isError()) {
                    qmlInfo(parent) << component->errorString();
                    delete component;
                    component = NULL;
                }
            } else {
                qmlInfo(parent) <<
                   UbuntuI18n::instance().tr(QString("Warning: Style %1 not found in theme %2").arg(styleName).arg(name()));
            }
        }
    }

    return component;
}

void UCTheme::loadPalette(bool notify)
{
    if (!m_engine) {
        return;
    }
    if (!m_palette.isNull()) {
        delete m_palette;
    }
    // theme may not have palette defined
    QUrl paletteUrl = styleUrl("Palette.qml");
    if (paletteUrl.isValid()) {
        m_palette = QuickUtils::instance().createQmlObject(paletteUrl, m_engine);
        _q_configurePalette(false);
        if (notify) {
            Q_EMIT paletteChanged();
        }
    } else {
        // use the default palette if none defined
        m_palette = defaultTheme().m_palette;
    }
}

void UCTheme::_q_configurePalette(bool notify)
{
    if (!m_paletteConfiguration || (sender() && !m_paletteConfiguration->property("__ready").toBool())) {
        return;
    }
    applyPaletteConfiguration("normal");
    applyPaletteConfiguration("selected");
    if (notify) {
        Q_EMIT paletteChanged();
    }
}

// returns the palette color value of a color profile
QColor UCTheme::getPaletteColor(const char *profile, const char *color)
{
    QColor result;
    if (m_palette) {
        QObject *paletteProfile = m_palette->property(profile).value<QObject*>();
        if (paletteProfile) {
            result = paletteProfile->property(color).value<QColor>();
        }
    }
    return result;
}

