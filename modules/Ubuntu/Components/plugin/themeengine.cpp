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
#include <QString>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>

#include <QDebug>

/*!
  \qmlclass Theme ThemeEngine
  \brief The Theme element provides functionality to change the current theme.

  Informs the application about errors during theme load through \a error property,
  the current theme file through \a currentTheme property and notifies the widgets
  and application about successful theme change through \a themeChanged() signal.

  The theme can be changed from within the application either through \a loadTheme()
  or through \a setTheme() functions. The difference between these two functions is
  that while loadTheme() simply loads and applies a theme file, setTheme() stores it
  into the application's settings.

  In order to have application theming enabled, applications must define the following
  setting keys in Generic section:
  - UseSystemTheme: a boolean key specifying whether the global theme declares
        theme fo rthe application, and the application is using that theme
  - ThemeFile: a string value specifying the application theme name (not the entire
        file) when the global theme is used, or if not, the private theme file of
        the application

  Not having these keys declared by an application will force the use of the global
  theme. Setting keys should be defined prior the QML files are loaded, and
  applications must define the organization name (domain in Mac OSX) and application
  names as specified in QSettings documentation.
*/

Q_GLOBAL_STATIC(ThemeEngine, themeEngine)

bool themeDebug = false;

/*=============================================================================
=============================================================================*/

/*=============================================================================
  THEMING ENGINE PRIVATES
=============================================================================*/

ThemeEnginePrivate::ThemeEnginePrivate(ThemeEngine *qq) :
    q_ptr(qq),
    initialized(false),
    m_engine(0),
    m_styleTree(new StyleTreeNode(0)),
    themeSettings(qq)
{
}

ThemeEnginePrivate::~ThemeEnginePrivate()
{
    delete m_styleTree;
}

/*!
  \internal
  Completes loading in case the theme file loading is asynchronous (i.e. the URL
  is on the Web).
*/
void ThemeEnginePrivate::_q_continueThemeLoading()
{
    Q_Q(ThemeEngine);
    m_qmlThemeLoader.finalizeThemeLoading();
    // emit theme changed signal so StyledItems get updated
    Q_EMIT q->themeChanged();
}

/*!
  \internal
  Reads the theme descriptor file and changes the theme accordingly
*/
void ThemeEnginePrivate::_q_updateTheme()
{
    const QUrl newTheme = themeSettings.themeFile();

    if (newTheme.isValid() && (currentTheme != newTheme)) {
        // remove previous import paths and add the ones defined for the new theme
        QStringList importList = m_engine->importPathList();
        if (!importPaths.isEmpty()) {
            Q_FOREACH (const QString &import, importPaths)
                importList.removeAll(import);
        }
        importPaths = themeSettings.imports();
        importList << importPaths;
        m_engine->setImportPathList(importList);
        // load the theme
        loadTheme(newTheme);
    }
}

/*!
  \internal
  Initializes engine.
  */
bool ThemeEnginePrivate::initialize(QQmlEngine *engine)
{
    // set as initialized so we are able to load the theme
    initialized = true;
    m_engine = engine;

    // load the tast theme configured
    _q_updateTheme();
    return initialized;
}

/*!
  \internal
  Loads the theme either using QmlTheme or CssTheme classes, depending on the
  thee file type.
  */
void ThemeEnginePrivate::loadTheme(const QUrl &themeFile)
{
    QString themePath = themeFile.path();

    if (themePath.endsWith(".css") || themePath.endsWith(".qthm")) {
        m_styleTree->clear();
        m_styleCache.clear();

        m_cssThemeLoader.loadTheme(themePath, m_engine, m_styleTree);
    } else if (themePath.endsWith(".qml")) {
        m_styleTree->clear();
        m_styleCache.clear();

        m_qmlThemeLoader.loadTheme(themePath, m_engine, m_styleTree);
    } else {
        setError("Unknown theme URL" + themeFile.toString());
    }

    // if no error, set the current theme and Q_EMIT themeChanged signal
    if (errorString.isEmpty()) {
        currentTheme = themeFile;
        Q_Q(ThemeEngine);
        Q_EMIT q->themeChanged();
    }
}

/*!
  \internal
  Traverses and returns the path from \a obj up to root as a list of styleClass
  and instanceId pairs, setting the relationship between the selector nodes
  depending on the relationship between the parent and child, i.e. if a certain
  StyledItem's parent is also a StyledItem, the SelectorNode::Child relation,
  otherwise SelectorNode::Descendant relation is used.
  */
