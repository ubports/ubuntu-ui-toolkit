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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "ucapplication.h"

#include <QtCore/QCoreApplication>

/*!
    \internal

    The UCApplication class is exposed to QML as the 'application' context property.
*/
UCApplication::UCApplication(QObject *parent) :
    QObject(parent)
{
}

QStringList UCApplication::arguments() const
{
    static QStringList arguments = QCoreApplication::arguments();
    return arguments;
}
