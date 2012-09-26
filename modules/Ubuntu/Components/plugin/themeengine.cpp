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
    else if (relationship == StylePathNode::Sibling)
        result += "+ ";
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

StyleComponent::StyleComponent(StylePath stylePath, StyleRule *style) :
    stylePath(stylePath), style(style)
{
}

bool StyleComponent::operator==(const StyleComponent &other)
{
    return stylePath == other.stylePath;
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


void ThemeEngine::initialize(QDeclarativeEngine *engine)
{
    ThemeEngine *theme = themeEngine();
    theme->m_engine = engine;
    // load default theme, and build hash tree
    //theme->loadTheme(QUrl::fromLocalFile("/usr/lib/qt4/imports/Ubuntu/Components/DefaultTheme.qml"));
    theme->loadTheme(QUrl::fromLocalFile("modules/Ubuntu/Components/DefaultTheme.qml"));
}

StyleRule *ThemeEngine::lookupStyleRule(StyledItem *item, bool forceClassName)
{
    ThemeEngine *theme = themeEngine();

    /*
    if (theme->m_styleCache.isEmpty())
        return 0;

    StylePath path = theme->getStylePath(item, forceClassName);
    const StyleComponent styleComponent = theme->match(path);
    if (theme->m_debug) {
        qDebug() << "Component:" << theme->stylePathToString(path)
                 << ", matched style selector:" << theme->stylePathToString(styleComponent.stylePath);
    }
    return styleComponent.style;
*/
    StylePath path = theme->getStylePath(item, forceClassName);
    if (themeDebug) {
        qDebug() << "ThemeEngine::lookupStyleRule- widget path" << theme->stylePathToString(path);
    }
    return theme->styleRuleForPath(path);
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
    m_themeComponent = new QDeclarativeComponent(m_engine, themeFile);
    if (m_themeComponent->isLoading())
        QObject::connect(m_themeComponent, SIGNAL(statusChanged(QDeclarativeComponent::Status)),
                         this, SLOT(completeThemeLoading()));
    else
        completeThemeLoading();
}

/*!
  \internal
  Completes loading in case the theme file loading si asynchronous (i.e. the URL
  is on the Web.
*/
void ThemeEngine::completeThemeLoading()
{
    if (!m_themeComponent->isError()) {
        QObject *themeObject = m_themeComponent->create();
        if (themeObject) {
            buildStyleCache(themeObject);

            // DEBUG: print theme cache
            /*
            if (themeDebug) {
                QListIterator<StyleComponent> sh(m_styleCache);
                while (sh.hasNext()) {
                    qDebug() << "Style" << stylePathToString(sh.next().stylePath);
                }
            }
            */
            m_styleTree->listTree();
        }
    } else {
        qWarning() << m_themeComponent->errors();
    }

    // resize match list
    m_maybeMatchList.resize(m_styleCache.size());

    // reset component
    m_themeComponent = 0;
    // emit theme changed signal so StyledItems get updated
    emit themeChanged();
}

/*!
  \internal
  Reads the theme descriptor file and changes the theme accordingly
*/
void ThemeEngine::changeTheme()
{
    // TODO: get the theme path and implement based on that
    // Q: should we clean the style tree we have from the previous theme?
}

/*!
  \internal
  Parses theme for style rules and builds up the cache from it.
*/
void ThemeEngine::buildStyleCache(QObject *theme)
{
    // parse its children for Styles
    QList<StyleRule*> styles = theme->findChildren<StyleRule*>();

    // check whether the team contains a resetCache property that woruls cause
    // resetting of teh current cache
    bool resetCache = theme->property("resetCache").toBool();
    if (resetCache) {
        //m_styleCache.clear();
        delete m_styleTree;
        m_styleTree = new StyleTreeNode(0);
    }

    // check whether the theme imports any other theme than the default one
    // the default theme import is not needed as that one is loaded automatically
    // by the engine - WE MAY REMOVE THIS
    QUrl importTheme = theme->property("include").toUrl();
    if (importTheme.isValid() && !importTheme.isEmpty()) {
        // load the theme as an increment
        m_updateTheme = true;
        loadTheme(importTheme);
    }

    // the rule hierarchy is not set well, as all child-rules under a rule
    // are listed under the theme's roo tobject; therefore we need to check
    // whether the rule element we work on doesn't have Rule as its parent
    foreach (StyleRule *style, styles) {
        if (qobject_cast<StyleRule*>(style->parent()))
            continue;

        if (style->d_ptr->selector.isEmpty())
            continue;
        //StylePath basePath;
        QList<StylePath> pathList = parseSelector(style, StylePath());
        foreach (const StylePath &path, pathList) {
            //m_styleCache.append(StyleComponent(path, style));
            m_styleTree->addStyleRule(path, style);
        }
    }
}

/**
  Parses and returns the path described by \a selector as a list of
  styleClass and styleId pairs.
  Current support (ref: www.w3.org/TR/selector.html):
    - Type selectors, e.g: "Button"
    - Descendant selectors, e.g: "Dialog Button"
    - Child selectors, e.g: "Dialog > Button"
    - ID selectors, e.g: "Button#mySpecialButton"
    - Grouping, e.g: "Button#foo, Checkbox, #bar"
  */
QList<StylePath> ThemeEngine::parseSelector(StyleRule *styleRule, const StylePath &parentPath) const
{
    QList<StylePath> pathList;
    QStringList groupList = styleRule->d_ptr->selector.split(",");
    StylePathNode::Relationship nextRelationShip = StylePathNode::Descendant;

    foreach (QString group, groupList) {
        StylePath stylePath(parentPath);
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
        // check its children to the selector
        foreach (QObject *child, styleRule->d_ptr->data) {
            StyleRule *cs = qobject_cast<StyleRule*>(child);
            if (cs)
                pathList << parseSelector(cs, stylePath);
        }
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

/**
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
        qDebug() << "test: lookup path =" << stylePathToString(path) <<
                    ", style rule found:" << ((rule) ? rule->selector() : "");
    return rule;
}

/**
  Tries to match \a path against the style paths found from parsing
  the style selectors
  */
StyleComponent ThemeEngine::match(const StylePath &srcStylePath)
{
    const int styleMapSize = m_styleCache.size();
    int bestMatchIndex = -1;
    // _maybeMatchList contains, for each selector in the style, the number of currently
    // matched path nodes in srcStylePath. The count will increase for the selectors as we
    // start matching, and the one that ends up with the highest count would normally be
    // the best.
    m_maybeMatchList.fill(0);

    int firstMapIndex = 0;
    int lastMapIndex = styleMapSize - 1;
    int lastMapIndexSoFar = 0;
    bool firstMapIndexFound = false;

    // Starting from the end of the path, traverse all path nodes in srcStylePath
    // and match them to the path nodes found in the style map:
    for (int pathIndex=0; pathIndex<srcStylePath.size(); ++pathIndex) {
        const StylePathNode srcNode = srcStylePath[srcStylePath.size() - pathIndex - 1];

        int pendingMatchesInRange = lastMapIndex - firstMapIndex + 1;
        for (int mapIndex=firstMapIndex; mapIndex<=lastMapIndex; ++mapIndex) {
            if (m_maybeMatchList[mapIndex] < 0) {
                // The current selector is already marked as done, meaning it
                // is not pending anymore, and we can bail out early. If the count
                // reaches zero, then _all_ of the remaining pending selectors have
                // been marked as done, and we can return:
                if (--pendingMatchesInRange == 0)
                    return bestMatchIndex == -1 ? StyleComponent(StylePath(), 0) : m_styleCache[bestMatchIndex];
                continue;
            }

            const StyleComponent styleComponent = m_styleCache[mapIndex];
            const StylePath stylePath = styleComponent.stylePath;
            const StylePathNode node = stylePath[stylePath.size() - m_maybeMatchList[mapIndex] - 1];
            const bool classMatch = node.styleClass.isEmpty() || (node.styleClass == srcNode.styleClass);
            const bool idMatch = node.styleId.isEmpty() || (node.styleId == srcNode.styleId);

            if (classMatch && idMatch) {
                if (pathIndex == 0) {
                    // First round. Find the range where the styleClass we're looking
                    // for is defined to avoid traversing the whole map for each iteration:
                    // Todo: put this range in a hash lookup for the styleClass up front.
                    lastMapIndexSoFar = mapIndex;
                    if (!firstMapIndexFound) {
                        firstMapIndexFound = true;
                        firstMapIndex = mapIndex;
                    }
                }

                if (pathIndex < stylePath.size() - 1) {
                    // Still matching, but more nodes to check. Continue to next iteration.
                    ++m_maybeMatchList[mapIndex];
                } else {
                    // INVARIANT: current selector is traversed to the end, and is a match.
                    // If it is the best possible match remains to see, but start by comparing
                    // it against the one we (might) already got:
                    if (bestMatchIndex == -1) {
                        bestMatchIndex = mapIndex;
                    } else if (m_maybeMatchList[mapIndex] > -m_maybeMatchList[bestMatchIndex]-1) {
                        // Longer matched paths are better than shorter paths:
                        bestMatchIndex = mapIndex;
                    } else if (!node.styleClass.isEmpty() && !node.styleId.isEmpty()) {
                        // If both class and id are specified, style selectors defined
                        // later in the list (qml file) are better then the ones on top:
                        bestMatchIndex = mapIndex;
                    } else {
                        // Id match is evaluated higher than class match:
                        StylePath p = m_styleCache[bestMatchIndex].stylePath;
                        if (p[p.size() - pathIndex - 1].styleId.isEmpty())
                            bestMatchIndex = mapIndex;
                    }

                    // Mark as done:
                    m_maybeMatchList[mapIndex] = -m_maybeMatchList[mapIndex] - 1;
                }
                continue;
            }

            if (pathIndex > 0) {
                const StylePathNode prevNode = stylePath[stylePath.size() - m_maybeMatchList[mapIndex]];
                if (prevNode.relationship == StylePathNode::Descendant) {
                    // The style node didnt match the current source node, but
                    // we still see it as a possible match since the previous node
                    // accepts any matching anchestor, which might show up later.
                    continue;
                }
            }

            // Current styleComponent is no longer considered a possible match. We
            // mark it as done by making the traverse value negative, and subtract
            // -1 to avoid confusion in case it was 0 from before:
            m_maybeMatchList[mapIndex] = -m_maybeMatchList[mapIndex] - 1;
        }
        lastMapIndex = lastMapIndexSoFar;
    }

    return bestMatchIndex == -1 ? StyleComponent(StylePath(), 0) : m_styleCache[bestMatchIndex];
}
