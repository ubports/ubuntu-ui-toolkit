/*
 * Copyright 2015 Canonical Ltd.
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
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#include "smilodon.h"

/*!
 * \qmltype Smilodon
 * \instantiates EASmilodon
 * \inqmlmodule Extinct.Animals 1.0
 * \ingroup ubuntu-commandline
 * \brief The Smilodon class specifies the behavior of an extinct big cat.
 *
 * \sa Andrewsarchus
 */

EASmilodon::EASmilodon(QObject *parent) :
    QObject(parent)
{
}

/*!
 * \qmlproperty string Smilodon::name
 *
 * Name identifying the smilodon.
 *
 */
QString EASmilodon::name() const
{
    return m_name;
}

void EASmilodon::setName(const QString &name)
{
    m_name = name;
    Q_EMIT(nameChanged());
}

/*!
 * \qmlmethod string Smilodon::paw(int i)
 *
 * Returns the \e{i}th paw of the smilodon.
 * Values are counted from 0.
 *
 */
QString EASmilodon::paw(int i) const
{
    return QString("Paw #%1").arg(i);
}
