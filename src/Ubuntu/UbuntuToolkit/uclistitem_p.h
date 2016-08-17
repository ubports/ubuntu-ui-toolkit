/*
 * Copyright 2014-2015 Canonical Ltd.
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

#ifndef UCLISTITEM_H
#define UCLISTITEM_H

#include <QtQuick/QQuickItem>
#include "ucstyleditembase_p.h"

UT_NAMESPACE_BEGIN

class UCListItemContent;
class UCListItemDivider;
class UCListItemActions;
class UCAction;
class UCListItemPrivate;
class UCListItemExpansion;
class UBUNTUTOOLKIT_EXPORT UCListItem : public UCStyledItemBase
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *contentItem READ contentItem CONSTANT)
#ifndef Q_QDOC
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCListItemDivider) *divider READ divider CONSTANT)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCListItemActions) *leadingActions READ leadingActions WRITE setLeadingActions NOTIFY leadingActionsChanged DESIGNABLE false)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCListItemActions) *trailingActions READ trailingActions WRITE setTrailingActions NOTIFY trailingActionsChanged DESIGNABLE false)
#else
    Q_PROPERTY(UCListItemDivider *divider READ divider CONSTANT)
    Q_PROPERTY(UCListItemActions *leadingActions READ leadingActions WRITE setLeadingActions NOTIFY leadingActionsChanged DESIGNABLE false)
    Q_PROPERTY(UCListItemActions *trailingActions READ trailingActions WRITE setTrailingActions NOTIFY trailingActionsChanged DESIGNABLE false)
#endif
    Q_PROPERTY(bool highlighted READ highlighted NOTIFY highlightedChanged)
    Q_PRIVATE_PROPERTY(UCListItem::d_func(), bool contentMoving READ contentMoving NOTIFY contentMovingChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor highlightColor READ highlightColor WRITE setHighlightColor RESET resetHighlightColor NOTIFY highlightColorChanged)
    Q_PRIVATE_PROPERTY(UCListItem::d_func(), bool dragging READ dragging NOTIFY draggingChanged)
    Q_PRIVATE_PROPERTY(UCListItem::d_func(), bool dragMode READ dragMode WRITE setDragMode NOTIFY dragModeChanged)
    Q_PRIVATE_PROPERTY(UCListItem::d_func(), bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
    Q_PRIVATE_PROPERTY(UCListItem::d_func(), bool selectMode READ selectMode WRITE setSelectMode NOTIFY selectModeChanged)
#ifndef Q_QDOC
    Q_PRIVATE_PROPERTY(UCListItem::d_func(), UT_PREPEND_NAMESPACE(UCAction) *action READ action WRITE setAction NOTIFY actionChanged DESIGNABLE false)
#else
    Q_PRIVATE_PROPERTY(UCListItem::d_func(), UCAction *action READ action WRITE setAction NOTIFY actionChanged DESIGNABLE false)
#endif
    Q_PRIVATE_PROPERTY(UCListItem::d_func(), QQmlListProperty<QObject> listItemData READ data DESIGNABLE false)
    Q_PRIVATE_PROPERTY(UCListItem::d_func(), QQmlListProperty<QQuickItem> listItemChildren READ children NOTIFY listItemChildrenChanged DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "listItemData")
    // 1.3
#ifndef Q_QDOC
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCListItemExpansion)* expansion READ expansion CONSTANT REVISION 1)
#else
    Q_PROPERTY(UCListItemExpansion* expansion READ expansion CONSTANT REVISION 1)
#endif
    Q_PROPERTY(bool swipeEnabled READ isSwipeEnabled WRITE setSwipeEnabled NOTIFY swipeEnabledChanged FINAL REVISION 1)
    Q_PROPERTY(bool swiped READ isSwiped NOTIFY swipedChanged FINAL REVISION 1)
public:
    explicit UCListItem(QQuickItem *parent = 0);
    ~UCListItem();

    // overrides
    bool keyNavigationFocus() const override;

    QQuickItem *contentItem() const;
    UCListItemDivider *divider() const;
    UCListItemActions *leadingActions() const;
    void setLeadingActions(UCListItemActions *options);
    UCListItemActions *trailingActions() const;
    void setTrailingActions(UCListItemActions *options);
    bool highlighted() const;
    QColor color() const;
    void setColor(const QColor &color);
    QColor highlightColor() const;
    void setHighlightColor(const QColor &color);
    void resetHighlightColor();
    // 1.3
    UCListItemExpansion *expansion();
    bool isSwipeEnabled() const;
    void setSwipeEnabled(bool swipeEnabled);
    bool isSwiped();

protected:
    virtual QObject *attachedViewItems(QObject *object, bool create);
    void classBegin() override;
    void componentComplete() override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    bool childMouseEventFilter(QQuickItem *child, QEvent *event) override;
    bool eventFilter(QObject *, QEvent *) override;
    void timerEvent(QTimerEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *key) override;

Q_SIGNALS:
    void leadingActionsChanged();
    void trailingActionsChanged();
    void highlightedChanged();
    void contentMovingChanged();
    void colorChanged();
    void highlightColorChanged();
    void draggingChanged();
    void dragModeChanged();
    void selectedChanged();
    void selectModeChanged();
    void actionChanged();
    void listItemChildrenChanged();
    Q_REVISION(1) void swipeEnabledChanged();
    Q_REVISION(1) void swipedChanged();

    void clicked();
    void pressAndHold();

    void contentMovementStarted();
    void contentMovementEnded();

public Q_SLOTS:

protected:
    Q_DECLARE_PRIVATE(UCListItem)

private:
    Q_PRIVATE_SLOT(d_func(), void _q_themeChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_relayout())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSwiping())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSize())
    Q_PRIVATE_SLOT(d_func(), void _q_updateIndex())
    Q_PRIVATE_SLOT(d_func(), void _q_contentMoving())
    Q_PRIVATE_SLOT(d_func(), void _q_syncDragMode())
    Q_PRIVATE_SLOT(d_func(), void _q_updateExpansion(const QList<int> &indices))
    Q_PRIVATE_SLOT(d_func(), void _q_popoverClosed())
};

class UCListItemDividerPrivate;
class UBUNTUTOOLKIT_EXPORT UCListItemDivider : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QColor colorFrom READ colorFrom WRITE setColorFrom NOTIFY colorFromChanged)
    Q_PROPERTY(QColor colorTo READ colorTo WRITE setColorTo NOTIFY colorToChanged)
public:
    explicit UCListItemDivider(UCListItem *parent = 0);
    ~UCListItemDivider();
    void init(UCListItem *listItem);
    void paletteChanged();

Q_SIGNALS:
    void colorFromChanged();
    void colorToChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *data) override;

private:
    void updateGradient();
    QColor colorFrom() const;
    void setColorFrom(const QColor &color);
    QColor colorTo() const;
    void setColorTo(const QColor &color);
    Q_DECLARE_PRIVATE(UCListItemDivider)
};

class UCDragEvent;
class UCViewItemsAttachedPrivate;
class UBUNTUTOOLKIT_EXPORT UCViewItemsAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool selectMode READ selectMode WRITE setSelectMode NOTIFY selectModeChanged)
    Q_PROPERTY(QList<int> selectedIndices READ selectedIndices WRITE setSelectedIndices NOTIFY selectedIndicesChanged)
    Q_PROPERTY(bool dragMode READ dragMode WRITE setDragMode NOTIFY dragModeChanged)
    Q_ENUMS(ExpansionFlag)
    // FIXME these should be revisioned, however bug doesn't allow us to do that
    // https://bugs.launchpad.net/ubuntu/+source/qtdeclarative-opensource-src/+bug/1389721
    Q_PROPERTY(QList<int> expandedIndices READ expandedIndices WRITE setExpandedIndices NOTIFY expandedIndicesChanged)
    Q_PROPERTY(int expansionFlags READ expansionFlags WRITE setExpansionFlags NOTIFY expansionFlagsChanged)
public:
    enum ExpansionFlag {
        Exclusive = 0x01,
        UnlockExpanded = 0x02,
        CollapseOnOutsidePress = Exclusive | 0x04
    };
    Q_DECLARE_FLAGS(ExpansionFlags, ExpansionFlag)
    explicit UCViewItemsAttached(QObject *owner = 0);
    ~UCViewItemsAttached();

    static UCViewItemsAttached *qmlAttachedProperties(QObject *owner);

    bool listenToRebind(UCListItem *item, bool listen);
    bool isAttachedToListView();
    bool isMoving();
    bool isBoundTo(UCListItem *item);

    // getter/setter
    bool selectMode() const;
    void setSelectMode(bool value);
    QList<int> selectedIndices() const;
    void setSelectedIndices(const QList<int> &list);
    bool dragMode() const;
    void setDragMode(bool value);

    // 1.3
    QList<int> expandedIndices() const;
    void setExpandedIndices(QList<int> indices);
    int expansionFlags() const;
    void setExpansionFlags(int flags);

private Q_SLOTS:
    void unbindItem();
    void completed();

Q_SIGNALS:
    void selectModeChanged();
    void selectedIndicesChanged(const QList<int> &indices);
    void dragModeChanged();

    void dragUpdated(UCDragEvent *event);

    // 1.3
    void expandedIndicesChanged(const QList<int> &indices);
    void expansionFlagsChanged();
private:
    Q_DECLARE_PRIVATE(UCViewItemsAttached)
};

UT_NAMESPACE_END

Q_DECLARE_OPERATORS_FOR_FLAGS(UT_PREPEND_NAMESPACE(UCViewItemsAttached)::ExpansionFlags)
QML_DECLARE_TYPEINFO(UT_PREPEND_NAMESPACE(UCViewItemsAttached), QML_HAS_ATTACHED_PROPERTIES)

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UCListItemExpansion : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(qreal height MEMBER m_height WRITE setHeight NOTIFY heightChanged)
public:
    explicit UCListItemExpansion(QObject *parent = 0);

    bool expandedLocked();
    void enableClickFiltering(bool enable);

    bool expanded();
    void setExpanded(bool expanded);
    void setHeight(qreal height);

Q_SIGNALS:
    void expandedChanged();
    void heightChanged();

protected:
    bool eventFilter(QObject *, QEvent *) override;

private:
    UCListItem *m_listItem;
    qreal m_height;
    bool m_filtering:1;

    friend class UCListItem;
    friend class UCListItemPrivate;
};

class UBUNTUTOOLKIT_EXPORT UCDragEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status)
    Q_PROPERTY(int from READ from)
    Q_PROPERTY(int to READ to)
    Q_PROPERTY(int minimumIndex MEMBER m_minimum)
    Q_PROPERTY(int maximumIndex MEMBER m_maximum)
    Q_PROPERTY(bool accept MEMBER m_accept)
    Q_ENUMS(Status)
public:
    enum Status {
        Started,
        Moving,
        Dropped
    };

    explicit UCDragEvent(Status status, int from, int to, int min, int max)
        : QObject(0), m_status(status), m_from(from), m_to(to), m_minimum(min), m_maximum(max), m_accept(true)
    {}
    int from() const
    {
        return m_from;
    }
    int to() const
    {
        return m_to;
    }
    Status status() const
    {
        return m_status;
    }

private:
    Status m_status;
    int m_from;
    int m_to;
    int m_minimum;
    int m_maximum;
    bool m_accept;

    friend class ListItemDragArea;
};

UT_NAMESPACE_END

#endif // UCLISTITEM_H
