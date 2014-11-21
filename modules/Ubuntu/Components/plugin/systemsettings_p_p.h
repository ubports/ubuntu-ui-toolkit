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

#ifndef SYSTEMSETTINGS_P_P_H
#define SYSTEMSETTINGS_P_P_H

#include "systemsettings_p.h"

class SystemSettings;
class SystemSettingsPrivate {
public:
    SystemSettingsPrivate(SystemSettings *qq);
    virtual ~SystemSettingsPrivate();

    static SystemSettingsPrivate *get(SystemSettings *settings);

    // initialize adaptation layer
    virtual void init() = 0;
    // catch property changes and report
    virtual void propertyChanged(const QString &property, const QVariant &value) = 0;
    // testing function, checks whether a property is defined or not.
    virtual bool hasProperty(const QString &property) = 0;
    // testing function, sets a property value
    virtual bool testProperty(const QString &property, const QVariant &value) = 0;

    SystemSettings *q_ptr;
    bool vibrate:1;
};

#endif // SYSTEMSETTINGS_P_P_H
