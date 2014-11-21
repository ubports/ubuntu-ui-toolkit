/*
 * Copyright 2012 Canonical Ltd.
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

#include "listener.h"

#include <QQmlContext>

ContextPropertyChangeListener::ContextPropertyChangeListener(QQmlContext *context, const QString &contextProperty,
                                                             QObject *source, const QString &sourceProperty) :
    QObject(context),
    m_context(context),
    m_contextProperty(contextProperty),
    m_source(source),
    m_sourceProperty(sourceProperty)
{
}

void ContextPropertyChangeListener::updateContextProperty()
{
    QVariant value;
    if (m_source && !m_sourceProperty.isEmpty()) {
        value = m_source->property(m_sourceProperty.toLocal8Bit().constData());
    } else {
        value = m_context->contextProperty(m_contextProperty);
    }
    m_context->setContextProperty(m_contextProperty, value);
}
