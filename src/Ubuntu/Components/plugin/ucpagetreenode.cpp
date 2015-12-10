#include "ucpagetreenode.h"
#include "ucpagetreenode_p.h"

UCPageTreeNodePrivate::UCPageTreeNodePrivate()
    : m_parentNode(nullptr),
      m_activeLeafNode(nullptr),
      m_pageStack(nullptr),
      m_propagated(nullptr),
      m_toolbar(nullptr),
      m_flags(0),
      m_isLeaf(false),
      m_active(false)

{}

void UCPageTreeNodePrivate::init()
{
    Q_Q(UCPageTreeNode);
    q->setActiveFocusOnPress(true);

    auto slotUpdateParentLeafNode = [this] () {
        updateParentLeafNode();
    };

    QObject::connect(q, &UCPageTreeNode::activeChanged, slotUpdateParentLeafNode);
    QObject::connect(q, &UCPageTreeNode::activeLeafNodeChanged, slotUpdateParentLeafNode);

    //make sure all bindings are in tact
    q->resetActive();
    q->resetPageStack();
    q->resetPropagated();
}

/*!
  \internal
  Find the parent node.
 */
void UCPageTreeNodePrivate::updatePageTree()
{
    Q_Q(UCPageTreeNode);
    q->setParentNode(getParentPageTreeNode());
}

UCPageTreeNode *UCPageTreeNodePrivate::getParentPageTreeNode()
{
    Q_Q(UCPageTreeNode);
    UCPageTreeNode *node = nullptr;

    QQuickItem *currItem = q->parentItem();
    while (currItem) {
        UCPageTreeNode *currPageTreeNode = qobject_cast<UCPageTreeNode *>(currItem);
        if (currPageTreeNode) {
            if (currPageTreeNode->isLeaf()) {
                // children of a leaf are not part of the tree
                node = nullptr;
            } else {
                // current node is part of the tree with currPageTreeNode as its parent.
                node = currPageTreeNode;
            }
            break;
        }
        currItem = currItem->parentItem();
    }

    return node;
}

void UCPageTreeNodePrivate::_q_activeBinding(bool active)
{
    if (m_active == active)
        return;

    Q_Q(UCPageTreeNode);
    m_active = active;
    Q_EMIT q->activeChanged(active);
}

void UCPageTreeNodePrivate::_q_pageStackBinding(QQuickItem *pageStack)
{
    if (m_pageStack == pageStack)
        return;

    Q_Q(UCPageTreeNode);
    m_pageStack = pageStack;
    Q_EMIT q->pageStackChanged(pageStack);
}

void UCPageTreeNodePrivate::_q_propagatedBinding(QObject *propagated)
{
    if (m_propagated == propagated)
        return;

    Q_Q(UCPageTreeNode);
    m_propagated = propagated;
    Q_EMIT q->propagatedChanged(propagated);
}

void UCPageTreeNodePrivate::updateParentLeafNode()
{
    Q_Q(UCPageTreeNode);
    if (q->active() && q->parentNode()) {
        if (q->isLeaf())
            q->parentNode()->setActiveLeafNode(q);
        else
            q->parentNode()->setActiveLeafNode(q->activeLeafNode());
    }
}

static QList<UCPageTreeNode *> findPTNChild (QQuickItem *i)
{
    QList<UCPageTreeNode *> nodes;
    UCPageTreeNode *thisNode = qobject_cast<UCPageTreeNode *>(i);
    if(thisNode)
        nodes << thisNode;
    else {
        Q_FOREACH(QQuickItem *curr, i->childItems()) {
            nodes.append(findPTNChild(curr));
        }
    }
    return nodes;
}

