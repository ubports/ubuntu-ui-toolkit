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

#include "ucpagetreenode.h"
#include "ucpagetreenode_p.h"

#include <QQmlEngine>

Q_LOGGING_CATEGORY(ucPageTreeNode, "ubuntu.components.PageTreeNode", QtMsgType::QtWarningMsg)

#define PT_TRACE(params) qCDebug(ucPageTreeNode).noquote().nospace()<< params

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

/*!
 * \internal
 * \brief UCPageTreeNodePrivate::init
 * Initialize the UCPageTreeNode and setup the bindings
 */
void UCPageTreeNodePrivate::init()
{
    Q_Q(UCPageTreeNode);
    q->setActiveFocusOnPress(true);

    auto slotUpdateParentLeafNode = [this] () {
        updateParentLeafNode();
    };

    //connect all signals that are required to keep the "activeLeafNode" in the
    //parents valid. The activeLeadNode property in the parent is set if the current
    //PTN is a leaf itself (isLeaf = true) or one of its children is a leaf
    QObject::connect(q, &UCPageTreeNode::activeChanged, slotUpdateParentLeafNode);
    QObject::connect(q, &UCPageTreeNode::activeLeafNodeChanged, slotUpdateParentLeafNode);
    QObject::connect(q, &UCPageTreeNode::parentNodeChanged, slotUpdateParentLeafNode);

    //make sure all bindings are in tact
    initActive();
    initPageStack();
    initPropagated();
}

/*!
 * \internal
 * \brief UCPageTreeNodePrivate::initActive
 * Initialize active property and setup the propagating binding.
 * By default the parents active property is propagated to the children.
 */
void UCPageTreeNodePrivate::initActive()
{
    Q_Q(UCPageTreeNode);
    bool intialActive = false;
    if (m_parentNode) {
        intialActive = m_parentNode->active();
        QObject::connect(m_parentNode, SIGNAL(activeChanged(bool)),
                         q, SLOT(_q_activeBinding(bool)));
    }
    _q_activeBinding(intialActive);
}

/*!
 * \internal
 * \brief UCPageTreeNodePrivate::initPageStack
 * Initialize pageStack property and setup the propagating binding.
 * By default the parents pageStack property is propagated to the children.
 */
void UCPageTreeNodePrivate::initPageStack()
{
    Q_Q(UCPageTreeNode);
    QQuickItem *initialPageStack = nullptr;
    if (m_parentNode) {
        initialPageStack = m_parentNode->pageStack();
        QObject::connect(m_parentNode,SIGNAL(pageStackChanged(QQuickItem*)),
                         q, SLOT(_q_pageStackBinding (QQuickItem *)));
    }
    _q_pageStackBinding(initialPageStack);
}

/*!
 * \internal
 * \brief UCPageTreeNodePrivate::initPropagated
 * Initialize __propagated property and setup the propagating binding.
 * By default the parents __propagated property is propagated to the children.
 */
void UCPageTreeNodePrivate::initPropagated()
{
    Q_Q(UCPageTreeNode);
    QObject *initialPropagated = nullptr;
    if (m_parentNode) {
        initialPropagated = m_parentNode->propagated();
        QObject::connect(m_parentNode,SIGNAL(propagatedChanged(QObject*)),
                         q, SLOT(_q_propagatedBinding (QObject *)));
    }
    _q_propagatedBinding(initialPropagated);
}

/*!
  \internal
  Find the parent node and update the parentNode property
 */
void UCPageTreeNodePrivate::updatePageTree()
{
    Q_Q(UCPageTreeNode);
    q->setParentNode(getParentPageTreeNode());
}


/*!
   \internal
   \brief UCPageTreeNodePrivate::getParentPageTreeNode
   Returns the parent node in the page tree, or null if the item is the root node or invalid.
 */
