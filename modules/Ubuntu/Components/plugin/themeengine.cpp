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

#include <QSettings>
#include <QDebug>

Q_GLOBAL_STATIC(ThemeEngine, themeEngine)

static bool themeDebug = false;

const char *themeConfigFile = "demos/theme.cfg";
const char *defaultThemeFile = "modules/Ubuntu/Components/defaulttheme.css";

/*=============================================================================
=============================================================================*/

SelectorNode::SelectorNode() :
    relationship(Descendant)
{}
SelectorNode::SelectorNode(const QString &styleClass, const QString &styleId, Relationship relationship, unsigned char ignore) :
    styleClass(styleClass), styleId(styleId), relationship(relationship), ignore(ignore)
{
}

QString SelectorNode::toString() const
{
    QString result;
    if (((ignore & SELECTOR_IGNORE_RELATIONSHIP) !=  SELECTOR_IGNORE_RELATIONSHIP) &&
            (relationship == SelectorNode::Child))
        result += "> ";
    if (!styleClass.isEmpty())
        result += "." + styleClass;
    if (((ignore & SELECTOR_IGNORE_STYLEID) !=  SELECTOR_IGNORE_STYLEID) && !styleId.isEmpty())
        result += "#" + styleId;
    return result;
}

bool SelectorNode::operator==(const SelectorNode &other)
{
    bool ret = (styleClass == other.styleClass) &&
               ((ignore & SELECTOR_IGNORE_STYLEID) ? true : styleId == other.styleId) &&
               ((ignore & SELECTOR_IGNORE_RELATIONSHIP) ? true : relationship == other.relationship);
    return ret;
}

/*!
  \internal
  Hash key for Selector. Uses the QString's hash function.
  */
uint qHash(const Selector &key)
{
    return qHash(ThemeEnginePrivate::selectorToString(key));
}


StyleTreeNode::StyleTreeNode(StyleTreeNode *parent) :
    parent(parent), styleNode("", "", SelectorNode::Descendant), styleRule(0)
{
}

StyleTreeNode::StyleTreeNode(StyleTreeNode *parent, const SelectorNode &node, StyleRule *styleRule) :
    parent(parent), styleNode(node), styleRule(styleRule)
{
}

StyleTreeNode::~StyleTreeNode()
{
    clear();
}

/*!
  \internal
  Clears the style tree except its content
  */
void StyleTreeNode::clear()
{
    QHashIterator<QString, StyleTreeNode*> i(children);
    while (i.hasNext()) {
        delete i.next().value();
    }
    children.clear();
}

/*!
  \internal
  Adds a style rule to the style tree based on the selector path specified.
  */
void StyleTreeNode::addStyleRule(const Selector &path, StyleRule *styleRule)
{
    Selector sparePath = path;
    SelectorNode nextNode = path.last();
    QString nodeKey = nextNode.toString();

    if (!sparePath.isEmpty())
        sparePath.removeLast();

    if (sparePath.isEmpty()) {
        // last or only item in the path, add it as final node
        // check if we have the node already, as it could be part of a previous path that
        // had not yet have a style set
        if (children.contains(nodeKey)) {
            children.value(nodeKey)->styleRule = styleRule;
        } else {
            // new leaf
            StyleTreeNode * node = new StyleTreeNode(this, nextNode, styleRule);
            children.insert(nodeKey, node);
        }
    } else {
        // check if we have the node in the hash
        if (children.contains(nodeKey)) {
            children.value(nodeKey)->addStyleRule(sparePath, styleRule);
        } else {
            // new node
            StyleTreeNode *node = new StyleTreeNode(this, nextNode, 0);
            children.insert(nodeKey, node);
            node->addStyleRule(sparePath, styleRule);
        }
    }
}

/*!
  \internal
  Search for the style best matching the widget path. The path is parsed from the
  tail as the styles are stored in the tree is stored in sufix form. The \a strict
  parameter specifies whether the search should be strict on the relationship or not.
*/
StyleRule *StyleTreeNode::lookupStyleRule(const Selector &path, bool strict)
{
    // the spare contains the remainder
    if (themeDebug)
        qDebug() << "enter" << __FUNCTION__ << ThemeEnginePrivate::selectorToString(path);
    Selector sparePath = path;
    SelectorNode nextPathNode;
    if (!sparePath.isEmpty()) {
        nextPathNode = sparePath.last();
        sparePath.removeLast();
    }

    // special case: root node forwards the lookup to its children
    if (!parent) {
        return testNode(nextPathNode, sparePath, strict);
    } else {
        // check whether we have a child that satisfies the node
        // try to remove elements from the path, maybe we still can get a match
        while (true) {
            StyleRule *rule = testNode(nextPathNode, sparePath, strict);
            if (rule)
                return rule;
            if (!sparePath.isEmpty()) {
                nextPathNode = sparePath.last();
                sparePath.removeLast();
            } else
                break;
            if (themeDebug)
                qDebug() << __FUNCTION__ << "items left in path:" << ThemeEnginePrivate::selectorToString(sparePath);
        }
    }

    // we have consumed the path, return the style from the node/leaf
    if (themeDebug)
        qDebug() << __FUNCTION__ << "got a style" << styleNode.toString() << styleRule << (styleRule ? styleRule->selector() : QString());
    return styleRule;
}

