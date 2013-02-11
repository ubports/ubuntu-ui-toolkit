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
 */

#ifndef BOTTOMBARVISIBILITYCOMMUNICATOR_H
#define BOTTOMBARVISIBILITYCOMMUNICATOR_H

#include <QObject>

class QDBusInterface;

class BottomBarVisibilityCommunicator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool forceHidden READ forceHidden NOTIFY forceHiddenChanged)

public:
    static BottomBarVisibilityCommunicator& instance() {
        static BottomBarVisibilityCommunicator instance;
        return instance;
    }

    bool forceHidden() const;

Q_SIGNALS:
    void forceHiddenChanged(bool forceHidden);

private Q_SLOTS:
    void onForceHiddenChanged(bool forceHidden);

private:
    BottomBarVisibilityCommunicator();

    QDBusInterface* m_shellDbusIface;

    // This is cached from dbus
    bool m_forceHidden;
};

#endif
