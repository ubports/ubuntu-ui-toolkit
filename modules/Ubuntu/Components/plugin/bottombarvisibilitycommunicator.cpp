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

#include "bottombarvisibilitycommunicator.h"

#include <QDBusInterface>

static const char* BOTTOM_BAR_VISIBILITY_COMMUNICATOR_DBUS_PATH = "/BottomBarVisibilityCommunicator";
static const char* BOTTOM_BAR_VISIBILITY_COMMUNICATOR_DBUS_INTERFACE = "com.canonical.Shell.BottomBarVisibilityCommunicator";
static const char* DBUS_SERVICE = "com.canonical.Shell.BottomBarVisibilityCommunicator";

/*!
    \internal

    BottomBarVisibilityCommunicator controller is used to communicate with the Shell BottomBarVisibilityCommunicator.
    This class allows for the bottom edge interaction to happen

    The shell can control the bottom bar behaviour:
     * forceHidden: If set to true, the bottom bar has to be forced to be hidden
*/

BottomBarVisibilityCommunicator::BottomBarVisibilityCommunicator()
 : m_shellDbusIface(NULL),
   m_position(0),
   m_forceHidden(false)
{
    m_shellDbusIface = new QDBusInterface(DBUS_SERVICE, BOTTOM_BAR_VISIBILITY_COMMUNICATOR_DBUS_PATH, BOTTOM_BAR_VISIBILITY_COMMUNICATOR_DBUS_INTERFACE, QDBusConnection::sessionBus(), this);
    if (m_shellDbusIface->isValid()) {
        connect(m_shellDbusIface, SIGNAL(forceHiddenChanged(bool)), SLOT(onShellForceHiddenChanged(bool)));

        const bool forceHidden = m_shellDbusIface->property("forceHidden").toDouble();
        onShellForceHiddenChanged(forceHidden);
    }
}

bool BottomBarVisibilityCommunicator::forceHidden() const
{
    return m_forceHidden;
}

double BottomBarVisibilityCommunicator::position() const
{
    return m_position;
}

void BottomBarVisibilityCommunicator::setPosition(double position)
{
    if (position != m_position) {
        m_position = position;
        m_shellDbusIface->setProperty("position", position);
        Q_EMIT positionChanged(position);
    }
}

void BottomBarVisibilityCommunicator::onShellForceHiddenChanged(bool forceHidden)
{
    if (forceHidden != m_forceHidden) {
        m_forceHidden = forceHidden;
        Q_EMIT forceHiddenChanged(forceHidden);
    }
}
