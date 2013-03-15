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
#include "stylecache_p.h"
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>
#include <QtCore/QRegularExpression>

StyleCache::StyleData::StyleData(StyleData *parent) :
    style(0), delegate(0), parent(parent), depth(parent ? parent->depth + 1 : 0)
{}

StyleCache::StyleData::StyleData(const SelectorNode &node, QQmlComponent *style, QQmlComponent *delegate, StyleData *parent) :
    style(style), delegate(delegate), parent(parent), node(node), depth(parent ? parent->depth + 1 : 0)
{}

StyleCache::StyleData::~StyleData()
{
    if (style)
        delete style;
    style = 0;
    if (delegate)
        delete delegate;
    delegate = 0;

    clear();
}

void StyleCache::StyleData::clear()
{
    QHashIterator<SelectorNode, StyleData*> i(children);
    while (i.hasNext()) {
        delete i.next().value();
    }
    children.clear();
}

void StyleCache::StyleData::add(const Selector &path, QQmlComponent *style, QQmlComponent *delegate)
{
    Selector leftover = path;
    SelectorNode nextNode = path.last();
    StyleData *childNode = 0;

    if (!leftover.isEmpty())
        leftover.removeLast();

    childNode = children.value(nextNode);
    if (leftover.isEmpty()) {
        // last or only item in the path, add it as final node
        // check if we have the node already, as it could be part of a previous path that
        // had not yet have a style set
        if (childNode) {
            childNode->style = style;
            childNode->delegate = delegate;
        } else {
            // new leaf
            children.insert(nextNode, new StyleData(nextNode, style, delegate, this));
        }
    } else {
        // check if we have the node in the hash
        if (childNode) {
            childNode->add(leftover, style, delegate);
        } else {
            // new intermediate node
            childNode = new StyleData(nextNode, 0, 0, this);
            children.insert(nextNode, childNode);
            childNode->add(leftover, style, delegate);
        }
    }
}

/*!
 * \internal
 * The method returns the selector path from the current node built using its parent
 * nodes.
 */
Selector StyleCache::StyleData::selector() const
{
    Selector result;

    result << node;

    StyleData *p = parent;
    while (p) {
        if (p->parent)
            result.append(p->node);
        p = p->parent;
    }
    return result;
}

/*!
  \internal
  Search for the style best matching the item path. The path is parsed from the
  tail as the styles are stored in the tree in suffix form. The search is performed
  in the strict or in the loose children list depending on the relationship of
  the tested node. Nodes with direct child descendants and null-compositors are
  in the strict, space compositors are in the loose children list.

  For example we have a style which defines a rule identified by the ".box>.frame .button"
  selector. A styled Item is looking after ".box>.frame>.button" selector,
  but as there is no exact match found for ">.button" node, the algorithm will
  continue to search in the loose list looking after ".button" node, and continue
  the search in its children. This means that the ".box>.frame .button" rule will be
  returned for the ".box>.frame>.button" selector. If the theme would have only
  the ".box .frame .button" rule defined, the lookup would match that. This is not
  valid for the case when the theme has rule for ".box>.frame>.button" selector
  and loking after ".box .frame .button" or ".box>.frame .button" selectors.
*/
StyleCache::MatchResult StyleCache::StyleData::lookup(const Selector &path, StyleData **match, int64_t &matchRank, int64_t searchRank)
{
    // the spare contains the remainder
    Selector leftover = path;
    SelectorNode nextPathNode;
    if (!leftover.isEmpty()) {
        nextPathNode = leftover.last();
        leftover.removeLast();
    } else {
        // there's nothing to do here
        if (style || delegate) {
            return setMatch(match, matchRank, searchRank);
        } else
            return NoMatch;
    }

    // special case: root node forwards the lookup to its children
    if (!parent) {
        return test(nextPathNode, leftover, match, matchRank, searchRank);
    } else {
        if (leftover.isEmpty() && (node == nextPathNode)) {
            if (style || delegate) {
                return setMatch(match, matchRank, searchRank);
            } else
                return NoMatch;
        }
        // check whether we have a child that satisfies the node
        // try to remove elements from the path, maybe we still can get a match
        MatchResult result = NoMatch;
        while (!result) {
            result = test(nextPathNode, leftover, match, matchRank, searchRank);
            if ((result == NoMatch) && !leftover.isEmpty()) {
                nextPathNode = leftover.last();
                leftover.removeLast();
            } else if ((result == NoMatch) && leftover.isEmpty()) {
                // nothing to parse anymore, set rule as this
                if (style || delegate) {
                    result = setMatch(match, matchRank, searchRank);
                } else
                    break;
            }
        }
        return result;
    }
}

