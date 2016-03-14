/*
 * Copyright 2016 Canonical Ltd.
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

#include "ucpagewrapperincubator_p.h"
#include <QVariantMap>
#include <QQmlInfo>

/*!
  \internal
  Incubator wrapper object. Used when page is loaded asynchronously.
  */
UCPageWrapperIncubator::UCPageWrapperIncubator(QQmlIncubator::IncubationMode mode, QObject *parent)
    : QObject(parent),
      QQmlIncubator(mode)
{
}

UCPageWrapperIncubator::~UCPageWrapperIncubator()
{}

void UCPageWrapperIncubator::forceCompletion()
{
    QQmlIncubator::forceCompletion();
}

QJSValue UCPageWrapperIncubator::onStatusChanged() const
{
    return m_onStatusChanged;
}

void UCPageWrapperIncubator::setOnStatusChanged(QJSValue onStatusChanged)
{
    m_onStatusChanged = onStatusChanged;
}

void UCPageWrapperIncubator::setInitialState(QObject *target)
{
    Q_EMIT initialStateRequested(target);
}

void UCPageWrapperIncubator::statusChanged(QQmlIncubator::Status status)
{
    Q_EMIT enterOnStatusChanged();
    if (m_onStatusChanged.isCallable()) {
        m_onStatusChanged.call(QJSValueList()<<QJSValue(static_cast<int>(status)));
    }
    Q_EMIT statusHasChanged(status);
}