Selector ThemeEnginePrivate::getSelector(const StyledItem *obj, bool forceClassName) const
{
    Selector selector;
    QQuickItem *parent;

    while (obj) {
        QString styleClass = obj->styleClass();
        parent = obj->parentItem();

        // if parent is alsoa StyledItem, we talk about Child relationship
        // otherwise we talk about Descendant
        SelectorNode::Relationship relation = qobject_cast<StyledItem*>(parent) ?
                    SelectorNode::Child : SelectorNode::Descendant;

        // if styleClass is not defined, use the component's meta class name
        if (styleClass.isEmpty() || forceClassName) {
            styleClass = obj->metaObject()->className();
            styleClass = styleClass.left(styleClass.indexOf("_QMLTYPE"));
        }
        QString styleId = obj->instanceId();
        if (!styleClass.isEmpty() || !styleId.isEmpty()) {
            selector.prepend(SelectorNode(styleClass, styleId, relation));
        }

        // get the next StyledItem, we don't care the rest
        while (parent && !qobject_cast<StyledItem*>(parent)) {
            parent = parent->parentItem();
        }
        if (!parent)
            break;
        obj = qobject_cast<StyledItem*>(parent);
    }
    return selector;
}

/*!
  \internal
  Wrapper function above the style tree lookup. Exposed for functional testing.
*/
StyleRule *ThemeEnginePrivate::styleRuleForPath(const Selector &path)
{
    StyleRule *rule = m_styleTree->lookupStyleRule(path);
    if (themeDebug)
        qDebug() << "lookup path =" << selectorToString(path) <<
                    ", style rule found:" << ((rule) ? rule->selector() : "");
    return rule;
}


/*=============================================================================
  Utility functions
=============================================================================*/

/*!
  \internal
  Sets the error string and emits the signal associated to the public property,
  so error handling is propagated to the UI.
  */
void ThemeEnginePrivate::setError(const QString &error)
{
    ThemeEngine *theme = themeEngine();
    theme->d_ptr->errorString = "Theme loading error!\n\t" + error;

    if (themeDebug && !theme->d_ptr->errorString.isEmpty())
        qWarning() << theme->d_ptr->errorString;

    Q_EMIT theme->errorChanged();
}

/*!
  \internal
  Converts a style path back to selector string.
*/
QString ThemeEnginePrivate::selectorToString(const Selector &path)
{
    QString result;
    Q_FOREACH (SelectorNode node, path) {
        result += " " + node.toString();
    }
    return result.simplified();
}

/*!
  \internal
  Parses and returns the path described by \a selector as a list of
  styleClass and styleId pairs.
  Current support (ref: www.w3.org/TR/selector.html):
    - Type selectors, e.g: "Button"
    - Descendant selectors, e.g: "Dialog Button"
    - Child selectors, e.g: "Dialog > Button"
    - ID selectors, e.g: "Button#mySpecialButton"
    - Grouping, e.g: "Button#foo, Checkbox, #bar"
  */
QList<Selector> ThemeEnginePrivate::parseSelector(const QString &selectorString, unsigned char ignore)
{
    QList<Selector> pathList;
    QStringList groupList = selectorString.split(",");
    SelectorNode::Relationship nextRelationShip = SelectorNode::Descendant;

    Q_FOREACH (QString group, groupList) {
        Selector selector;
        QStringList tokens = group.simplified().split(' ');

        Q_FOREACH (QString token, tokens) {
            if (token.isEmpty() || token == " ")
                continue;
            if (token == ">") {
                nextRelationShip = SelectorNode::Child;
            } else {
                QString styleClass;
                QString styleId;
                int idIndex = token.indexOf('#');
                if (idIndex != -1) {
                    styleId = token.mid(idIndex + 1);
                    if (idIndex > 1 && token[0] == '.')
                        styleClass = token.mid(1, idIndex - 1);
                } else if (token[0] == '.') {
                    styleClass = token.mid(1);
                } else
                    styleClass = token;
                if (!styleClass.isEmpty() || !styleId.isEmpty())
                    selector.append(SelectorNode(styleClass, styleId, nextRelationShip, ignore));
                nextRelationShip = SelectorNode::Descendant;
            }
        }
        pathList.append(selector);
    }
    return pathList;
}


/*=============================================================================
  THEMING ENGINE PUBLIC API
=============================================================================*/

ThemeEngine::ThemeEngine(QObject *parent) :
    QObject(parent),
    d_ptr(new ThemeEnginePrivate(this))
{
}

ThemeEngine::~ThemeEngine()
{
    // needed by the Q_GLOBAL_STATIC
}

