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
#include "ucglobals.h"
#include "ucfocusscope.h"

class QQuickFlickable;
class UCListItemBackground;
class UCListItemDivider;
class UCViewItemOptions;
class UCListItemBasePrivate;
class UCListItemBase : public UCFocusScope
{
    Q_OBJECT
    DECLARE_PRIVATE_READONLY_PROPERTY(UCListItemBackground*, background, CONSTANT)
    DECLARE_PRIVATE_READONLY_PROPERTY(UCListItemDivider*, divider, DESIGNABLE true)
    DECLARE_PRIVATE_PROPERTY_PTYPE(UCViewItemOptions, leadingOptions, DESIGNABLE false)
    DECLARE_PRIVATE_PROPERTY_PTYPE(UCViewItemOptions, trailingOptions, DESIGNABLE false)
    DECLARE_PRIVATE_READONLY_PROPERTY(bool, pressed)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_PROPERTY(QQmlListProperty<QQuickItem> children READ children NOTIFY childrenChanged DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")

    DECLARE_PRIVATE_READONLY_PROPERTY(QQuickFlickable*, flickable)
public:
    explicit UCListItemBase(QQuickItem *parent = 0);
    ~UCListItemBase();

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
    Q_DECLARE_PRIVATE(UCListItemBase)
    QQmlListProperty<QObject> data();
    QQmlListProperty<QQuickItem> children();
    Q_PRIVATE_SLOT(d_func(), void _q_rebound())
};

#endif // UCLISTITEM_H