UCPageTreeNode *UCPageTreeNodePrivate::getParentPageTreeNode()
{
    Q_Q(UCPageTreeNode);
    UCPageTreeNode *node = nullptr;

    //search the current tree for the next parent item that
    //is a UCPageTreeNode
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

/*!
 * \internal
 * \brief UCPageTreeNodePrivate::_q_activeBinding
 * Directly updates the activeBinding property. Is used as
 * callback to support qml style bindings that can be overriden
 */
void UCPageTreeNodePrivate::_q_activeBinding(bool active)
{
    if (m_active == active)
        return;

    Q_Q(UCPageTreeNode);
    m_active = active;
    Q_EMIT q->activeChanged(active);
}


/*!
 * \internal
 * \brief UCPageTreeNodePrivate::_q_pageStackBinding
 * Directly updates the pageStack property. Is used as
 * callback to support qml style bindings that can be overriden
 */
void UCPageTreeNodePrivate::_q_pageStackBinding(QQuickItem *pageStack)
{
    if (m_pageStack == pageStack)
        return;

    Q_Q(UCPageTreeNode);
    m_pageStack = pageStack;
    Q_EMIT q->pageStackChanged(pageStack);
}


/*!
 * \internal
 * \brief UCPageTreeNodePrivate::_q_propagatedBinding
 * Directly updates the __propagated property. Is used as
 * callback to support qml style bindings that can be overriden
 */
void UCPageTreeNodePrivate::_q_propagatedBinding(QObject *propagated)
{
    if (m_propagated == propagated)
        return;
    Q_Q(UCPageTreeNode);
    m_propagated = propagated;
    Q_EMIT q->propagatedChanged(propagated);
}

/*!
 * \internal
 * \brief UCPageTreeNodePrivate::updateParentLeafNode
 * Update the activeLeafNode of the parent. The activeLeadNode property
 * in the parent is set if the current PTN is a leaf itself (isLeaf = true)
 * or one of its children is a leaf
 */
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

/*!
 * \brief findPTNChild
 * Returns only the next level of UCPageTreeNode children of \l rootNode.
 * Just used for debugging output.
 */
static QList<UCPageTreeNode *> findPTNChild (QQuickItem *rootNode)
{
    QList<UCPageTreeNode *> nodes;
    UCPageTreeNode *thisNode = qobject_cast<UCPageTreeNode *>(rootNode);
    if(thisNode)
        nodes << thisNode;
    else {
        Q_FOREACH(QQuickItem *curr, rootNode->childItems()) {
            nodes.append(findPTNChild(curr));
        }
    }
    return nodes;
}

/*!
 * \brief collectNodes
 * Recursively collects all UCPageTreeNodes in a node tree and builds up
 * a representation of the Tree. Just used for debugging output.
 */
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

/*!
 * \brief UCPageTreeNodePrivate::dumpNode
 * Pretty prints the node \l n using the \l oldDepth of the parentNode and
 * \l depth as the current Nodes prefix. The \l isRoot parameter
 * specifies if the current Node \l n is the root Node.
 */
void UCPageTreeNodePrivate::dumpNode (const Node &n, const QString &oldDepth,const QString &depth, bool isRoot)
{
    UCPageTreeNode *currNode = n.m_node;

    //print the current node name and info
    if (!isRoot)
        PT_TRACE(oldDepth<<"+--"<<currNode);
    else
        PT_TRACE(currNode);

    //print the current nodes properties we are interested in
    switch(QQmlEngine::objectOwnership(currNode)) {
        case QQmlEngine::CppOwnership:
            PT_TRACE(QString("%1|  ->ownership: ").arg(depth)<<"C++");
        break;
        case QQmlEngine::JavaScriptOwnership:
            PT_TRACE(QString("%1|  ->ownership: ").arg(depth)<<"JS");
        break;
    }

    PT_TRACE(QString("%1|  ->parentNode: ").arg(depth)<<currNode->parentNode());
    PT_TRACE(QString("%1|  ->parent: ").arg(depth)<<currNode->parent());
    PT_TRACE(QString("%1|  ->pageStack: ").arg(depth)<<currNode->pageStack()
                               <<" custom:"<<((currNode->d_func()->m_flags & UCPageTreeNodePrivate::CustomPageStack) ? true : false));
    PT_TRACE(QString("%1|  ->propagated: ").arg(depth)<<currNode->propagated()
                               <<" custom:"<<((currNode->d_func()->m_flags & UCPageTreeNodePrivate::CustomPropagated) ? true : false));
    PT_TRACE(QString("%1|  ->active: ").arg(depth)<<currNode->active()
                               <<" custom:"<<((currNode->d_func()->m_flags & UCPageTreeNodePrivate::CustomActive) ? true : false));
    PT_TRACE(QString("%1|  ->activeLeaf: ").arg(depth)<<currNode->activeLeafNode());

    if (n.m_children.length())
        PT_TRACE(QString("%1|  ->isLeaf: ").arg(depth)<<currNode->isLeaf());
    else
        PT_TRACE(QString("%1└  ->isLeaf: ").arg(depth)<<currNode->isLeaf());

    //print the current nodes children
    for (int i = 0; i < n.m_children.length(); i++) {
        QString subDepth = depth;

        if (i == n.m_children.length() - 1 ) //last
            subDepth.append("   ");
        else
            subDepth.append("|  ");

        dumpNode(n.m_children.at(i), depth, subDepth, false);
    }
}

/*!
 * \brief UCPageTreeNodePrivate::dumpNodeTree
 * Prints the complete node tree this node is part of.
 * This recursively searches and prints the whole tree, so
 * its pretty expensive. Do not leaf the calls to it after
 * finishing the debugging.
 */
void UCPageTreeNodePrivate::dumpNodeTree()
{
    Q_Q(UCPageTreeNode);

    UCPageTreeNode *node = q;
    UCPageTreeNode *rootNode = nullptr;
    while(node) {
        rootNode = node;
        node = node->d_func()->getParentPageTreeNode();
    }

    PT_TRACE("Begin Node List for"<<q);
    if (Q_UNLIKELY(!rootNode)) {
        PT_TRACE("Empty tree");
    } else {
        Node root;
        root.m_node = rootNode;
        root.m_children = collectNodes(rootNode);
        dumpNode(root);
    }
    PT_TRACE("End Node List\n");
}

/*!
    \internal
    \qmltype PageTreeNode
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief The common parent of \l Page, \l MainView, \l PageStack and \l Tabs.

    It is used to propagate properties such as \l header and \l toolbar from a
    \l MainView (the root node) to each \l Page (leaf node) in the tree.
*/
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

    //update the parentNode property.
    //Likely it changes together with the Items parent
    UCStyledItemBase::itemChange(change, value);
    if (change == QQuickItem::ItemParentHasChanged) {
        d->updatePageTree();
    }
}

