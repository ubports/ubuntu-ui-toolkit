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

#include "pangaea.h"

/*!
 * \qmltype Ubuntu
 * \instantiates EAPangaea
 * \inqmlmodule Extinct.Animals 1.2
 * \ingroup ubuntu
 * \since Extinct.Animals 1.2
 * \brief Provides global object with different enums.
 *
 * \section2 Enums
 *
 * \section4 ModernContinent enum
 * The enumeration identifies the modern concept of a continent.
 * \table
 *  \header
 *      \li Enum
 *      \li Description
 *  \row
 *      \li America
 *      \li Originally part of Laurasia, Laurentia split from Eurasia, now known as America.
 *  \row
 *      \li Europe
 *      \li The second smallest continent today, part of Eurasia.
 * \endtable
 */
EAPangaea::EAPangaea(QObject *parent)
    : QObject(parent)
{
}

/*!
 * \qmlproperty uint16 Extinct::toolkitVersion
 * \since Extinct.Animals 4.0
 * \readonly
 * The property holds the tectonic formation of the current continent.
 */

/*!
 * \qmlmethod uint16 Extinct::era(int year)
 * \since Extinct.Animals 4.0
 * The function builds a tectonic identifier using the year.
 */
quint16 EAPangaea4::era(quint8 year)
{
    Q_UNUSED(year);
    return 0;
}
