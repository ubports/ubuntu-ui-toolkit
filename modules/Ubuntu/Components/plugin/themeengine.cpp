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
#include "rule.h"
#include "itemstyleattached.h"
#include "themeloader_p.h"
#include "qmlthemeloader_p.h"
#include "qmlloader_p.h"
#include <QtCore/QString>
#include <QtQml/QQmlEngine>
#include <QtQml/QJSEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>

/*!
  \qmltype Theme
  \inqmlmodule Ubuntu.Components 0.1
  \ingroup theming
  \brief The Theme element provides functionality to change the current theme.

  Informs the application about the current theme file used through \a currentTheme
  property and notifies the items and application about successful theme change
  through \a themeChanged() signal. When error occurs during theme loading, it is
  reported through \a error property.
*/

ThemeEngine *ThemeEnginePrivate::themeEngine = 0;

/*=============================================================================
  THEMING ENGINE PRIVATES
=============================================================================*/

ThemeEnginePrivate::ThemeEnginePrivate(ThemeEngine *qq) :
    q_ptr(qq),
    m_engine(qobject_cast<QQmlEngine*>(qq->parent())),
    m_styleTree(new StyleTreeNode(0)),
    firstThemeLoaded(false)
{
    themeEngine = q_ptr;

    // register theme loaders
    // TODO: shouldn't these be in separate plugins?
    themeLoaders[".qml"] = new QmlLoader(m_engine);
    themeLoaders[".qmltheme"] = new QmlThemeLoader(m_engine);

    // connect theme settings to capture theme updates
    QObject::connect(&themeSettings, SIGNAL(themeSettingsChanged()), q_ptr, SLOT(_q_updateTheme()));
}

ThemeEnginePrivate::~ThemeEnginePrivate()
{
    delete m_styleTree;
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
        if (!importPaths.isEmpty()) {
            importList << importPaths;
            m_engine->setImportPathList(importList);
        }
        // load the theme
        loadTheme(newTheme);
    }
}

/*!
  \internal
  Loads the theme using the registered theme file format loaders. Aliased theme
  files stored in the resources should contain the file format, otherwise the
  engine cannot detect the file format.
  */
void ThemeEnginePrivate::loadTheme(const QUrl &themeFile)
{
    QUrl url(themeFile);
    if (!url.isValid())
        url = themeSettings.themeFile();

    QString fileType = url.path();
    fileType = fileType.right(fileType.length() - fileType.lastIndexOf('.'));

    if (themeLoaders.contains(fileType)) {
        StyleTreeNode *styleTree = themeLoaders.value(fileType)->loadTheme(url);
        if (errorString.isEmpty() && styleTree) {
            // clear the previous style and use the loaded one
            delete m_styleTree;
            m_styleCache.clear();
            m_styleTree = styleTree;
            // emit themeChanged() to update style
            currentTheme = url;
            Q_Q(ThemeEngine);
            Q_EMIT q->themeChanged();
        } else {
            delete styleTree;
            // continue using the previous style
        }
    } else {
        setError("Unknown theme URL" + url.toString());
    }
}

/*!
  \internal
  Traverses and returns the path from \a obj up to root as a list of class
  and name pairs, setting the relationship between the selector nodes
  depending on the relationship between the parent and child, i.e. if a certain
  ItemStyleAttached's parent is also a ItemStyleAttached, the SelectorNode::Child
  relation, otherwise SelectorNode::Descendant relation is used.

  The obj is an Item derived element and should have class and name properties
  to be used if styling happens on them.
  */
Selector ThemeEnginePrivate::getSelector(QQuickItem *obj, bool forceClassName) const
{
    Selector selector;
    QQuickItem *parent;

    while (obj) {
        ItemStyleAttached *style = attachedStyle(obj);
        QString styleClass = style ? style->styleClass() : QString();

        parent = obj->parentItem();

        // we talk about Child relationship when the parent has styling properties
        // otherwise we talk about Descendant
        ItemStyleAttached *parentStyle = attachedStyle(parent);
        SelectorNode::Relationship relation = parentStyle ?
                    SelectorNode::Child : SelectorNode::Descendant;

        // if class is not defined, use the component's meta class name
        if (styleClass.isEmpty() || forceClassName) {
            styleClass = obj->metaObject()->className();
            styleClass = styleClass.left(styleClass.indexOf("_QMLTYPE"));
        }
        QString styleId = style->name();
        if (!styleClass.isEmpty() || !styleId.isEmpty()) {
            selector.prepend(SelectorNode(styleClass, styleId, relation));
        }

        // get the next ItemStyleAttached, we don't care the rest
        while (parent && !parentStyle) {
            parent = parent->parentItem();
            parentStyle = attachedStyle(parent);
        }
        obj = parent;
    }
    return selector;
}

/*!
  \internal
  Wrapper function above the style tree lookup. Exposed for functional testing.
*/
Rule *ThemeEnginePrivate::styleRuleForPath(const Selector &path)
{
    if (!m_styleTree)
        return 0;
    Rule *rule = m_styleTree->lookupStyleRule(path);
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
    themeEngine->d_ptr->errorString = error;
    Q_EMIT themeEngine->errorChanged();
}

