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

#include <QDebug>

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
    emit q->themeChanged();
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
            foreach (const QString &import, importPaths)
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
bool ThemeEnginePrivate::initialize(QDeclarativeEngine *engine)
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

        m_cssThemeLoader.loadCssTheme(themePath, m_engine, m_styleTree);
    } else if (themePath.endsWith(".qml")) {
        m_styleTree->clear();
        m_styleCache.clear();

        m_qmlThemeLoader.loadQmlTheme(themePath, m_engine, m_styleTree);
    } else {
        setError("Unknown theme URL" + themeFile.toString());
    }

    // if no error, set the current theme and emit themeChanged signal
    if (errorString.isEmpty()) {
        currentTheme = themeFile;
        Q_Q(ThemeEngine);
        emit q->themeChanged();
    }
}

/*!
  \internal
  Traverses and returns the path from \a obj up to root as a list of styleClass
  and instanceId pairs
  */
Selector ThemeEnginePrivate::getSelector(const StyledItem *obj, bool forceClassName) const
{
    Selector selector;
    QDeclarativeItem *parent;

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

    emit theme->errorChanged();
}

/*!
  \internal
  Converts a style path back to selector string.
*/
QString ThemeEnginePrivate::selectorToString(const Selector &path)
{
    QString result;
    foreach (SelectorNode node, path) {
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

    foreach (QString group, groupList) {
        Selector selector;
        QStringList tokens = group.simplified().split(' ');

        foreach (QString token, tokens) {
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
  \preliminary
  The method is used internally by the component plug-in to initialize the
  theming engine. When called configures the engine with the given declarative
  engine and loads the last theme configured in the settings. Returns true on
  successful initialization. Theme loading failure does not affect the success
  of the initialization, however it is reflected in the \a error property.
  Further calls of the function do not re-initialize the engine nor re-load the
  configured theme.
*/
bool ThemeEngine::initialize(QDeclarativeEngine *engine)
{
    ThemeEngine *theme = themeEngine();
    if (!theme->d_ptr->initialized) {
        theme->d_ptr->initialize(engine);
    }
    return theme->d_ptr->initialized;
}

/*!
  \preliminary
  */
ThemeEngine *ThemeEngine::instance()
{
    ThemeEngine *ret = themeEngine();
    if (!ret->d_func()->initialized)
        ret = 0;
    return ret;
}

/*!
  \preliminary
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
  \preliminary
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
  \preliminary
  */
void ThemeEngine::loadTheme(const QUrl &themeFile)
{
    Q_D(ThemeEngine);
    d->loadTheme(themeFile);
}

/*!
  \preliminary
  This is a helper slot provided for testing purposes and should not be considered
  as part of the final API.
  Loads the \a theme file and stores it in the application's settings if possible.
  If application theme can be stored in settings, it will
  */
void ThemeEngine::setTheme(const QUrl &theme, bool global)
{
    Q_D(ThemeEngine);
    QUrl themeFile = d->themeSettings.setThemeFile(theme, global);
    if (themeFile.isValid())
        d->_q_updateTheme();
    else if (d->errorString.isEmpty()) {
        d->setError(QString("Error setting theme %1 as %2")
                    .arg(theme.toString())
                    .arg(global ? "global" : "private"));
    }
}

/*!
  \internal
  Property getter, returns the error string set by the engine upon loading.
  */
QString ThemeEngine::error() const
{
    Q_D(const ThemeEngine);
    return d->errorString;
}

/*!
  \internal
  Property reset method for \a error property.
  */
void ThemeEngine::resetError()
{
    Q_D(ThemeEngine);
    d->errorString = QString();
}


/*!
  \internal
  Property getter, returns the current theme file set.
  */
QString ThemeEngine::currentTheme() const
{
    Q_D(const ThemeEngine);
    return d->currentTheme.toString();
}

#include "moc_themeengine.cpp"
