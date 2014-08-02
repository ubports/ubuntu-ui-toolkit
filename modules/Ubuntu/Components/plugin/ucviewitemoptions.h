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

#ifndef UCVIEWITEMOPTIONS_H
#define UCVIEWITEMOPTIONS_H

#include <QtCore/QObject>
#include "ucviewitem_p.h"

class QQmlComponent;
class UCViewItemOptionsPrivate;
class UCViewItemOptions : public QObject
{
    Q_OBJECT
    DECLARE_PROPERTY_PRIVATE_RW(QQmlComponent*, delegate)
    DECLARE_LISTPROPERTY_PRIVATE(QObject, actions)
    DECLARE_PROPERTY_PRIVATE_RW(QColor, backgroundColor)
    DECLARE_PROPERTY_PRIVATE_RO(qreal, offset)
public:
    explicit UCViewItemOptions(QObject *parent = 0);
    ~UCViewItemOptions();

Q_SIGNALS:

public Q_SLOTS:

private:
    Q_DECLARE_PRIVATE(UCViewItemOptions)
    QScopedPointer<UCViewItemOptionsPrivate> d_ptr;
};

#endif // UCVIEWITEMOPTIONS_H
