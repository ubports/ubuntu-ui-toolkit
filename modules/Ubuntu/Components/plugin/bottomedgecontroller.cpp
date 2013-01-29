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

#include "bottomedgecontroller.h"

/*!
    \internal

    BottomEdgeController controller is used to communicate with the Shell BottomEdgeController.
    This class allows hooking a bottom bar into the shell bottom edge dragging

    The user of this class needs to give the following information to the shell:
     * height: This is the height of the bottom bar that will be shown when dragging up

    The user of this class needs to obey the following information from the shell:
     * y: This is the y the bottom bar has to be positioned to
     *    0 means the bottom bar should totally shown
     *    height/2 means half the bottom bar should be shown
     *    height means the bottom bar should be hidden
     * available: This is true if the shell is available and thus will control y
     *            If this is false the user of the class should find a different way to
     *            animate the bottom edge since the shell won't do it (since the shell is not there)

    The user of this class can control the bottom bar behaviour:
     * active: If set to true, will show the bottom bar
     *         If set to false, will hide the bottom bar
     * locked: If set to true, the bottom bar will be locked in its fully open/closed position

*/
BottomEdgeController::BottomEdgeController()
 : m_y(-1),
   m_height(-1),
   m_active(false),
   m_locked(false),
   m_available(false)
{
}

double BottomEdgeController::y() const
{
    return m_y;
}

void BottomEdgeController::setY(double y)
{
    if (m_y != y) {
        m_y = y;
        Q_EMIT yChanged(y);
    }
}

double BottomEdgeController::height() const
{
    return m_height;
}

void BottomEdgeController::setHeight(double height)
{
    if (m_height != height) {
        m_height = height;
        Q_EMIT heightChanged(height);
    }
}

bool BottomEdgeController::active() const
{
    return m_active;
}

void BottomEdgeController::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        Q_EMIT activeChanged(active);
    }
}

bool BottomEdgeController::locked() const
{
    return m_locked;
}

void BottomEdgeController::setLocked(bool locked)
{
    if (locked != m_locked) {
        m_locked = locked;
        Q_EMIT lockedChanged(locked);
    }
}

bool BottomEdgeController::available() const
{
    return m_available;
}

void BottomEdgeController::setAvailable(bool available)
{
    if (available != m_available) {
        m_available = available;
        Q_EMIT availableChanged(available);
    }
}

