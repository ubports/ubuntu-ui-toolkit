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

#ifndef SUFFIXTREE_P_H
#define SUFFIXTREE_P_H

#include "rule.h"

// node of a selector
class SelectorNode {
    public:
    enum Relationship {Child, Descendant, Sibling};
    enum NodeSensitivity {
        Normal = 0,
        IgnoreRelationship = 0x01,
        IgnoreStyleId = 0x02,
        IgnoreAll = IgnoreRelationship | IgnoreStyleId};
    SelectorNode();
    SelectorNode(const QString &styleClass, const QString &styleId, Relationship relationship = Descendant, NodeSensitivity sensitivity = Normal);
    QString toString() const;
    bool operator==(const SelectorNode &other);
    QString className;
    QString styleClass;
    QString styleId;
    Relationship relationship;
    int sensitivity;
};

// selector type
typedef QList<SelectorNode> Selector;
uint qHash(const Selector &key);

// style rule tree
class StyleTreeNode {
public:
    StyleTreeNode(StyleTreeNode *parent = 0);
    StyleTreeNode(StyleTreeNode *parent, const SelectorNode &node, Rule *styleRule);
    ~StyleTreeNode();
    void clear();
    void addStyleRule(const Selector &path, Rule *styleRule);
    Rule *lookupStyleRule(const Selector &path, bool strict = false);
    Rule *testNode(SelectorNode &nextNode, const Selector &sparePath, bool &strict);

public:
    StyleTreeNode *parent;
    SelectorNode styleNode;
    Rule *styleRule;
    // the key is the next style node's "relationship class#name" combination
    QHash<QString, StyleTreeNode*> children;
};



#endif // SUFFIXTREE_P_H