static QList<UCPageTreeNodePrivate::Node> collectNodes (UCPageTreeNode *root)
{
    if (!root)
        return QList<UCPageTreeNodePrivate::Node>();

    QList<UCPageTreeNodePrivate::Node> nodes;

    QList<QQuickItem *> items = root->childItems();
    Q_FOREACH(QQuickItem *item, items) {
        QList<UCPageTreeNode *>subNodes = findPTNChild(item);

        if (subNodes.isEmpty()) continue;

        Q_FOREACH(UCPageTreeNode *currPTN, subNodes) {
            UCPageTreeNodePrivate::Node n;
            n.m_node = currPTN;
            n.m_children = collectNodes(currPTN);
            nodes.append(n);
        }
    }

    return nodes;
}

void UCPageTreeNodePrivate::dumpNode (const Node &n, const QString &oldDepth,const QString &depth, bool isRoot)
{
    UCPageTreeNode *currNode = n.m_node;

    if (!isRoot)
        qDebug().noquote().nospace()<<oldDepth<<"+--"<<currNode;
    else
        qDebug().noquote().nospace()<<currNode;

    qDebug().noquote().nospace()<<QString("%1|  ->parentNode: ").arg(depth)<<currNode->parentNode();
    qDebug().noquote().nospace()<<QString("%1|  ->pageStack: ").arg(depth)<<currNode->pageStack()
                               <<" custom:"<<((currNode->d_func()->m_flags & UCPageTreeNodePrivate::CustomPageStack) ? true : false);
    qDebug().noquote().nospace()<<QString("%1|  ->propagated: ").arg(depth)<<currNode->propagated()
                               <<" custom:"<<((currNode->d_func()->m_flags & UCPageTreeNodePrivate::CustomPropagated) ? true : false);
    qDebug().noquote().nospace()<<QString("%1|  ->active: ").arg(depth)<<currNode->active()
                               <<" custom:"<<((currNode->d_func()->m_flags & UCPageTreeNodePrivate::CustomActive) ? true : false);
    qDebug().noquote().nospace()<<QString("%1|  ->activeLeaf: ").arg(depth)<<currNode->activeLeafNode();

    if (n.m_children.length())
        qDebug().noquote().nospace()<<QString("%1|  ->isLeaf: ").arg(depth)<<currNode->isLeaf();
    else
        qDebug().noquote().nospace()<<QString("%1└  ->isLeaf: ").arg(depth)<<currNode->isLeaf();

    for (int i = 0; i < n.m_children.length(); i++) {
        QString subDepth = depth;

        if (i == n.m_children.length() - 1 ) //last
            subDepth.append("   ");
        else
            subDepth.append("|  ");

        dumpNode(n.m_children.at(i), depth, subDepth, false);
    }
}

void UCPageTreeNodePrivate::dumpNodeTree()
{
    Q_Q(UCPageTreeNode);

    UCPageTreeNode *node = q;
    UCPageTreeNode *rootNode = nullptr;
    while(node) {
        rootNode = node;
        node = node->d_func()->getParentPageTreeNode();
    }

    if (!rootNode) {
        qDebug()<<"Empty tree";
    } else {
        qDebug().noquote().nospace()<<"\nBegin Node List for"<<q;
        Node root;
        root.m_node = rootNode;
        root.m_children = collectNodes(rootNode);
        dumpNode(root);
        qDebug().noquote().nospace()<<"End Node List\n";
    }
}

UCPageTreeNode::UCPageTreeNode(QQuickItem *parent)
    : UCStyledItemBase(*(new UCPageTreeNodePrivate), parent)
{
    d_func()->init();
}

UCPageTreeNode::UCPageTreeNode(UCPageTreeNodePrivate &dd, QQuickItem *parent)
    : UCStyledItemBase(dd, parent)
{
    d_func()->init();
}

void UCPageTreeNode::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    Q_D(UCPageTreeNode);
    UCStyledItemBase::itemChange(change, value);
    if (change == QQuickItem::ItemParentHasChanged)
        d->updatePageTree();
}

bool UCPageTreeNode::isLeaf() const
{
    return d_func()->m_isLeaf;
}

