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

#include "ucnamespace.h"

/*!
 * \qmltype Ubuntu
 * \instantiates UCNamespace
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
UCNamespace::UCNamespace(QObject *parent) :
    QObject(parent)
{
}