/*!
  \internal
  Test whether a child matches the criteria
*/
StyleRule *StyleTreeNode::testNode(SelectorNode &nextNode, const Selector &sparePath, bool &strict)
{
    QString nodeKey = nextNode.toString();
    if (themeDebug)
        qDebug() << __FUNCTION__ << nodeKey;
    if (children.contains(nodeKey)) {
        return children.value(nodeKey)->lookupStyleRule(sparePath, strict);
    } else if (!strict && (nextNode.relationship == SelectorNode::Child)){
        // check if the searched node had Child relationship; if yes,
        // change it to Descendant and look after the style again; if
        // found, the lookup after this point should be strict
        nextNode.relationship = SelectorNode::Descendant;
        nodeKey = nextNode.toString();
        if (themeDebug)
            qDebug() << __FUNCTION__ << "no match, testing" << nodeKey;
        strict = true;
        if (children.contains(nodeKey)) {
            return children.value(nodeKey)->lookupStyleRule(sparePath, strict);
        }
    }

    return 0;
}

/*!
  \internal
  For debugging purposes, lists the tree content.
*/
void StyleTreeNode::listTree(const QString &prefix)
{
    if (themeDebug) {
        // go backwards to build the path
        if (styleRule) {
            QString path = '(' + styleNode.toString() + ')';
            for (StyleTreeNode *pl = parent; pl; pl = pl->parent)
                path.append(" (" + pl->styleNode.toString() + ')');
            qDebug() << "node" << prefix << path << ":::" << styleRule->selector();
        }
        QHashIterator<QString, StyleTreeNode*> i(children);
        while (i.hasNext()) {
            i.next();
            i.value()->listTree(prefix + " ");
        }
    }
}

/*=============================================================================
  THEMING ENGINE PRIVATES
=============================================================================*/

ThemeEnginePrivate::ThemeEnginePrivate(ThemeEngine *qq) :
    q_ptr(qq),
    initialized(false),
    m_engine(0),
    m_styleTree(new StyleTreeNode(0))
{
    // TODO: watch theme file to detect changes
    m_themeWatcher.addPath(themeConfigFile);
    QObject::connect(&m_themeWatcher, SIGNAL(fileChanged(QString)), q_ptr, SLOT(_q_updateTheme()));
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
    // get the theme path and implement based on that
    // Q: should we clean the style tree we have from the previous theme?

    // open configuration file
    QSettings settings(themeConfigFile, QSettings::NativeFormat);
    QVariant themeVariant = settings.value("theme");
    QUrl newTheme = themeVariant.toUrl();
    if (!newTheme.isValid()) {
        qWarning() << "WARNING! Theme specified in configuration is invalid. Loading default.";
        //newTheme = QUrl::fromLocalFile("modules/Ubuntu/Components/DefaultTheme.qml");
        newTheme = QUrl::fromLocalFile(defaultThemeFile);
    }

    if (currentTheme != newTheme) {
        currentTheme = newTheme;
        loadTheme(newTheme);

        // in case of error, load the default one
        if (!errorString.isEmpty()) {
            errorString.clear();
            loadTheme(QUrl::fromLocalFile(defaultThemeFile));
        }

    }
}

/*!
  \internal
  Traverses and returns the path from \a obj up to root as
  a list of styleClass and styleId pairs
  */
void ThemeEnginePrivate::loadTheme(const QUrl &themeFile)
{
    QString themePath = themeFile.path();

    if (themePath.endsWith(".css")) {
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
  Traverses and returns the path from \a obj up to root as
  a list of styleClass and styleId pairs
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
                    SelectorNode::Child :SelectorNode::Descendant;

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
    if (!theme->d_ptr->errorString.isEmpty())
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
  The function is used internally by the component plug-in to initialize the
  theming engine. When called configures the engine with the given declarative
  engine and loads the last theme configured. Returns true on successful initialization.
  Further calls of the function do not re-initialize the engine.
*/
bool ThemeEngine::initialize(QDeclarativeEngine *engine)
{
    ThemeEngine *theme = themeEngine();
    if (!theme->d_ptr->initialized) {
        theme->d_ptr->m_engine = engine;

        // set as initialized so we are able to load the theme
        theme->d_ptr->initialized = true;

        // load the tast theme configured
        theme->d_ptr->_q_updateTheme();
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
        qDebug() << "ThemeEnginePrivate::lookupStyleRule- widget path" << ThemeEnginePrivate::selectorToString(path);
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
  Updates the "system theme" configuration file with the given \a theme.
  */
void ThemeEngine::setTheme(const QUrl &theme)
{
    QSettings settings("demos/theme.cfg", QSettings::NativeFormat);
    settings.setValue("theme", theme);
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
  Property getter, returns the current theme file set.
  */
QString ThemeEngine::currentTheme() const
{
    Q_D(const ThemeEngine);
    return d->currentTheme.toString();
}

#include "moc_themeengine.cpp"
