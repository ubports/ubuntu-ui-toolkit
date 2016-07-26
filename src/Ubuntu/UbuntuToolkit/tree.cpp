/*
 * Copyright 2016 Canonical Ltd.
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
 */

#include "tree_p.h"

#include <private/qobject_p.h>
#include <QQmlEngine>

UT_NAMESPACE_BEGIN

class TreePrivate : public QObjectPrivate
{
public:
    // list<object> of nodes
    QList<QObject*> m_nodes;
    // list<int> of parent nodes
    QList<QObject*> m_parents;
    // list<int> of stems
    QList<int> m_stems;
};

Tree::Tree(QObject *parent) :
    QObject((*new TreePrivate), parent)
{

}

// Return the index of the given node.
// Returns -1 the node was not found.
int Tree::index(QObject *node) const
{
    return d_func()->m_nodes.indexOf(node);
}

// Add newNode to the tree in the specified stem, with the specified parent node.
// The root node should be added with parentNode null. All other nodes must be added
// with a parentNode that is already in the tree.
bool Tree::add(const int stem, QObject *parentNode, QObject *newNode)
{
    Q_D(Tree);

    if (index(newNode) != -1) {
        qWarning("Cannot add the same node twice to a tree.");
        return false;
    }
    if (d->m_nodes.size() == 0) {
        // adding root node
        if (parentNode != nullptr) {
            qWarning("Root node must have parentNode null.");
            return false;
        }
    } else {
        // adding non-root node
        if (parentNode == nullptr) {
            qWarning("Only root node has parentNode null.");
            return false;
        }
        if (index(parentNode) == -1) {
            qWarning("Cannot add non-root node if parentNode is not in the tree.");
            return false;
        }
    }

    d->m_nodes.push_back(newNode);
    d->m_stems.push_back(stem);
    d->m_parents.push_back(parentNode);
    return true;
}

// Remove all nodes from the specified stem and higher stems.
//
// Returns the removed nodes.
QList<QObject *> Tree::prune(const int stem)
{
    Q_D(Tree);
    QList<QObject *> newNodes;
    QList<QObject *> newParents;
    QList<QObject *> removedNodes;
    QList<int> newStems;

    for (int i = 0; i < d->m_nodes.length(); i++) {
        if (d->m_stems[i] < stem) {
            newNodes.push_back(d->m_nodes.at(i));
            newStems.push_back(d->m_stems.at(i));
            newParents.push_back(d->m_parents.at(i));
        } else  {
            removedNodes.push_back(d->m_nodes.at(i));
        }
    }

    d->m_nodes = newNodes;
    d->m_parents = newParents;
    d->m_stems = newStems;
    return removedNodes;
}

// Chops all nodes with an index higher than the given node which
//  are in the same stem or a higher stem.
//
// If, and only if, (inclusive) then also chop the given node.
//
// Default values for node and inclusive are top() and true.
// Returns a list that contains the nodes that were chopped.
QList<QObject *> Tree::chop(const QVariant &jsNode, const QVariant &jsInclusive)
{
    Q_D(Tree);

    QObject *node = nullptr;

    if (!jsNode.isValid())
        node = top();
    else
        node = jsNode.value<QObject *>();

    bool inclusive = true;
    if (jsInclusive.isValid() && jsInclusive.canConvert<bool>())
        inclusive = jsInclusive.toBool();

    int nodeIndex = index(node);
    if (nodeIndex < 0) {
        // given node is not in the tree.
        return QList<QObject *>();
    }

    int size = 0;
    if (inclusive) {
        size = nodeIndex;
    } else {
        size = nodeIndex + 1;
    }

    // Nodes with index(node) >= nodeIndex && stem >= stems[nodeIndex];
    QList<QObject *> badNodes; // to fill below

    // Nodes with index(node) >= nodeIndex (any stem).
    // Potential bad nodes to be removed.
    QList<QObject *> uglyNodes = d->m_nodes.mid(size);
    QList<QObject *> uglyParents = d->m_parents.mid(size);
    QList<int> uglyStems = d->m_stems.mid(size); // to check below

    int stem = d->m_stems[nodeIndex];
    // Good nodes, with index(node) < nodeIndex && stem < stems[nodeIndex]:
    d->m_nodes = d->m_nodes.mid(0, size);
    d->m_stems = d->m_stems.mid(0, size);
    d->m_parents = d->m_parents.mid(0, size);

    // Add nodes with index(node) > nodeIndex && stem < stems[nodeIndex] back:
    for (int i = 0; i < uglyNodes.length(); i++) {
        if (uglyStems.at(i) < stem) {
            // Because the stem of the parentNode <= stem of the node,
            //  the node that is added back has the parentNode in nodes.
            d->m_nodes.push_back(uglyNodes.at(i));
            d->m_stems.push_back(uglyStems.at(i));
            d->m_parents.push_back(uglyParents.at(i));
        } else {
            badNodes.push_back(uglyNodes.at(i));
        }
    }
    return badNodes;

}

// Returns the n'th node when traversing one or more stems from the
//  top down. When exactMatch, only the specified stem is traversed, and
//  when !exactMatch the specified stem and all higher stems are traversed.
//
// Returns null if no matching node was found.
//
// Default value for stem: 0
// Default value for exactMatch: false
// Default value for n: 0 (first node)
//
// Calling top() with no parameters returns top(0, false, 0) which is the
//  last node that was added to the tree.
QObject *Tree::top(const QVariant &jsStem, const QVariant &jsExactMatch, const QVariant &jsN) const
{
    const Q_D(Tree);

    int stem = 0;
    bool exactMatch = false;
    int n = 0;

    //@FIXME remove this part once we do not need to export Tree to JS anymore
    if (jsStem.isValid() && jsStem.canConvert<int>())
        stem = jsStem.value<int>();
    if (jsExactMatch.isValid() && jsExactMatch.canConvert<bool>())
        exactMatch = jsExactMatch.toBool();
    if (jsN.isValid() && jsN.canConvert<int>())
        n = jsN.value<int>();

    int st;
    int count = n;
    for (int i = d->m_nodes.size() - 1; i >= 0; i--) {
        st = d->m_stems[i];
        if ((exactMatch && st == stem) || (!exactMatch && st >= stem)) {
            count--;
        }
        if (count < 0) {
            return d->m_nodes.at(i);
        }
    }
    return nullptr;
}

// Return the parent node of the specified node in the tree
QObject *Tree::parent(QObject *node) const
{
    const Q_D(Tree);

    int i = index(node);
    if (i == -1 //Specified node not found in tree.
        || i == 0 ) { //Root node has no parent node.
        return nullptr;
    }
    return d->m_parents.at(i);
}

UT_NAMESPACE_END
