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
#include "ucstatesaver_p.h"
#include "statesaverbackend_p.h"
#include <QtQml/QQmlInfo>
#include "i18n.h"
#include "private/qqmldata_p.h"
#include "private/qqmlcontext_p.h"
#include <private/qqmlcomponentattached_p.h>
#include <QtGui/QGuiApplication>

#include <QDebug>

UCStateSaverAttachedPrivate::UCStateSaverAttachedPrivate(UCStateSaverAttached *qq, QObject *attachee)
    : q_ptr(qq)
    , m_attachee(attachee)
    , m_enabled(true)
    , m_changeDisabled(false)
    , m_propertiesDirty(false)
{
    qDebug() << "CONSTRUCT" << qmlContext(m_attachee)->nameForObject(m_attachee);

    // connect attachee completed and destroyed to catch state saving points
    QQmlComponentAttached *componentAttached = QQmlComponent::qmlAttachedProperties(m_attachee);
    QObject::connect(componentAttached, SIGNAL(completed()), q_ptr, SLOT(_q_init()));
    QObject::connect(componentAttached, SIGNAL(destruction()), q_ptr, SLOT(_q_save()));
    // connect to StateSaverBackend::idle
    QObject::connect(&StateSaverBackend::instance(), SIGNAL(idle()), q_ptr, SLOT(_q_save()));
    // and deactivated
    QObject::connect(&StateSaverBackend::instance(), SIGNAL(deactivated()), q_ptr, SLOT(_q_save()));
}

/*
 * Initializer slot.
 */
void UCStateSaverAttachedPrivate::_q_init()
{
    QString id = qmlContext(m_attachee)->nameForObject(m_attachee);
    m_absoluteId = absoluteId(id);
    qDebug() << "INIT" << m_absoluteId;

    if (m_absoluteId.isEmpty()) {
        qmlInfo(m_attachee) << UbuntuI18n::instance().tr("Warning: attacheee ID or one of its parent IDs are not defined. State will not be saved.");
    }
    if (!StateSaverBackend::registerId(m_absoluteId)) {
        qmlInfo(m_attachee) << UbuntuI18n::instance().tr("Warning: attachee's UUID is already registered, state won't be saved: %1").arg(m_absoluteId);
        m_absoluteId.clear();
    }
    restore();
}

/*
 * Force state saver slot.
 */
void UCStateSaverAttachedPrivate::_q_save()
{
    if (m_enabled && m_propertiesDirty && !m_properties.isEmpty() && !m_absoluteId.isEmpty()) {
        if (StateSaverBackend::instance().save(m_absoluteId, m_attachee, m_properties) > 0) {
            m_propertiesDirty = false;
        }
    }
}

void UCStateSaverAttachedPrivate::_q_propertyChange()
{
    if (m_changeDisabled) {
        return;
    }
    // the proeprty has been changed outside the state saver
    m_propertiesDirty = true;
}

QString UCStateSaverAttachedPrivate::absoluteId(const QString &id)
{
    QString path = id;
    QObject *parent = m_attachee->parent();

    while (parent) {
        QString parentId = qmlContext(parent)->nameForObject(parent);
        qDebug() << "^" << parentId;

        if (!parentId.isEmpty()) {
            path.prepend(parentId + ":");
        } else {
            qmlInfo(m_attachee) << "All the parents must have an id.";
            return QString();
        }

        parent = parent->parent();
    }
    return path;
}

void UCStateSaverAttachedPrivate::restore()
{
    if (m_enabled && !m_absoluteId.isEmpty() && !m_properties.isEmpty()) {
        // load group
        m_changeDisabled = true;
        StateSaverBackend::instance().load(m_absoluteId, m_attachee, m_properties);
        m_changeDisabled = false;
        // set dirty so at least one save will be occurring for sure
        m_propertiesDirty = true;
    }
}

/*
 * Watch property changes made outside of the state saver, so we know whether we need
 * to save the property values on idle.
 */
void UCStateSaverAttachedPrivate::connectChangeSlot(bool connect)
{
    Q_Q(UCStateSaverAttached);
    const QMetaObject *mo = m_attachee->metaObject();

    const QMetaMethod slot = q->metaObject()->method(q->metaObject()->indexOfSlot("_q_propertyChange()"));
    Q_FOREACH(const QString &propertyName, m_properties) {
        const QMetaProperty property = mo->property(mo->indexOfProperty(propertyName.toLocal8Bit().constData()));
        if (connect) {
            QObject::connect(m_attachee, property.notifySignal(), q, slot);
        } else {
            QObject::disconnect(m_attachee, property.notifySignal(), q, slot);
        }
    }
}


/*!
 * \qmltype StateSaver
 * \instantiates UCStateSaverAttached
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu-services
 * \brief Attached propertyes to save component property states.
 *
 * StateSaver attached object provides the ability to serialize property values
 * between application starts. Th eproperties subject of serialization must be
 * given in the \l properties as list of strings. Th eserialization will happen
 * automatically on component's completion and destruction time, as well as when
 * the application is deactivated or when idling. However, serialization can be
 * turned off by simply setting false to \l enabled property.
 *
 * Example:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 *
 * TextField {
 *     id: input
 *     StateSaver.properties: ["text"]
 *     StateSaver.enabled: input.enabled
 * }
 * \endqml
 *
 * StateSaver computes a unique identifier for the attachee using the component's
 * \a{id}. Therefore attachee component as well as all its parents must have a
 * valid and possibly unique ID set.
 *
 * The following example will give error fo rthe \a label, as the root component
 * has no id specified:
 * \qml
 * Item {
 *     //[...]
 *     Item {
 *         id: parent
 *         //[...]
 *         TextField {
 *             id: input
 *             StateSaver.properties: ["text"]
 *         }
 *     }
 * }
 * \endqml
 * but the following example will successfully save the text field content
 *
 * The StateSaver can save only certain types of properties, which are: bool, int,
 * real, string, list of these types. Objects, list of objects or variants containing
 * any of these cannot be saved.
 */

UCStateSaverAttached::UCStateSaverAttached(QObject *attachee)
    : QObject(attachee)
    , d_ptr(new UCStateSaverAttachedPrivate(this, attachee))
{
}

UCStateSaverAttached::~UCStateSaverAttached()
{
    qDebug() << "DESTROYING";
}

// getter/setter
/*!
 * \qmlproperty bool StateSaver::enabled
 * The property drives the automatic state saving. When disabled, state saving
 * will not happen on properties.
 *
 * The default value is true.
 */
bool UCStateSaverAttached::enabled() const
{
    return d_func()->m_enabled;
}
void UCStateSaverAttached::setEnabled(bool v)
{
    Q_D(UCStateSaverAttached);
    if (d->m_enabled != v) {
        d->m_enabled = v;
        // make sure next time we sync properties
        d->m_propertiesDirty = true;
        Q_EMIT enabledChanged();
    }
}

/*!
 * \qmlproperty list<string> StateSaver::properties
 * List of properties to be serialized. Properties must be writable and can only
 * have bool, int, real, string or list of one of these types.
 */
QStringList UCStateSaverAttached::properties() const
{
    return d_func()->m_properties;
}
void UCStateSaverAttached::setProperties(const QStringList &list)
{
    Q_D(UCStateSaverAttached);
    if (d->m_properties != list) {
        d->connectChangeSlot(false);
        d->m_properties = list;
        d->connectChangeSlot(true);
        Q_EMIT propertiesChanged();
        d->restore();
    }
}

#include "moc_ucstatesaver.cpp"
