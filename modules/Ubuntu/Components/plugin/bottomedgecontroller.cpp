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
     * enabled: This is whether that bottom bar actually should be shown
     *          Setting it to false is equivalent to setting height to 0

    The user of this class needs to obey the following information from the shell:
     * y: This is the y the bottom bar has to be positioned to
     *    0 means the bottom bar should totally shown
     *    height/2 means hald the bottom bar should be shown
     *    height means the bottom bar should be hidden
     * available: This is true if the shell is available and thus will control y
     *            If this is false the user of the class should find a different way to
     *            animate the bottom edge since the shell won't do it (since the shell is not there)

    The user of this class has a function to instruct the shell:
     * setForceVisible: If set to true the bottom bar will be shown even
     *                  if the user did not drag the bottom edge to show it
*/
BottomEdgeController::BottomEdgeController()
 : m_y(-1),
   m_height(-1),
   m_enabled(false),
   m_forceVisible(false),
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

bool BottomEdgeController::enabled() const
{
    return m_enabled;
}

void BottomEdgeController::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged(enabled);
    }
}

bool BottomEdgeController::forceVisible() const
{
    return m_forceVisible;
}

void BottomEdgeController::setForceVisible(bool visible)
{
    if (visible != m_forceVisible) {
        m_forceVisible = visible;
        Q_EMIT forceVisibleChanged(visible);
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

