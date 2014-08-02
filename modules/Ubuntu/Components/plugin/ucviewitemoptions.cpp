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

#include "ucviewitemoptions.h"
#include "ucviewitemoptions_p.h"
#include "ucviewitem_p.h"

UCViewItemOptionsPrivate::UCViewItemOptionsPrivate(UCViewItemOptions *qq)
    : q_ptr(qq)
    , delegate(0)
    , offset(0.0)
    , backgroundColor(Qt::red)
{

}

UCViewItemOptions::UCViewItemOptions(QObject *parent)
    : QObject(parent)
    , d_ptr(new UCViewItemOptionsPrivate(this))
{
}
UCViewItemOptions::~UCViewItemOptions()
{
}

/*!
 * \qmlproperty Component ViewItemOptions::delegate
 */
PROPERTY_SETTER(UCViewItemOptions, QQmlComponent*, delegate, qDebug() << "HAI")
PROPERTY_GETTER(UCViewItemOptions, QQmlComponent*, delegate)

/*!
 * \qmlproperty real ViewItemOptions::offset
 */
PROPERTY_GETTER(UCViewItemOptions, qreal, offset)

/*!
 * \qmlproperty list<Action> ViewItemOptions::actions
 */
LISTPROPERTY_GETTER(UCViewItemOptions, QObject, actions)

/*!
 * \qmlproperty color ViewItemOptions::backgroundColor
 */
PROPERTY_GETTER(UCViewItemOptions, QColor, backgroundColor)
PROPERTY_SETTER(UCViewItemOptions, QColor, backgroundColor, qDebug() << "HAI COLOR")