/*!
  \qmlproperty Item PageTreeNode::isLeaf
  Whether or not this node is a leaf, that is if it has no descendant that are nodes.
  Pages are leafs, and they don't have descendants in the PageTree.
 */
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

    //notify all our parent nodes that we are the leaf
    d->updateParentLeafNode();
}

/*!
  \qmlproperty Item PageTreeNode::parentNode
  The parent node of the current node in the page tree.
 */
void UCPageTreeNode::setParentNode(UCPageTreeNode *parentNode)
{
    Q_D(UCPageTreeNode);

    if (d->m_parentNode == parentNode)
        return;

    //disconnect from the old parent, we do not want to get
    //false property updates
    if (d->m_parentNode && !QQmlData::wasDeleted(d->m_parentNode)) {
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

        //the parent has changed, in case we or one of our children
        //were the active leaf node
        //make sure we are not anymore, since we are not part of that
        //tree
        if (d->m_parentNode->activeLeafNode() == this
                || d->m_parentNode->activeLeafNode() == activeLeafNode()) {
            d->m_parentNode->setActiveLeafNode(nullptr);
        }
    }

    d->m_parentNode = parentNode;

    //connect to the property changes of the parent node so they
    //can be propagated
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

    //update properties if they are not set manually
    if (!(d->m_flags & UCPageTreeNodePrivate::CustomActive))
        d->_q_activeBinding (parentNode && parentNode->active() );
    if (!(d->m_flags & UCPageTreeNodePrivate::CustomPageStack))
        d->_q_pageStackBinding (parentNode ? parentNode->pageStack() : nullptr);
    if (!(d->m_flags & UCPageTreeNodePrivate::CustomPropagated))
        d->_q_propagatedBinding (parentNode ? parentNode->propagated() : nullptr);

    Q_EMIT parentNodeChanged (parentNode);
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

/*!
  \internal
  \qmlproperty Item PageTreeNode::__propagated
  QtObject containing all the properties that are propagated from the
  root (MainView) of a page tree to its leafs (Pages).
  This object contains properties such as the header and toolbar that are
  instantiated by the MainView.

  This property is internal because the derived classes (MainView and Page)
  need to access it, but other components using those classes should not have
  access to it.
 */
QObject *UCPageTreeNode::propagated() const
{
    return d_func()->m_propagated;
}

void UCPageTreeNode::setPropagated(QObject *propagated)
{
    Q_D(UCPageTreeNode);

    //remove the binding to parent
    if (d->m_parentNode && !(d->m_flags & UCPageTreeNodePrivate::CustomPropagated))
        disconnect(d->m_parentNode,SIGNAL(propagatedChanged(QObject*)),
                   this, SLOT(_q_propagatedBinding (QObject *)));

    d->m_flags |= UCPageTreeNodePrivate::CustomPropagated;
    d->_q_propagatedBinding(propagated);
}

/*!
  \qmlproperty Item PageTreeNode::toolbar
  \deprecated
  The toolbar of the node. Propagates down from the root node.
  This property is DEPRECATED.
 */
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

/*!
  \internal
  \qmlproperty Item PageTreeNode::__isPageTreeNode
  Used to determine whether an Item is a PageTreeNode
 */
bool UCPageTreeNode::isPageTreeNode() const
{
    return true;
}

/*!
  \qmlproperty Item PageTreeNode::activeLeafNode
  The leaf node that is active.
 */
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


/*!
  \qmlproperty Item PageTreeNode::active
  At any time, there is exactly one path from the root node to a Page leaf node
  where all nodes are active. All other nodes are not active. This is used by
  \l Tabs and \l PageStack to determine which of multiple nodes in the Tabs or
  PageStack is the currently active one.
 */
void UCPageTreeNode::setActive(bool active)
{
    Q_D(UCPageTreeNode);

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

/*!
  \qmlproperty Item PageTreeNode::pageStack
  The \l PageStack that this Page has been pushed on, or null if it is not
  part of a PageStack. This value is automatically set for pages that are pushed
  on a PageStack, and propagates to its child nodes.
 */
// Note: pageStack is not included in the propagated property because there may
//  be multiple PageStacks in a single page tree.
void UCPageTreeNode::setPageStack(QQuickItem *pageStack)
{
    Q_D(UCPageTreeNode);

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

#include "moc_ucpagetreenode.cpp"
