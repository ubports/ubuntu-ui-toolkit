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
#include "suffixtree_p.h"
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>

/*
  This file contains the Rule-element suffix-tree handling classes. The suffix-tree
  contains all the rules defined for selectors in suffix format, meaning the selectors
  are parsed and stored from the end to the beginning. Each node in the tree collects
  its children nodes in a hash table, where the key is the selector node converted to
  a string.
*/

SelectorNode::SelectorNode() :
    relationship(Descendant)
{}
/*!
    \internal
    Creates an instance of a SelectorNode with a given class, name
    and relationship. The sensitivity parameter configures the node so that during
    string conversion and comparison ignores the relationship, the name
    both or none. This feature is used when building up QmlTheme selectorTable.
*/
SelectorNode::SelectorNode(const QString &styleClass, const QString &styleId, Relationship relationship, NodeSensitivity sensitivity) :
    styleClass(styleClass.toLower()), styleId(styleId.toLower()), relationship(relationship), sensitivity(sensitivity)
{
}

/*!
    \internal
    Converts a SelectorNode into string using "<relation> .<class>#<name>"
    format. Depending on the sensitivity set, may ignore the relationship and styleId.
  */
QString SelectorNode::toString() const
{
    QString result;
    if (((sensitivity & IgnoreRelationship) !=  IgnoreRelationship) &&
            (relationship == SelectorNode::Child))
        result += "> ";
    if (!styleClass.isEmpty())
        result += "." + styleClass;
    if (((sensitivity & IgnoreStyleId) !=  IgnoreStyleId) && !styleId.isEmpty())
        result += "#" + styleId;
    return result;
}

bool SelectorNode::operator==(const SelectorNode &other)
{
    bool ret = (styleClass == other.styleClass) &&
               (((sensitivity & IgnoreStyleId) ==  IgnoreStyleId) ? true : styleId == other.styleId) &&
               (((sensitivity & IgnoreRelationship) ==  IgnoreRelationship) ? true : relationship == other.relationship);
    return ret;
}

/*!
  \internal
  Hash key for Selector. Uses QString's hash function.
  */
uint qHash(const Selector &key)
{
    return qHash(ThemeEnginePrivate::selectorToString(key));
}


StyleTreeNode::StyleTreeNode(StyleTreeNode *parent) :
    parent(parent), styleNode("", "", SelectorNode::Descendant), styleRule(0)
{
}

StyleTreeNode::StyleTreeNode(StyleTreeNode *parent, const SelectorNode &node, Rule *styleRule) :
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
void StyleTreeNode::addStyleRule(const Selector &path, Rule *styleRule)
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
  Search for the style best matching the item path. The path is parsed from the
  tail as the styles are stored in the tree in suffix form. The \a strict
  parameter specifies whether the search should be strict on the relationship or
  not.

  For example we have a style which defines a rule identified by the ".box > .frame .button"
  selector. A styled Item is looking after ".box > .frame > .button" selector,
  but as there is no exact match found for "> .button" node, as the strictness
  is loose the algorithm will ignore the relationship and start looking after the
  ".button" node, and continue the search in its children by no longer ignoring
  the relationship. This means that the ".box > .frame .button" rule will be
  returned for the ".box > .frame > .button" Item. If the theme would have only
  the ".box .frame .button" rule defined, the lookup would not match that rule.
*/
Rule *StyleTreeNode::lookupStyleRule(const Selector &path, bool strict)
{
    // the spare contains the remainder
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
            Rule *rule = testNode(nextPathNode, sparePath, strict);
            if (rule)
                return rule;
            if (!sparePath.isEmpty()) {
                nextPathNode = sparePath.last();
                sparePath.removeLast();
            } else
                break;
        }
    }

    // we have consumed the path, return the style from the node/leaf
    return styleRule;
}

/*!
  \internal
  Test whether a child matches the criteria
*/
Rule *StyleTreeNode::testNode(SelectorNode &nextNode, const Selector &sparePath, bool &strict)
{
    Rule *rule = 0;
    QString nodeKey = nextNode.toString();
    if (children.contains(nodeKey)) {
        rule = children.value(nodeKey)->lookupStyleRule(sparePath, strict);
    }
    //if (!rule && !strict && (nextNode.relationship == SelectorNode::Child)) {
    if (!rule && !strict) {
        // check if we find something without the style name
        if (!nextNode.styleId.isEmpty())
            nextNode.styleId = QString();
        nodeKey = nextNode.toString();
        strict = true;
        if (children.contains(nodeKey)) {
            rule = children.value(nodeKey)->lookupStyleRule(sparePath, strict);
        }
        if (!rule && (nextNode.relationship == SelectorNode::Child)) {
            // check if the searched node had Child relationship; if yes,
            // change it to Descendant and look after the style again; if
            // found, the lookup after this point should be strict
            nextNode.relationship = SelectorNode::Descendant;
            nodeKey = nextNode.toString();
            strict = true;
            if (children.contains(nodeKey)) {
                rule = children.value(nodeKey)->lookupStyleRule(sparePath, strict);
            }
        }
    }

    return rule;
}