/*!
  \internal
  The method is used internally by the component plug-in to initialize the
  theming engine. When called configures the engine with the given declarative
  engine and loads the last theme configured in the settings. Returns true on
  successful initialization. Theme loading failure does not affect the success
  of the initialization, however it is reflected in the \a error property.
  Further calls of the function do not re-initialize the engine nor re-load the
  configured theme.
*/
bool ThemeEngine::initialize(QQmlEngine *engine)
{
    ThemeEngine *theme = themeEngine();
    if (!theme->d_ptr->initialized) {
        theme->d_ptr->initialize(engine);
    }
    return theme->d_ptr->initialized;
}

/*!
  \internal
  The method returns the singleton instance of the theme engine.
  */
ThemeEngine *ThemeEngine::instance()
{
    ThemeEngine *ret = themeEngine();
    if (!ret->d_func()->initialized)
        ret = 0;
    return ret;
}

/*!
  \internal
  Checks whether the instance can be registerd to the given name, and registers it.
  Removes any previous registration.
*/
bool ThemeEngine::registerInstanceId(StyledItem *item, const QString &newId)
{
    Q_D(ThemeEngine);
    bool ret = true;

    // check first whether the nex ID is valid and can be registered
    if (newId.isEmpty()) {
        // remove the previous occurence
        d->m_instanceCache.remove(item->instanceId());
    } else {
        if (d->m_instanceCache.contains(newId))
            ret = false;
        else {
            // remove the previous occurence
            d->m_instanceCache.remove(item->instanceId());
            // register instance
            d->m_instanceCache.insert(newId, item);
        }
    }

    return ret;
}

/*!
  \internal
  This method searches for a Rule element that matches the conditions for a
  StyledItem. The selector searched is built up by traversing the \a item
  parents and considering only StyledItem elements in the hierarchy.
  */
StyleRule *ThemeEngine::lookupStyleRule(StyledItem *item, bool forceClassName)
{
    Q_D(ThemeEngine);
    if (!d->initialized)
        return 0;

    Selector path = d->getSelector(item, forceClassName);

    // check whether we have the path cached
    if (d->m_styleCache.contains(path)) {
        return d->m_styleCache.value(path);
    }

    if (themeDebug) {
        qDebug() << "ThemeEnginePrivate::lookupStyleRule - widget path" << ThemeEnginePrivate::selectorToString(path);
    }
    StyleRule *rule = d->styleRuleForPath(path);
    if (rule) {
        // cache the rule
        d->m_styleCache.insert(path, rule);
    }
    return rule;
}

/*!
  Loads a theme file from any location, and updates the \a currentTheme property
  on success. The ocurred errors are reported in \a error property.
  */
bool ThemeEngine::loadTheme(const QUrl &themeFile)
{
    Q_D(ThemeEngine);
    d->loadTheme(themeFile);
    return d->errorString.isEmpty();
}

/*!
  The function sets the theme file in the application's settings and loads the
  theme.

  Its parameters depend on what is about to be set.
  - if the theme is global, then the \a theme specifies the theme folder as specified
    in the global theme; if the \a theme is empty, the global theme's default will
    be set and loaded (e.g. TestApp from Ambiance theme will lead to
    /usr/shared/theme/Ambiance/qthm/TestApp/theme.qthm URL returned)
  - if the theme is local (\a global set to false), the \a theme contains the full
    path to the theme file to be used.

  Returns the path to the theme file loaded or an invalid URL on failure. The \a error
  will contain the error occurred.
  */
bool ThemeEngine::setTheme(const QString &theme, bool global)
{
    Q_D(ThemeEngine);
    QUrl themeFile = d->themeSettings.setTheme(theme, global);
    if (themeFile.isValid())
        d->_q_updateTheme();
    else if (d->errorString.isEmpty()) {
        d->setError(QString("Error setting theme %1 as %2")
                    .arg(theme)
                    .arg(global ? "global" : "private"));
    }
    return d->errorString.isEmpty();
}

/*!
  \qmlproperty string Theme::error
  The property contains the error occurred upon loading. The error is not cleared
  automatically and must be acknowledged after each operation.
  */
QString ThemeEngine::error() const
{
    Q_D(const ThemeEngine);
    return d->errorString;
}

/*!
  Property reset method for \a error property.
  */
void ThemeEngine::resetError()
{
    Q_D(ThemeEngine);
    d->errorString = QString();
}


/*!
  \qmlproperty string Theme::currentTheme
  This property holds the current theme file loaded.
  */
QString ThemeEngine::currentTheme() const
{
    Q_D(const ThemeEngine);
    return d->currentTheme.toString();
}

#include "moc_themeengine.cpp"
