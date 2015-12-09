#ifndef UCPAGETREENODE_P_H
#define UCPAGETREENODE_P_H

#include "ucstyleditembase_p.h"

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
        CustomPropagated = 0x01,
        CustomActive     = 0x02,
        CustomPageStack  = 0x04
    };

    void _q_activeBinding (bool active);
    void _q_pageStackBinding (QQuickItem *pageStack);
    void _q_propagatedBinding (QObject *propagated);
    void dumpNodeTree ();

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

#endif // UCPAGETREENODE_P_H

