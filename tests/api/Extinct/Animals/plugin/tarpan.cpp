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

#include "tarpan.h"

/*!
 * \qmltype EATarpan
 * \instantiates EATarpan
 * \inqmlmodule Extinct.Animals 1.0
 * \ingroup ubuntu-commandline
 * \brief The Tarpan class specifies the behavior of a Eurasian wild horse.
 */

EATarpan::EATarpan(QObject *parent) :
    QObject(parent)
{
}

/*!
 * \qmlproperty string EATarpan::name
 *
 * Name identifying the wild horse.
 *
 */
QString EATarpan::name() const
{
    return m_name;
}

void EATarpan::setName(const QString &name)
{
    m_name = name;
    Q_EMIT(nameChanged());
}

/*!
 * \qmlmethod string EATarpan::kick()
 *
 * A generous kick, commonly towards someone's groin area.
 *
 */
void EATarpan::kick() const
{
}
