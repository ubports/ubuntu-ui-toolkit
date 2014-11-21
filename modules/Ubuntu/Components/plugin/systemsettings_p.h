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

#ifndef SYSTEMSETTINGS_P_H
#define SYSTEMSETTINGS_P_H

#include <QtCore/QObject>
#include "systemsettings_p_p.h"

//class SystemSettingsPrivate;
class SystemSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool vibraEnabled READ vibraEnabled NOTIFY vibraEnabledChanged)
public:
    ~SystemSettings();

    static SystemSettings &instance()
    {
        static SystemSettings instance;
        return instance;
    }

    bool vibraEnabled() const;

Q_SIGNALS:
    void vibraEnabledChanged();

protected:
    explicit SystemSettings(QObject *parent = 0);

    SystemSettingsPrivate *d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, SystemSettings)
    Q_PRIVATE_SLOT(d_func(), void propertyChanged(const QString &property, const QVariant &value))
};

// platform dependent system setting
SystemSettingsPrivate *createSystemSettingsAdaptation(SystemSettings *qq);

#endif // SYSTEMSETTINGS_P_H
