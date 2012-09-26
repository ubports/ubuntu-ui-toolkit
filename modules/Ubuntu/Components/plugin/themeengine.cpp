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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#include "themeengine.h"
#include "style.h"
#include "style_p.h"
#include "styleditem.h"
#include "styleditem_p.h"
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeItem>

#include <QDebug>

Q_GLOBAL_STATIC(ThemeEngine, themeEngine)

static bool themeDebug = false;

/*=============================================================================
=============================================================================*/

StylePathNode::StylePathNode() :
    relationship(Descendant)
{}
StylePathNode::StylePathNode(const QString &styleClass, const QString &styleId, Relationship relationship) :
    styleClass(styleClass), styleId(styleId), relationship(relationship)
{
}

QString StylePathNode::toString() const
{
    QString result;
    if (relationship == StylePathNode::Child)
        result += "> ";
    /* we don't use this...
    else if (relationship == StylePathNode::Sibling)
        result += "+ ";
    */
    if (!styleClass.isEmpty())
        result += "." + styleClass;
    if (!styleId.isEmpty())
        result += "#" + styleId;
    return result;
}


bool StylePathNode::operator==(const StylePathNode &other)
{
    return styleClass == other.styleClass && styleId == other.styleId && relationship == other.relationship;
}

uint qHash(const StylePath &key)
{
    return qHash(themeEngine()->stylePathToString(key));
}


StyleTreeNode::StyleTreeNode(StyleTreeNode *parent) :
    parent(parent), styleNode("", "", StylePathNode::Descendant), styleRule(0)
{
}

StyleTreeNode::StyleTreeNode(StyleTreeNode *parent, const StylePathNode &node, StyleRule *styleRule) :
    parent(parent), styleNode(node), styleRule(styleRule)
{
}

StyleTreeNode::~StyleTreeNode()
{
    clear();
}

void StyleTreeNode::clear()
{
    QHashIterator<QString, StyleTreeNode*> i(children);
    while (i.hasNext()) {
        delete i.next().value();
    }
}


