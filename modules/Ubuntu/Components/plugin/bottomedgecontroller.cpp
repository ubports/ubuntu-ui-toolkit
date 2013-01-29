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

