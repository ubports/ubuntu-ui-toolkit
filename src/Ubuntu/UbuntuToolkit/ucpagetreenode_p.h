/*
 * Copyright 2015 Canonical Ltd.
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
#ifndef UCPAGETREENODE_P_H
#define UCPAGETREENODE_P_H

#include "ucstyleditembase_p.h"

namespace UbuntuToolkit {

class UCPageTreeNode;
class UCPageTreeNodePrivate : public UCStyledItemBasePrivate
{
    Q_DECLARE_PUBLIC(UCPageTreeNode)

public:
    UCPageTreeNodePrivate ();
    void init ();
    void updatePageTree ();
    UCPageTreeNode *getParentPageTreeNode ();

    enum PropertyFlags {
        FirstFlag            = 0x01,
        CustomPropagated     = FirstFlag,
        CustomActive         = FirstFlag << 1,
        CustomPageStack      = FirstFlag << 2,
        LastPageTreeNodeFlag = FirstFlag << 8
    };

    struct Node {
        UCPageTreeNode *m_node = 0;
        QList<Node> m_children;
    };

    void _q_activeBinding (bool active);
    void _q_pageStackBinding (QQuickItem *pageStack);
    void _q_propagatedBinding (QObject *propagated);
    void updateParentLeafNode ();
    void dumpNodeTree ();
    void initActive();
    void initPageStack();
    void initPropagated();

private:
    void dumpNode(const Node &n, const QString &oldDepth = QString(), const QString &depth = QString(), bool isRoot = true);

public:
    UCPageTreeNode *m_parentNode;
    QQuickItem* m_activeLeafNode;
    QQuickItem* m_pageStack;
    QObject* m_propagated;
    QQuickItem* m_toolbar;
    qint8 m_flags;
    bool m_isLeaf:1;
    bool m_active:1;
};

UT_NAMESPACE_END

#endif // UCPAGETREENODE_P_H
