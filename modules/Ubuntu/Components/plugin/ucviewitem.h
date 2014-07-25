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

class UCViewItemBackground;
class UCViewItemPrivate;
class UCViewItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(UCViewItemBackground* background READ background)
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_PROPERTY(QQmlListProperty<QQuickItem> children READ children NOTIFY childrenChanged DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit UCViewItem(QQuickItem *parent = 0);
    ~UCViewItem();

    UCViewItemBackground* background() const;
    bool pressed() const;

protected:
    void classBegin();
    void componentComplete();
    void itemChange(ItemChange change, const ItemChangeData &data);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

Q_SIGNALS:
    void pressedChanged();
    void childrenChanged();

    void clicked();

public Q_SLOTS:

private:
    Q_DECLARE_PRIVATE(UCViewItem)
    QScopedPointer<UCViewItemPrivate> d_ptr;
    QQmlListProperty<QObject> data();
    QQmlListProperty<QQuickItem> children();
    Q_PRIVATE_SLOT(d_func(), void _q_rebound())
};

// property binding
void ucBindProperty(const QQmlProperty &target, const QString &script, QObject *src, QQmlContext *context = 0);

#endif // UCVIEWITEM_H
