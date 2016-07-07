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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCIMPORTVERSIONCHECKER_P_H
#define UCIMPORTVERSIONCHECKER_P_H

#include <QtCore>
#include <ubuntutoolkitglobal.h>

namespace UbuntuToolkit {

class UBUNTUTOOLKIT_EXPORT UCImportVersionChecker
{
public:
    UCImportVersionChecker() {}

    virtual QString propertyForVersion(quint16 version) const = 0;

    virtual quint16 importVersion(QObject *object);
};

UT_NAMESPACE_END

#endif // UCIMPORTVERSIONCHECKER_P_H
