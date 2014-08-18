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
    DECLARE_PRIVATE_PROPERTY_PTYPE(QQmlComponent, delegate)
    DECLARE_PRIVATE_LISTPROPERTY(QObject, options)
    DECLARE_PRIVATE_PROPERTY(QColor, backgroundColor)
    Q_CLASSINFO("DefaultProperty", "options")
public:
    explicit UCListItemOptions(QObject *parent = 0);
    ~UCListItemOptions();

Q_SIGNALS:

public Q_SLOTS:

private:
    Q_DECLARE_PRIVATE(UCListItemOptions)
};

#endif // UCLISTITEMOPTIONS_H
