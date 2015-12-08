#ifndef UCPAGETREENODE_H
#define UCPAGETREENODE_H

#include "ucstyleditembase.h"

class UCPageTreeNodePrivate;
class UCPageTreeNode : public UCStyledItemBase
{
    Q_OBJECT
    Q_PROPERTY(bool __isPageTreeNode READ isPageTreeNode NOTIFY isPageTreeNodeChanged)
    Q_PROPERTY(QQuickItem* toolbar READ toolbar WRITE setToolbar NOTIFY toolbarChanged)
    Q_PROPERTY(QObject* __propagated READ propagated WRITE setPropagated NOTIFY propagatedChanged RESET resetPropagated)
    Q_PROPERTY(bool active READ active WRITE setActive RESET resetActive NOTIFY activeChanged)
    Q_PROPERTY(bool isLeaf READ isLeaf WRITE setIsLeaf NOTIFY isLeafChanged)
    Q_PROPERTY(UCPageTreeNode* parentNode READ parentNode WRITE setParentNode NOTIFY parentNodeChanged)
    Q_PROPERTY(QQuickItem* activeLeafNode READ activeLeafNode WRITE setActiveLeafNode NOTIFY activeLeafNodeChanged)
    Q_PROPERTY(QQuickItem* pageStack READ pageStack WRITE setPageStack RESET resetPageStack NOTIFY pageStackChanged)
public:
    UCPageTreeNode(QQuickItem *parent = 0);

    void setIsLeaf(bool isLeaf);
    bool isLeaf() const;

    void setParentNode(UCPageTreeNode *parentNode);
    UCPageTreeNode *parentNode() const;

    void setActive(bool active);
    bool active() const;
    void resetActive ();

    void setPageStack(QQuickItem *pageStack);
    QQuickItem *pageStack() const;
    void resetPageStack ();

    QQuickItem *activeLeafNode() const;
    void setActiveLeafNode(QQuickItem* activeLeafNode);

    // QQmlParserStatus interface
    virtual void componentComplete() override;

    QObject *propagated() const;
    void setPropagated(QObject *propagated);
    void resetPropagated();

    QQuickItem *toolbar() const;
    void setToolbar(QQuickItem *toolbar);

    bool isPageTreeNode() const;

Q_SIGNALS:
    void isLeafChanged(bool isLeaf);
    void activeChanged(bool active);
    void parentNodeChanged(UCPageTreeNode *parentNode);
    void pageStackChanged(QQuickItem* pageStack);
    void activeLeafNodeChanged(QQuickItem* activeLeafNode);
    void propagatedChanged(QObject* propagated);
    void toolbarChanged(QQuickItem* toolbar);
    void isPageTreeNodeChanged(bool isPageTreeNode);

protected:
    UCPageTreeNode(UCPageTreeNodePrivate &, QQuickItem *parent);

private:
    Q_DECLARE_PRIVATE(UCPageTreeNode)
    Q_PRIVATE_SLOT(d_func(), void _q_activeBinding(bool active))
    Q_PRIVATE_SLOT(d_func(), void _q_pageStackBinding (QQuickItem *pageStack))
    Q_PRIVATE_SLOT(d_func(), void _q_propagatedBinding (QObject *propagated))
    bool m___isPageTreeNode;
};

#endif // UCPAGETREENODE_H