/*!
  \internal
  Returns the style propertyies object attached to an item.
  */
ItemStyleAttached *ThemeEnginePrivate::attachedStyle(QObject *obj)
{
    if (!obj)
        return 0;
    QObject *attached = qmlAttachedPropertiesObject<ItemStyleAttached>(obj, false);
    return qobject_cast<ItemStyleAttached*>(attached);
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
  class and name pairs.
  Current support (ref: www.w3.org/TR/selector.html):
    - Type selectors, e.g: "Button"
    - Descendant selectors, e.g: "Dialog Button"
    - Child selectors, e.g: "Dialog > Button"
    - ID selectors, e.g: "Button#mySpecialButton"
    - Grouping, e.g: "Button#foo, Checkbox, #bar"
  */
QList<Selector> ThemeEnginePrivate::parseSelector(const QString &selectorString, SelectorNode::NodeSensitivity sensitivity)
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
                    selector.append(SelectorNode(styleClass, styleId, nextRelationShip, sensitivity));
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

/*!
  \preliminary
  \qmlsignal Theme::themeChanged()
  Emitted when the theme file gets loaded with success.
*/

ThemeEngine::ThemeEngine(QObject *parent) :
    QObject(parent),
    d_ptr(new ThemeEnginePrivate(this))
{
}

/*!
  \internal
  The method is used by the QML framework to register theme engine instance.

  When called it configures the engine with the given declarative engine and loads
  the last theme configured in the settings. Returns the theme engine's instance
  on successful initialization. Theme loading failure does not affect the success
  of the initialization, however it is reflected in the \a error property.
  Further calls of the function do not re-initialize the engine nor re-load the
  configured theme.
*/
QObject *ThemeEngine::initializeEngine(QQmlEngine *engine)
{
    if (!ThemeEnginePrivate::themeEngine && engine) {
        ThemeEnginePrivate::themeEngine = new ThemeEngine(engine);
    }
    return ThemeEnginePrivate::themeEngine;
}

/*!
  \internal
  The method returns the singleton instance of the theme engine.
  */
ThemeEngine *ThemeEngine::instance()
{
    ThemeEngine *ret = ThemeEnginePrivate::themeEngine;
    if (ret && !ret->d_ptr->firstThemeLoaded) {
        ret->d_ptr->firstThemeLoaded = true;
        ret->d_ptr->_q_updateTheme();
    }
    return ret;
}

/*!
  \internal
  Checks whether the instance can be registered to the given name, and registers it.
  Removes any previous registration.
*/
bool ThemeEngine::registerName(QQuickItem *item, const QString &newName)
{
    Q_D(ThemeEngine);
    bool ret = true;

    // check first whether the next ID is valid and can be registered
    QString prevName(item->property("name").toString());
    if (newName.isEmpty()) {
        // remove the previous occurence
        if (!prevName.isEmpty())
            d->m_instanceCache.remove(prevName);
    } else {
        if (d->m_instanceCache.contains(newName))
            ret = false;
        else {
            // remove the previous occurence
            if (!prevName.isEmpty())
                d->m_instanceCache.remove(prevName);
            // register instance
            d->m_instanceCache.insert(newName, item);
        }
    }

    return ret;
}

/*!
  \internal
  This method searches for a Rule element that matches the conditions for an
  Item. The selector identifying the Rule is built up by traversing the \a item
  parents and considering only those having ItemStyle elements attached in the hierarchy.
  */
Rule *ThemeEngine::lookupStyleRule(QQuickItem *item, bool forceClassName)
{
    Q_D(ThemeEngine);

    Selector path = d->getSelector(item, forceClassName);

    // check whether we have the path cached
    if (d->m_styleCache.contains(path)) {
        return d->m_styleCache.value(path);
    }
    Rule *rule = d->styleRuleForPath(path);
    if (rule) {
        // cache the rule
        d->m_styleCache.insert(path, rule);
    }
    return rule;
}

/*!
  \preliminary
  \qmlmethod bool Theme::loadTheme(const QUrl &themeFile)
  Loads a theme file from any location, and updates the \a currentTheme property
  on success. Any error messages are reported in the \a error property.
  */
bool ThemeEngine::loadTheme(const QUrl &themeFile)
{
    Q_D(ThemeEngine);
    d->errorString = QString();
    d->loadTheme(themeFile);
    return d->errorString.isEmpty();
}

/*!
  \qmlproperty string Theme::error
  The property contains the error, which occurred upon loading. The error is not cleared
  automatically and must be acknowledged after each operation.
  */
QString ThemeEngine::error() const
{
    Q_D(const ThemeEngine);
    return d->errorString;
}

/*!
  Property reset method for the \a error property.
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
QString ThemeEngine::theme() const
{
    Q_D(const ThemeEngine);
    return d->currentTheme.toString();
}

#include "moc_themeengine.cpp"
