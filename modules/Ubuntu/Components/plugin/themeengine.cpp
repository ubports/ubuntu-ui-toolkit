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

/*=============================================================================
=============================================================================*/

StylePathNode::StylePathNode(const QString &styleClass, const QString &styleId, Relationship relationship) :
    styleClass(styleClass), styleId(styleId), relationship(relationship)
{
}

bool StylePathNode::operator==(const StylePathNode &other)
{
    return styleClass == other.styleClass && styleId == other.styleId && relationship == other.relationship;
}

StyleComponent::StyleComponent(StylePath stylePath, Style *style) :
    stylePath(stylePath), style(style)
{
}

bool StyleComponent::operator==(const StyleComponent &other)
{
    return stylePath == other.stylePath;
}

/*=============================================================================
=============================================================================*/

ThemeEngine::ThemeEngine(QObject *parent) :
    QObject(parent),
    themeComponent(0)
{
}

ThemeEngine::~ThemeEngine()
{
    // needed by the Q_GLOBAL_STATIC
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
    //theme->loadTheme("/usr/lib/qt4/imports/Ubuntu/Components/DefaultTheme.qml");
    theme->loadTheme("modules/Ubuntu/Components/DefaultTheme.qml");
    // TODO: watch theme folders to detect system theme changes
}

Style *ThemeEngine::lookupStyle(StyledItem *item)
{
    ThemeEngine *theme = themeEngine();

    if (theme->m_styleCache.isEmpty())
        return 0;

    StylePath path = theme->getStylePath(item);
    qDebug() << __FUNCTION__ << theme->stylePathToString(path);
    const StyleComponent styleComponent = theme->match(path);

    qDebug() << "Component:" << theme->stylePathToString(path)
             << ", matched style selector:" << theme->stylePathToString(styleComponent.stylePath);
    return styleComponent.style;
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



void ThemeEngine::loadTheme(const QString &themeFile)
{
    themeComponent = new QDeclarativeComponent(m_engine, QUrl::fromLocalFile(themeFile));
    if (themeComponent->isLoading())
        QObject::connect(themeComponent, SIGNAL(statusChanged(QDeclarativeComponent::Status)),
                         this, SLOT(completeThemeLoading()));
    else
        completeThemeLoading();
}

void ThemeEngine::completeThemeLoading()
{
    if (!themeComponent->isError()) {
        QObject *themeObject = themeComponent->create();
        if (themeObject) {
            buildStyleCache(themeObject);

            // DEBUG: print theme cache

            QListIterator<StyleComponent> sh(m_styleCache);
            while (sh.hasNext()) {
                qDebug() << "Style" << stylePathToString(sh.next().stylePath);
            }


        }
    } else {
        qWarning() << themeComponent->errors();
    }

    // resize match list
    m_maybeMatchList.resize(m_styleCache.size());

    // reset component
    themeComponent = 0;
    // emit theme changed signal so StyledItems get updated
    emit themeChanged();
}

// parses theme for style objects and builds up the cache from it
void ThemeEngine::buildStyleCache(QObject *theme)
{
    // parse its children for Styles
    QList<Style*> styles = theme->findChildren<Style*>();

    // the hierarchy is not set well, as all child-styles under other style
    // elements are listed under themeObject; therefore we need to check
    // whether each style element is not having a Style as its parent
    foreach (Style *style, styles) {
        if (qobject_cast<Style*>(style->parent()))
            continue;

        if (style->d_ptr->selector.isEmpty())
            continue;
        //StylePath basePath;
        QList<StylePath> pathList = parseSelector(style, StylePath());
        foreach (const StylePath &path, pathList)
            m_styleCache.append(StyleComponent(path, style));
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
QList<StylePath> ThemeEngine::parseSelector(Style *style, const StylePath &parentPath) const
{
    QList<StylePath> pathList;
    QStringList groupList = style->d_ptr->selector.split(",");
    StylePathNode::Relationship nextRelationShip = StylePathNode::Descendant;

    foreach (QString group, groupList) {
        StylePath stylePath(parentPath);
        QStringList tokens = group.simplified().split(QRegExp("\\b"));

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
        foreach (Style *child, style->d_ptr->data)
            pathList << parseSelector(child, stylePath);
    }
    return pathList;
}

QString ThemeEngine::stylePathToString(const StylePath &path) const
{
    QString result;
    foreach (StylePathNode node, path) {
        if (node.relationship == StylePathNode::Child)
            result += "> ";
        else if (node.relationship == StylePathNode::Sibling)
            result += "+ ";
        if (!node.styleClass.isEmpty())
            result += "." + node.styleClass;
        if (!node.styleId.isEmpty())
            result += "#" + node.styleId;
        result += " ";
    }
    return result.simplified();
}

/**
  Traverses and returns the path from \a obj up to root as
  a list of styleClass and styleId pairs
  */
StylePath ThemeEngine::getStylePath(const StyledItem *obj) const
{
    StylePath stylePath;
    while (obj) {
        QString styleClass = obj->d_ptr->styleClass;

        // if styleClass is not defined, use the component's meta class name
        if (styleClass.isEmpty()) {
            styleClass = obj->metaObject()->className();
            styleClass = styleClass.left(styleClass.indexOf("_QMLTYPE"));
        }
        QString styleId = obj->d_ptr->instanceId;
        if (!styleClass.isEmpty() || !styleId.isEmpty()) {
            stylePath.prepend(StylePathNode(styleClass, styleId, StylePathNode::Descendant));
        }

        // get the next StyledItem, we don't care the rest
        QDeclarativeItem *parent = obj->parentItem();
        while (parent && !qobject_cast<StyledItem*>(parent)) {
            parent = parent->parentItem();
        }
        if (!parent)
            break;
        obj = qobject_cast<StyledItem*>(parent);
    }
    return stylePath;
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
