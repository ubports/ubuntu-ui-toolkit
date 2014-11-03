/*
 * Copyright 2014 Canonical Ltd.
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
#include "ucstyleditembase.h"

class UCListItemContent;
class UCListItemDivider;
class UCListItemActions;
class UCListItemPrivate;
class UCListItemAttached;
class UCListItem : public UCStyledItemBase
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *contentItem READ contentItem CONSTANT)
    Q_PROPERTY(UCListItemDivider *divider READ divider CONSTANT)
    Q_PROPERTY(UCListItemActions *leadingActions READ leadingActions WRITE setLeadingActions NOTIFY leadingActionsChanged DESIGNABLE false)
    Q_PROPERTY(UCListItemActions *trailingActions READ trailingActions WRITE setTrailingActions NOTIFY trailingActionsChanged DESIGNABLE false)
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor highlightColor READ highlightColor WRITE setHighlightColor NOTIFY highlightColorChanged)
    Q_PROPERTY(bool selectable READ selectable WRITE setSelectable NOTIFY selectableChanged)
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_PROPERTY(QQmlListProperty<QQuickItem> children READ children NOTIFY childrenChanged DESIGNABLE false)
    Q_PRIVATE_PROPERTY(d_func(), bool moving READ isMoving NOTIFY movingChanged)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit UCListItem(QQuickItem *parent = 0);
    ~UCListItem();

    static UCListItemAttached *qmlAttachedProperties(QObject *owner);

    QQuickItem *contentItem() const;
    UCListItemDivider *divider() const;
    UCListItemActions *leadingActions() const;
    void setLeadingActions(UCListItemActions *options);
    UCListItemActions *trailingActions() const;
    void setTrailingActions(UCListItemActions *options);
    bool pressed() const;
    QColor color() const;
    void setColor(const QColor &color);
    QColor highlightColor() const;
    void setHighlightColor(const QColor &color);
    bool selectable() const;
    void setSelectable(bool selectable);
    bool selected() const;
    void setSelected(bool selected);

protected:
    void componentComplete();
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data);
    void itemChange(ItemChange change, const ItemChangeData &data);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *, QEvent *);

Q_SIGNALS:
    void leadingActionsChanged();
    void trailingActionsChanged();
    void pressedChanged();
    void colorChanged();
    void highlightColorChanged();
    void selectableChanged();
    void selectedChanged();
    void childrenChanged();
    void movingChanged();

    void clicked();
    void movingStarted();
    void movingEnded();

public Q_SLOTS:

private:
    Q_DECLARE_PRIVATE(UCListItem)
    QQmlListProperty<QObject> data();
    QQmlListProperty<QQuickItem> children();
    Q_PRIVATE_SLOT(d_func(), void _q_updateColors())
    Q_PRIVATE_SLOT(d_func(), void _q_dimmDisabled())
    Q_PRIVATE_SLOT(d_func(), void _q_rebound())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSize())
    Q_PRIVATE_SLOT(d_func(), void _q_completeRebinding())
    Q_PRIVATE_SLOT(d_func(), void _q_updateIndex())
    Q_PRIVATE_SLOT(d_func(), void _q_completeSnapping())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSelected())
};
QML_DECLARE_TYPEINFO(UCListItem, QML_HAS_ATTACHED_PROPERTIES)

class UCListItemAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<int> selectedIndexes READ selectedIndexes WRITE setSelectedIndexes NOTIFY selectedIndexesChanged)
public:
    explicit UCListItemAttached(QObject *parent = 0);

    QList<int> selectedIndexes() const;
    void setSelectedIndexes(const QList<int> &list);

    void addSelectedItem(UCListItem *item);
    void removeSelectedItem(UCListItem *item);
    bool isItemSelected(UCListItem *item);

Q_SIGNALS:
    void selectedIndexesChanged();

private:
    QList<int> m_indexes;
};

#endif // UCLISTITEM_H
