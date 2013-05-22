/*
 * Copyright 2013 Canonical Ltd.
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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#define foreach Q_FOREACH
#include <private/qqmlbinding_p.h>
#undef foreach

#include "ulconditionallayout.h"
#include "ulconditionallayout_p.h"

ULConditionalLayoutPrivate::ULConditionalLayoutPrivate(ULConditionalLayout *qq) :
    q_ptr(qq)
{
}

ULConditionalLayout::ULConditionalLayout(QObject *parent) :
    QQmlComponent(parent),
    d_ptr(new ULConditionalLayoutPrivate(this))
{
}

ULConditionalLayout::~ULConditionalLayout()
{
}

ULConditionalLayoutAttached * ULConditionalLayout::qmlAttachedProperties(QObject *owner)
{
    return new ULConditionalLayoutAttached(owner);
}


QString ULConditionalLayout::name() const
{
    return QString();
}
void ULConditionalLayout::setName(const QString &name)
{

}

QQmlBinding *ULConditionalLayout::when() const
{
    return 0;
}
void ULConditionalLayout::setWhen(QQmlBinding *when)
{

}
