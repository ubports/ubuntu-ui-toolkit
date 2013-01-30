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

/*!
    \internal

    BottomBarVisibilityCommunicator controller is used to communicate with the Shell BottomBarVisibilityCommunicator.
    This class allows for the bottom edge interaction to happen

    The user of this class needs to give the following information to the shell:
     * currentHeight: This is the current height of the bottom bar
     * targetHeight: This is the target final height of the bottom bar

    The shell can control the bottom bar behaviour:
     * forceHidden: If set to true, the bottom bar has to be forced to be hidden
*/
BottomBarVisibilityCommunicator::BottomBarVisibilityCommunicator()
 : m_currentHeight(-1),
   m_targetHeight(-1),
   m_forceHidden(false)
{
}

double BottomBarVisibilityCommunicator::currentHeight() const
{
    return m_currentHeight;
}

void BottomBarVisibilityCommunicator::setCurrentHeight(double currentHeight)
{
    if (m_currentHeight != currentHeight) {
        m_currentHeight = currentHeight;
        Q_EMIT currentHeightChanged(currentHeight);
    }
}

double BottomBarVisibilityCommunicator::targetHeight() const
{
    return m_targetHeight;
}

void BottomBarVisibilityCommunicator::setTargetHeight(double targetHeight)
{
    if (m_targetHeight != targetHeight) {
        m_targetHeight = targetHeight;
        Q_EMIT targetHeightChanged(targetHeight);
    }
}

bool BottomBarVisibilityCommunicator::forceHidden() const
{
    return m_forceHidden;
}

void BottomBarVisibilityCommunicator::setForceHidden(bool forceHidden)
{
    if (forceHidden != m_forceHidden) {
        m_forceHidden = forceHidden;
        Q_EMIT forceHiddenChanged(forceHidden);
    }
}