// parse path from the end; assumption is that we always start from the root
void StyleTreeNode::addStyleRule(const StylePath &path, StyleRule *styleRule)
{
    StylePath sparePath = path;
    StylePathNode nextNode = path.last();
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
StyleRule *StyleTreeNode::lookupStyleRule(const StylePath &path, bool strict)
{
    // the spare contains the remainder
    if (themeDebug)
        qDebug() << "enter" << __FUNCTION__ << themeEngine()->stylePathToString(path);
    StylePath sparePath = path;
    StylePathNode nextPathNode;
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
                qDebug() << __FUNCTION__ << "items left in path:" << themeEngine()->stylePathToString(sparePath);
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
StyleRule *StyleTreeNode::testNode(StylePathNode &nextNode, const StylePath &sparePath, bool &strict)
{
    QString nodeKey = nextNode.toString();
    if (themeDebug)
        qDebug() << __FUNCTION__ << nodeKey;
    if (children.contains(nodeKey)) {
        return children.value(nodeKey)->lookupStyleRule(sparePath, strict);
    } else if (!strict && (nextNode.relationship == StylePathNode::Child)){
        // check if the searched node had Child relationship; if yes,
        // change it to Descendant and look after the style again; if
        // found, the lookup after this point should be strict
        nextNode.relationship = StylePathNode::Descendant;
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
=============================================================================*/
void QmlTheme::loadQmlTheme(const QUrl &path, QDeclarativeEngine *engine, StyleTreeNode *styleTree)
{
    themeComponent = new QDeclarativeComponent(engine, path);
    this->styleTree = styleTree;
    if (themeComponent->isLoading())
        QObject::connect(themeComponent, SIGNAL(statusChanged(QDeclarativeComponent::Status)),
                         themeEngine(), SLOT(completeThemeLoading()));
    else
        finalizeThemeLoading();
}

/*!
  \internal
  Finalizes theme loading. Parses theme for style rules and builds up the cache from it.
*/
void QmlTheme::finalizeThemeLoading()
{
    if (!themeComponent->isError()) {
        QObject *theme = themeComponent->create();
        if (theme) {
            // parse its children for Styles
            QList<StyleRule*> rules = theme->findChildren<StyleRule*>();

            foreach (StyleRule *rule, rules) {
                const QString selector = rule->selector();
                if (selector.isEmpty()) {
                    qWarning() << "Rule without selector!";
                    continue;
                }
                QList<StylePath> pathList = themeEngine()->parseSelector(selector);
                foreach (const StylePath &path, pathList) {
                    styleTree->addStyleRule(path, rule);
                }
            }

            styleTree->listTree();
        }
    } else {
        qWarning() << themeComponent->errors();
    }

    // reset component
    themeComponent = 0;
    styleTree = 0;
}

/*=============================================================================
=============================================================================*/

void CssTheme::loadCssTheme(const QString &file)
{

}

/*=============================================================================
=============================================================================*/

ThemeEngine::ThemeEngine(QObject *parent) :
    QObject(parent),
    m_engine(0),
    m_themeComponent(0),
    m_updateTheme(false)
{
    m_styleTree = new StyleTreeNode(0);

    // TODO: watch theme file to detect changes
    m_themeWatcher.addPath("demos/theme.cfg");
    connect(&m_themeWatcher, SIGNAL(fileChanged(QString)), this, SLOT(changeTheme()));
}

ThemeEngine::~ThemeEngine()
{
    // needed by the Q_GLOBAL_STATIC
    delete m_styleTree;
}
bool ThemeEngine::debug()
{
    return themeDebug;
}

void ThemeEngine::setDebug(bool debug)
{
    themeDebug = debug;
}


ThemeEngine* ThemeEngine::instance()
{
    return themeEngine();
}

QDeclarativeEngine *ThemeEngine::engine()
{
    return themeEngine()->m_engine;
}

/*!
  \preliminary
*/
void ThemeEngine::initialize(QDeclarativeEngine *engine)
{
    ThemeEngine *theme = themeEngine();
    theme->m_engine = engine;
    // load default theme, and build hash tree
    //theme->loadTheme(QUrl::fromLocalFile("/usr/lib/qt4/imports/Ubuntu/Components/DefaultTheme.qml"));
    theme->loadTheme(QUrl::fromLocalFile("modules/Ubuntu/Components/DefaultTheme.qml"));
}

/*!
  \preliminary
*/
StyleRule *ThemeEngine::lookupStyleRule(StyledItem *item, bool forceClassName)
{
    ThemeEngine *theme = themeEngine();
    StylePath path = theme->getStylePath(item, forceClassName);

    // check whether we have the path cached
    if (theme->m_styleCache.contains(path)) {
        qDebug() << theme->stylePathToString(path) << "cached";
        return theme->m_styleCache.value(path);
    }

    if (themeDebug) {
        qDebug() << "ThemeEngine::lookupStyleRule- widget path" << theme->stylePathToString(path);
    }
    StyleRule *rule = theme->styleRuleForPath(path);
    if (rule) {
        // cache the rule
        theme->m_styleCache.insert(path, rule);
    }
    return rule;
}

/*!
  \preliminary
  Checks whether the instance can be registerd to the given name, and registers it.
  Removes any previous registration.
*/
bool ThemeEngine::registerInstanceId(StyledItem *item, const QString &newId)
{
    ThemeEngine *engine = themeEngine();
    bool ret = true;

    // check first whether the nex ID is valid and can be registered
    if (newId.isEmpty()) {
        // remove the previous occurence
        engine->m_instanceCache.remove(item->d_ptr->instanceId);
    } else {
        if (engine->m_instanceCache.contains(newId))
            ret = false;
        else {
            // remove the previous occurence
            engine->m_instanceCache.remove(item->d_ptr->instanceId);
            // register instance
            engine->m_instanceCache.insert(newId, item);
        }
    }

    return ret;
}

/*!
  \preliminary
  Loads a QML theme into the system specified by the \a themeFile.
*/
void ThemeEngine::loadTheme(const QUrl &themeFile)
{
    QString themePath = themeFile.path();
    if (themePath.endsWith(".css")) {
        m_cssThemeLoader.loadCssTheme(themePath);
    } else if (themePath.endsWith(".qml")) {
        m_qmlThemeLoader.loadQmlTheme(themeFile, m_engine, m_styleTree);
    } else {
        qWarning() << "Unknown theme URL" << themeFile.toString();
    }
}

/*!
  \preliminary
  Completes loading in case the theme file loading is asynchronous (i.e. the URL
  is on the Web).
*/
void ThemeEngine::completeThemeLoading()
{
    m_qmlThemeLoader.finalizeThemeLoading();
    // emit theme changed signal so StyledItems get updated
    emit themeChanged();
}

/*!
  \preliminary
  Reads the theme descriptor file and changes the theme accordingly
*/
void ThemeEngine::changeTheme()
{
    // TODO: get the theme path and implement based on that
    // Q: should we clean the style tree we have from the previous theme?

    m_styleCache.clear();
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
QList<StylePath> ThemeEngine::parseSelector(const QString &selector) const
{
    QList<StylePath> pathList;
    QStringList groupList = selector.split(",");
    StylePathNode::Relationship nextRelationShip = StylePathNode::Descendant;

    foreach (QString group, groupList) {
        StylePath stylePath;
        QStringList tokens = group.simplified().split(' ');

        foreach (QString token, tokens) {
            if (token.isEmpty() || token == " ")
                continue;
            if (token == ">") {
                nextRelationShip = StylePathNode::Child;
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
                    stylePath.append(StylePathNode(styleClass, styleId, nextRelationShip));
                nextRelationShip = StylePathNode::Descendant;
            }
        }
        pathList.append(stylePath);
    }
    return pathList;
}

QString ThemeEngine::stylePathToString(const StylePath &path) const
{
    QString result;
    foreach (StylePathNode node, path) {
        result += " " + node.toString();
    }
    return result.simplified();
}

/*!
  \internal
  Traverses and returns the path from \a obj up to root as
  a list of styleClass and styleId pairs
  */
StylePath ThemeEngine::getStylePath(const StyledItem *obj, bool forceClassName) const
{
    StylePath stylePath;
    QDeclarativeItem *parent;

    while (obj) {
        QString styleClass = obj->d_ptr->styleClass;
        parent = obj->parentItem();

        // if parent is alsoa StyledItem, we talk about Child relationship
        // otherwise we talk about Descendant
        StylePathNode::Relationship relation = qobject_cast<StyledItem*>(parent) ?
                    StylePathNode::Child :StylePathNode::Descendant;

        // if styleClass is not defined, use the component's meta class name
        if (styleClass.isEmpty() || forceClassName) {
            styleClass = obj->metaObject()->className();
            styleClass = styleClass.left(styleClass.indexOf("_QMLTYPE"));
        }
        QString styleId = obj->d_ptr->instanceId;
        if (!styleClass.isEmpty() || !styleId.isEmpty()) {
            stylePath.prepend(StylePathNode(styleClass, styleId, relation));
        }

        // get the next StyledItem, we don't care the rest
        while (parent && !qobject_cast<StyledItem*>(parent)) {
            parent = parent->parentItem();
        }
        if (!parent)
            break;
        obj = qobject_cast<StyledItem*>(parent);
    }
    return stylePath;
}

/*!
  \internal
  Wrapper function above the style tree lookup. Exposed for functional testing.
*/
StyleRule *ThemeEngine::styleRuleForPath(const StylePath &path)
{
    StyleRule *rule = m_styleTree->lookupStyleRule(path);
    if (themeDebug)
        qDebug() << "lookup path =" << stylePathToString(path) <<
                    ", style rule found:" << ((rule) ? rule->selector() : "");
    return rule;
}
