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

#ifndef UCLISTITEMACTIONS_H
#define UCLISTITEMACTIONS_H

#include <QtCore/QObject>
#include "uclistitem_p.h"

class QQmlComponent;
class UCAction;
class UCListItemActionsPrivate;
class UCListItemActions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(QQmlListProperty<UCAction> actions READ actions CONSTANT)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit UCListItemActions(QObject *parent = 0);
    ~UCListItemActions();

    QQmlComponent *delegate() const;
    void setDelegate(QQmlComponent *delegate);
    QQmlListProperty<UCAction> actions();
    QQmlListProperty<QObject> data();

Q_SIGNALS:
    void delegateChanged();

private:
    Q_DECLARE_PRIVATE(UCListItemActions)
};

#endif // UCLISTITEMACTIONS_H
