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

#include <QtQml/QQmlContext>
#include <QtQml>
#include <QtQml/QQmlProperty>

ContextPropertyChangeListener::ContextPropertyChangeListener(QQmlContext *context, const QString &contextProperty) :
    QObject(context),
    m_context(context),
    m_contextProperty(contextProperty),
    m_sender(0),
    m_property(0)
{
}

ContextPropertyChangeListener::~ContextPropertyChangeListener()
{
    delete m_property;
}

void ContextPropertyChangeListener::setUpdaterProperty(QObject *sender, const char *property)
{
    m_sender = sender;
    if (m_sender) {
        m_property = new QQmlProperty(m_sender, property, qmlContext(m_sender));
        m_property->connectNotifySignal(this, SLOT(updateContextPropertyFromSenderProperty()));
    }
}

void ContextPropertyChangeListener::updateContextProperty()
{
    QVariant value = m_context->contextProperty(m_contextProperty);
    m_context->setContextProperty(m_contextProperty, value);
}

void ContextPropertyChangeListener::updateContextPropertyFromSenderProperty()
{
    m_context->setContextProperty(m_contextProperty, m_property->read());
}
