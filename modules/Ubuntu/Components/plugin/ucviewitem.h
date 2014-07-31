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

class QQuickFlickable;
class UCViewItemBackground;
class UCViewItemBasePrivate;
class UCViewItemBase : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(UCViewItemBackground* background READ background)
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_PROPERTY(QQmlListProperty<QQuickItem> children READ children NOTIFY childrenChanged DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")

    Q_PROPERTY(QQuickFlickable *flickable READ flickable NOTIFY flickableChanged)
public:
    explicit UCViewItemBase(QQuickItem *parent = 0);
    ~UCViewItemBase();

    UCViewItemBackground* background() const;
    bool pressed() const;

    QQuickFlickable *flickable() const;

protected:
    void itemChange(ItemChange change, const ItemChangeData &data);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

Q_SIGNALS:
    void pressedChanged();
    void childrenChanged();

    void flickableChanged();

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