/*!
  \internal
  Test whether a child matches the criteria
*/
StyleCache::MatchResult StyleCache::StyleData::test(SelectorNode &nextNode, const Selector &leftover, StyleData **match, int64_t &matchRank, int64_t searchRank)
{
    // depending on the relation in nextNode we check in strict or loose children
    bool strict = nextNode.relation() == SelectorNode::Child;
    MatchResult ret = NoMatch;

    StyleData *childNode = children.value(nextNode);
    if (childNode)
        ret = childNode->lookup(leftover, match, matchRank, searchRank);

    if ((ret == NoMatch) && !nextNode.id().isEmpty() && !parent) {
        // we have style name, check without that; this can be applied only on the first node
        // in the selector
        childNode = children.value(SelectorNode(nextNode, SelectorNode::NoStyleId));
        if (childNode)
            ret = childNode->lookup(leftover, match, matchRank, searchRank);
    }

    if (strict) {
        // try to lookup with loose relation
        nextNode = SelectorNode(nextNode, SelectorNode::NoRelation);
        childNode = children.value(nextNode);
        if (childNode)
            ret = childNode->lookup(leftover, match, matchRank, searchRank);
        if ((ret == NoMatch) && !nextNode.id().isEmpty() && !parent) {
            // same for loose relationship: we have style name, check without that; this can be applied only on the first node
            // in the selector
            childNode = children.value(SelectorNode(nextNode, SelectorNode::NoStyleId));
            if (childNode)
                ret = childNode->lookup(leftover, match, matchRank, searchRank);
        }
    }

    return ret;
}

StyleCache::MatchResult StyleCache::StyleData::setMatch(StyleData **match, int64_t &matchRank, int64_t searchRank)
{
    if (*match && ((*match)->depth > depth))
        // the current match depth (selector length) is higher
        return !(searchRank - matchRank) ? ExactMatch : Match;

    int64_t rank = selector().rank();
    if (*match && ((*match)->depth == depth) && ((searchRank - matchRank) <= (searchRank - rank))) {
        // the previous match has higher rank (is closer to the searched path)
        return !(searchRank - matchRank) ? ExactMatch : Match;
    }
    matchRank = rank;
    *match = this;
    return !(searchRank - matchRank) ? ExactMatch : Match;
}

/*
 * StyleCache, the main class
 *
 */

StyleCache::StyleCache() : enableStyleCache(true), styles(0)
{}

StyleCache::~StyleCache()
{
    clear();
}

/*!
  \internal
  Clears the style tree except its content
  */
void StyleCache::clear()
{
    cache.clear();
    if (styles)
        delete styles;
    styles = 0;
}

/*!
  \internal
  Adds a style rule to the style tree based on the selector path specified.
  */
void StyleCache::addStyleRule(const Selector &path, QQmlComponent *style, QQmlComponent *delegate)
{
    if (!styles)
        styles = new StyleData(0);
    styles->add(path, style, delegate);
}

/*!
  \internal
  Search for the style best matching the item path. The path is parsed from the
  tail as the styles are stored in the tree in suffix form. The \a strict
  parameter specifies whether the search should be strict on the relationship or
  not.
*/
StyleCache::StyleData *StyleCache::match(const Selector &selector)
{
    StyleData *result = 0;

    if (styles) {
        if (enableStyleCache)
            result = cache.value(selector);
        if (!result) {
            int64_t searchRank = selector.rank();
            int64_t matchRank = 0L;
            styles->lookup(selector, &result, matchRank, searchRank);

            if (!result) {
                Selector copy(selector);
                SelectorNode last = selector.last();
                // check if the class wasn't empty and if it was, search with the type
                if (!last.getClass().isEmpty() && !last.type().isEmpty()) {
                    copy.last() = SelectorNode("", last.type(), last.id(), last.relation());
                    styles->lookup(copy, &result, matchRank, searchRank);
                }
            }

            if (result && enableStyleCache)
                cache.insert(selector, result);
        }
    }

    return result;
}