void UCPageTreeNode::setIsLeaf(bool isLeaf)
{
    Q_D(UCPageTreeNode);

    if (d->m_isLeaf == isLeaf)
        return;

    d->m_isLeaf = isLeaf;
    Q_EMIT isLeafChanged(isLeaf);
    d->updateParentLeafNode();
}

void UCPageTreeNode::setParentNode(UCPageTreeNode *parentNode)
{
    Q_D(UCPageTreeNode);

    if (d->m_parentNode == parentNode)
        return;

    if (d->m_parentNode) {
        if (!(d->m_flags & UCPageTreeNodePrivate::CustomActive)) {
            disconnect(d->m_parentNode, SIGNAL(activeChanged(bool)),
                       this, SLOT(_q_activeBinding(bool)));
        }
        if (!(d->m_flags & UCPageTreeNodePrivate::CustomPageStack)) {
            disconnect(d->m_parentNode,SIGNAL(pageStackChanged(QQuickItem*)),
                       this, SLOT(_q_pageStackBinding (QQuickItem *)));
        }
        if (!(d->m_flags & UCPageTreeNodePrivate::CustomPropagated)) {
            disconnect(d->m_parentNode,SIGNAL(propagatedChanged(QObject*)),
                       this, SLOT(_q_propagatedBinding (QObject *)));
        }
        if (d->m_parentNode->activeLeafNode() == this) {
            d->m_parentNode->setActiveLeafNode(nullptr);
        }
    }

    d->m_parentNode = parentNode;

    if (d->m_parentNode ) {
        if (!(d->m_flags & UCPageTreeNodePrivate::CustomActive)) {
            connect(d->m_parentNode, SIGNAL(activeChanged(bool)),
                   this, SLOT(_q_activeBinding(bool)));
        }
        if (!(d->m_flags & UCPageTreeNodePrivate::CustomPageStack)) {
            connect(d->m_parentNode,SIGNAL(pageStackChanged(QQuickItem*)),
                   this, SLOT(_q_pageStackBinding (QQuickItem *)));
        }
        if (!(d->m_flags & UCPageTreeNodePrivate::CustomPropagated)) {
            connect(d->m_parentNode,SIGNAL(propagatedChanged(QObject*)),
                    this, SLOT(_q_propagatedBinding (QObject *)));
        }
    }

    Q_EMIT parentNodeChanged (parentNode);

    //update properties if they are not set manually
    if (!(d->m_flags & UCPageTreeNodePrivate::CustomActive))
        d->_q_activeBinding (parentNode && parentNode->active() );
    if (!(d->m_flags & UCPageTreeNodePrivate::CustomPageStack))
        d->_q_pageStackBinding (parentNode ? parentNode->pageStack() : nullptr);
    if (!(d->m_flags & UCPageTreeNodePrivate::CustomPropagated))
        d->_q_propagatedBinding (parentNode ? parentNode->propagated() : nullptr);

    //make sure the activeLeafNode is set correctly
    d->updateParentLeafNode();
}

UCPageTreeNode *UCPageTreeNode::parentNode() const
{
    return d_func()->m_parentNode;
}

void UCPageTreeNode::componentComplete()
{
    UCStyledItemBase::componentComplete();
    d_func()->updatePageTree();
}

void UCPageTreeNode::dumpNodeTree()
{
    d_func()->dumpNodeTree();
}

QObject *UCPageTreeNode::propagated() const
{
    return d_func()->m_propagated;
}

void UCPageTreeNode::setPropagated(QObject *propagated)
{
    Q_D(UCPageTreeNode);
    if (d->m_propagated == propagated) {
        return;
    }

    //remove the binding to parent
    if (d->m_parentNode && !(d->m_flags & UCPageTreeNodePrivate::CustomPropagated))
        disconnect(d->m_parentNode,SIGNAL(propagatedChanged(QObject*)),
                   this, SLOT(_q_propagatedBinding (QObject *)));

    d->m_flags |= UCPageTreeNodePrivate::CustomPropagated;
    d->_q_propagatedBinding(propagated);
}

