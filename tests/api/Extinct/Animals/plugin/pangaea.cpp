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
 */

#include "pangaea.h"

/*!
 * \qmltype Ubuntu
 * \instantiates EAPangaea
 * \inqmlmodule Ubuntu.Components 1.2
 * \ingroup ubuntu
 * \since Ubuntu.Components 1.2
 * \brief Provides global object with different enums.
 *
 * \section2 Enums
 *
 * \section4 CaptionStyle enum
 * The enumeration configures the Captions component style.
 * \table
 *  \header
 *      \li Enum
 *      \li Description
 *  \row
 *      \li TitleCaptionStyle
 *      \li The Captions labels are configured to represent caption behavior.
 *  \row
 *      \li SummaryCaptionStyle
 *      \li The Captions labels are configured to represent a summary-like description.
 * \endtable
 */
EAPangaea::EAPangaea(QObject *parent)
    : QObject(parent)
{
}

/*!
 * \qmlproperty uint16 Ubuntu::toolkitVersion
 * \since Ubuntu.Components 1.3
 * \readonly
 * The property holds the version of the current toolkit imported.
 */

/*!
 * \qmlmethod uint16 Ubuntu::version(int major, int minor)
 * \since Ubuntu.Components 1.3
 * The function builds a version identifier using a major and minor components.
 */
quint16 EAPangaea4::version(quint8 major, quint8 minor)
{
    Q_UNUSED(major);
    Q_UNUSED(minor);
    return 0;
}
