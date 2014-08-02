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

#ifndef UCVIEWITEM_H
#define UCVIEWITEM_H

#include <QtQuick/QQuickItem>
#include "ucglobals.h"

class QQuickFlickable;
class UCViewItemBackground;
class UCViewItemDivider;
class UCViewItemOptions;
class UCViewItemBasePrivate;
class UCViewItemBase : public QQuickItem
{
    Q_OBJECT
    DECLARE_PROPERTY_PRIVATE_RW(UCViewItemOptions*, leadingOptions)
    DECLARE_PROPERTY_PRIVATE_RW(UCViewItemOptions*, trailingOptions)
    DECLARE_PROPERTY_PRIVATE_RO(UCViewItemBackground*, background)
    DECLARE_PROPERTY_PRIVATE_RO(UCViewItemDivider*, divider)
    DECLARE_PROPERTY_PRIVATE_RO(bool, pressed)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_PROPERTY(QQmlListProperty<QQuickItem> children READ children NOTIFY childrenChanged DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")

    DECLARE_PROPERTY_PRIVATE_RO(QQuickFlickable*, flickable)
public:
    explicit UCViewItemBase(QQuickItem *parent = 0);
    ~UCViewItemBase();

    QQuickFlickable *flickable() const;

protected:
    void itemChange(ItemChange change, const ItemChangeData &data);
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

Q_SIGNALS:
    void childrenChanged();

    void clicked();

public Q_SLOTS:

private:
    Q_DECLARE_PRIVATE(UCViewItemBase)
    QScopedPointer<UCViewItemBasePrivate> d_ptr;
    QQmlListProperty<QObject> data();
    QQmlListProperty<QQuickItem> children();
    Q_PRIVATE_SLOT(d_func(), void _q_rebound())
};

#endif // UCVIEWITEM_H
