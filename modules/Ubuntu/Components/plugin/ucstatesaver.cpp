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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucstatesaver.h"
#include "statesaverbackend_p.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlProperty>
#include "private/qqmldata_p.h"
#include "private/qqmlcontext_p.h"
#include <QtGui/QGuiApplication>

#include <QDebug>

UCStateSaverAttached::UCStateSaverAttached(QObject *attachee)
    : QObject(attachee)
    , m_attachee(attachee)
    , m_enabled(true)
{
    QQmlContextData *contextData = QQmlContextPrivate::get(qmlContext(m_attachee))->data;
    qDebug() << "CONSTRUCT" << contextData->findObjectId(m_attachee);

    // connect attachee completed and destroyed to catch state saving points
    QObject* componentAttached = qmlAttachedPropertiesObject<QQmlComponent>(m_attachee, false);
    QObject::connect(componentAttached, SIGNAL(completed()), this, SLOT(init()));
    QObject::connect(componentAttached, SIGNAL(destruction()), this, SLOT(save()));
    // TODO: connect save() to application's focus gain/loose signal
}

UCStateSaverAttached::~UCStateSaverAttached()
{
}

// getter/setter
bool UCStateSaverAttached::enabled() const
{
    return m_enabled;
}
void UCStateSaverAttached::setEnabled(bool v)
{
    if (m_enabled != v) {
        m_enabled = v;
        Q_EMIT enabledChanged();
    }
}

QStringList UCStateSaverAttached::properties() const
{
    return m_properties;
}
void UCStateSaverAttached::setProperties(const QStringList &list)
{
    if (m_properties != list) {
        m_properties = list;
        Q_EMIT propertiesChanged();
    }
}

void UCStateSaverAttached::init()
{
    QString id = qmlContext(m_attachee)->nameForObject(m_attachee);
}

void UCStateSaverAttached::save()
{

}

QString UCStateSaverAttached::absoluteId(const QString &id)
{
    QString path = id;
    return path;
}