void UCPageTreeNode::resetPropagated()
{
    Q_D(UCPageTreeNode);
    d->m_flags &= ~UCPageTreeNodePrivate::CustomPropagated;

    QObject *resPropagated = nullptr;
    if (d->m_parentNode) {
        resPropagated = d->m_parentNode->propagated();
        connect(d->m_parentNode,SIGNAL(propagatedChanged(QObject*)),
                this, SLOT(_q_propagatedBinding (QObject *)));
    }
    d->_q_propagatedBinding(resPropagated);
}

QQuickItem *UCPageTreeNode::toolbar() const
{
    return d_func()->m_toolbar;
}

void UCPageTreeNode::setToolbar(QQuickItem *toolbar)
{
    Q_D(UCPageTreeNode);
    if (d->m_toolbar == toolbar)
        return;

    d->m_toolbar = toolbar;
    Q_EMIT toolbarChanged(toolbar);
}

bool UCPageTreeNode::isPageTreeNode() const
{
    return true;
}

void UCPageTreeNode::setActiveLeafNode(QQuickItem *activeLeafNode)
{
    Q_D(UCPageTreeNode);
    if (d->m_activeLeafNode == activeLeafNode)
        return;

    d->m_activeLeafNode = activeLeafNode;
    Q_EMIT activeLeafNodeChanged(activeLeafNode);
}

QQuickItem *UCPageTreeNode::activeLeafNode() const
{
    return d_func()->m_activeLeafNode;
}

//property bool active: node.parentNode ? node.parentNode.active : false
void UCPageTreeNode::setActive(bool active)
{
    Q_D(UCPageTreeNode);

    if (d->m_active == active)
        return;

    //remove the binding to parent
    if (d->m_parentNode && !(d->m_flags & UCPageTreeNodePrivate::CustomActive)) {
        disconnect(d->m_parentNode, SIGNAL(activeChanged(bool)),
                   this, SLOT(_q_activeBinding(bool)));
    }

    d->m_flags |= UCPageTreeNodePrivate::CustomActive;
    d->_q_activeBinding(active);
}

bool UCPageTreeNode::active() const
{
    return d_func()->m_active;
}

void UCPageTreeNode::resetActive()
{
    Q_D(UCPageTreeNode);

    d->m_flags &= ~UCPageTreeNodePrivate::CustomActive;

    bool resActive = false;
    if (d->m_parentNode) {
        resActive = d->m_parentNode->active();
        connect(d->m_parentNode, SIGNAL(activeChanged(bool)),
                this, SLOT(_q_activeBinding(bool)));
    }
    d->_q_activeBinding(resActive);
}

void UCPageTreeNode::setPageStack(QQuickItem *pageStack)
{
    Q_D(UCPageTreeNode);

    if (d->m_pageStack == pageStack)
        return;

    //remove the binding to parent
    if (d->m_parentNode && !(d->m_flags & UCPageTreeNodePrivate::CustomPageStack))
        disconnect(d->m_parentNode,SIGNAL(pageStackChanged(QQuickItem*)),
                   this, SLOT(_q_pageStackBinding (QQuickItem *)));

    d->m_flags |= UCPageTreeNodePrivate::CustomPageStack;
    d->_q_pageStackBinding(pageStack);
}

QQuickItem *UCPageTreeNode::pageStack() const
{
    return d_func()->m_pageStack;
}

void UCPageTreeNode::resetPageStack()
{
    Q_D(UCPageTreeNode);

    d->m_flags &= ~UCPageTreeNodePrivate::CustomPageStack;

    QQuickItem *resPageStack = nullptr;
    if (d->m_parentNode) {
        resPageStack = d->m_parentNode->pageStack();
        connect(d->m_parentNode,SIGNAL(pageStackChanged(QQuickItem*)),
               this, SLOT(_q_pageStackBinding (QQuickItem *)));
    }

    d->_q_pageStackBinding(resPageStack);
}

#include "moc_ucpagetreenode.cpp"
