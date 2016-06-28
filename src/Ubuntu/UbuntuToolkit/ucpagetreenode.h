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
#ifndef UCPAGETREENODE_H
#define UCPAGETREENODE_H

#include "ucstyleditembase.h"

namespace UbuntuToolkit {

class UCPageTreeNodePrivate;
class UBUNTUTOOLKIT_EXPORT UCPageTreeNode : public UCStyledItemBase
{
    Q_OBJECT
    Q_PROPERTY(bool isLeaf READ isLeaf WRITE setIsLeaf NOTIFY isLeafChanged)
    Q_PROPERTY(QQuickItem* toolbar READ toolbar WRITE setToolbar NOTIFY toolbarChanged)
    Q_PROPERTY(QQuickItem* activeLeafNode READ activeLeafNode NOTIFY activeLeafNodeChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
#ifndef Q_QDOC
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCPageTreeNode)* parentNode READ parentNode WRITE setParentNode NOTIFY parentNodeChanged)
#else
    Q_PROPERTY(UCPageTreeNode* parentNode READ parentNode WRITE setParentNode NOTIFY parentNodeChanged)
#endif
    Q_PROPERTY(QQuickItem* pageStack READ pageStack WRITE setPageStack NOTIFY pageStackChanged)

    Q_PROPERTY(bool __isPageTreeNode READ isPageTreeNode NOTIFY isPageTreeNodeChanged)
    Q_PROPERTY(QObject* __propagated READ propagated WRITE setPropagated NOTIFY propagatedChanged)
public:
    UCPageTreeNode(QQuickItem *parent = 0);

    void setIsLeaf(bool isLeaf);
    bool isLeaf() const;

    void setParentNode(UCPageTreeNode *parentNode);
    UCPageTreeNode *parentNode() const;

    virtual void setActive(bool active);
    bool active() const;

    void setPageStack(QQuickItem *pageStack);
    QQuickItem *pageStack() const;

    QQuickItem *activeLeafNode() const;

    QObject *propagated() const;
    void setPropagated(QObject *propagated);
    void resetPropagated();

    QQuickItem *toolbar() const;
    void setToolbar(QQuickItem *toolbar);

    bool isPageTreeNode() const;

    // QQmlParserStatus interface
    void componentComplete() override;

    Q_INVOKABLE void dumpNodeTree ();

Q_SIGNALS:
    void isLeafChanged(bool isLeaf);
    void activeChanged(bool active);
#ifndef Q_QDOC
    void parentNodeChanged(UT_PREPEND_NAMESPACE(UCPageTreeNode) *parentNode);
#else
    void parentNodeChanged(UCPageTreeNode *parentNode);
#endif
    void pageStackChanged(QQuickItem* pageStack);
    void activeLeafNodeChanged(QQuickItem* activeLeafNode);
    void propagatedChanged(QObject* propagated);
    void toolbarChanged(QQuickItem* toolbar);
    void isPageTreeNodeChanged(bool isPageTreeNode);

protected:
    UCPageTreeNode(UCPageTreeNodePrivate &, QQuickItem *parent);

    void setActiveLeafNode(QQuickItem* activeLeafNode);

    // QQuickItem interface
    void itemChange(ItemChange change, const ItemChangeData &value) override;

private:
    Q_DECLARE_PRIVATE(UCPageTreeNode)
    Q_PRIVATE_SLOT(d_func(), void _q_activeBinding(bool active))
    Q_PRIVATE_SLOT(d_func(), void _q_pageStackBinding (QQuickItem *pageStack))
    Q_PRIVATE_SLOT(d_func(), void _q_propagatedBinding (QObject *propagated))
};

UT_NAMESPACE_END

#endif // UCPAGETREENODE_H
