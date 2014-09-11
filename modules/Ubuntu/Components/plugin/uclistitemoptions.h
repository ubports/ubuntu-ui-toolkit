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

#ifndef UCLISTITEMOPTIONS_H
#define UCLISTITEMOPTIONS_H

#include <QtCore/QObject>
#include "uclistitem_p.h"

class QQmlComponent;
class UCListItemOptionsPrivate;
class UCListItemOptions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(QQmlListProperty<QObject> options READ options CONSTANT)
    Q_PROPERTY(QQuickItem *panelItem READ panelItem NOTIFY panelItemChanged)
    Q_CLASSINFO("DefaultProperty", "options")
public:
    explicit UCListItemOptions(QObject *parent = 0);
    ~UCListItemOptions();

    QQmlComponent *delegate() const;
    void setDelegate(QQmlComponent *delegate);
    QQmlListProperty<QObject> options();
    QQuickItem *panelItem() const;

Q_SIGNALS:
    void delegateChanged();
    void panelItemChanged();

public Q_SLOTS:

private:
    Q_DECLARE_PRIVATE(UCListItemOptions)
    Q_PRIVATE_SLOT(d_func(), void _q_handlePanelDrag())
    Q_PRIVATE_SLOT(d_func(), void _q_handlePanelWidth())
};

#endif // UCLISTITEMOPTIONS_H
