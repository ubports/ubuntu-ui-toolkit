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
#include "i18n.h"
#include "quickutils.h"
#include <QtQml/QQmlInfo>
#include <QtGui/QGuiApplication>
#include "private/qqmldata_p.h"
#include "private/qqmlcontext_p.h"
#include <private/qqmlcomponentattached_p.h>

UCStateSaverAttachedPrivate::UCStateSaverAttachedPrivate(UCStateSaverAttached *qq, QObject *attachee)
    : q_ptr(qq)
    , m_attachee(attachee)
    , m_enabled(false)
    , m_changeDisabled(false)
    , m_propertiesDirty(false)
{
}

/*
 * Initializer slot.
 */
void UCStateSaverAttachedPrivate::_q_init()
{
    QString id = qmlContext(m_attachee)->nameForObject(m_attachee);
    if (id.isEmpty()) {
        qmlInfo(m_attachee) << UbuntuI18n::instance().tr("Warning: attachee must have an ID. State will not be saved.");
        return;
    }
    m_absoluteId = absoluteId(id);
    if (m_absoluteId.isEmpty()) {
        return;
    }
    if (!StateSaverBackend::instance().registerId(m_absoluteId)) {
        qmlInfo(m_attachee) << UbuntuI18n::instance().tr("Warning: attachee's UUID is already registered, state won't be saved: %1").arg(m_absoluteId);
        m_absoluteId.clear();
        return;
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
    QString path = QuickUtils::instance().className(m_attachee) + '-' + id;
    QObject *parent = m_attachee->parent();

    while (parent) {
        QString parentId = qmlContext(parent)->nameForObject(parent);
        QString className = QuickUtils::instance().className(parent);
        if (!parentId.isEmpty()) {
            path.prepend(className + '-' + parentId + ":");
        } else {
            qmlInfo(parent) << UbuntuI18n::instance().tr("All the parents must have an id. State saving disabled for %1").arg(path);
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
        if (!property.isValid()) {
            continue;
        }
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
 * between application starts. The properties subject of serialization must be
 * given in the \l properties as a string, separated with commas. The serialization
 * will happen automatically on component's completion and destruction time, as
 * well as when the application is deactivated. Automatic serialization can be
 * turned off by simply setting false to \l enabled property.
 *
 * Example:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 *
 * TextField {
 *     id: input
 *     StateSaver.properties: "text"
 *     StateSaver.enabled: input.enabled
 * }
 * \endqml
 *
 * In this example the state saver is synchronized with the attachee's one.
 *
 * StateSaver computes a unique identifier for the attachee using the component's
 * \a{id}. Therefore attachee component as well as all its parents must have a
 * valid and possibly unique ID set.
 *
 * The following example will give error for the \a input, as the root component
 * has no id specified:
 * \qml
 * Item {
 *     //[...]
 *     Item {
 *         id: parent
 *         //[...]
 *         TextField {
 *             id: input
 *             StateSaver.properties: "text"
 *         }
 *     }
 * }
 * \endqml
 * but the following example will successfully save the text field content
 * \qml
 * Item {
 *     id: root
 *     //[...]
 *     Item {
 *         id: parent
 *         //[...]
 *         TextField {
 *             id: input
 *             StateSaver.properties: "text"
 *         }
 *     }
 * }
 * \endqml
 *
 * The StateSaver can save all \l{http://qt-project.org/doc/qt-5.0/qtqml/qtqml-typesystem-basictypes.html}{QML base types},
 * Objects, list of objects or variants containing any of these cannot be saved.
 */

UCStateSaverAttached::UCStateSaverAttached(QObject *attachee)
    : QObject(attachee)
    , d_ptr(new UCStateSaverAttachedPrivate(this, attachee))
{
    setEnabled(true);
}

UCStateSaverAttached::~UCStateSaverAttached()
{
    StateSaverBackend::instance().removeId(d_func()->m_absoluteId);
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
        if (!d->m_enabled) {
            // disconnect to save processing time if no state save is needed
            QQmlComponentAttached *componentAttached = QQmlComponent::qmlAttachedProperties(d->m_attachee);
            QObject::disconnect(componentAttached, SIGNAL(completed()), this, SLOT(_q_init()));
            QObject::disconnect(componentAttached, SIGNAL(destruction()), this, SLOT(_q_save()));
            QObject::disconnect(&QuickUtils::instance(), SIGNAL(deactivated()), this, SLOT(_q_save()));
        } else {
            // re-connect to proceed with saving
            QQmlComponentAttached *componentAttached = QQmlComponent::qmlAttachedProperties(d->m_attachee);
            QObject::connect(componentAttached, SIGNAL(completed()), this, SLOT(_q_init()));
            QObject::connect(componentAttached, SIGNAL(destruction()), this, SLOT(_q_save()));
            QObject::connect(&QuickUtils::instance(), SIGNAL(deactivated()), this, SLOT(_q_save()));
        }
        Q_EMIT enabledChanged();
    }
}

/*!
 * \qmlproperty string StateSaver::properties
 * List of properties to be serialized, separated with commas. Properties must be
 * writable and can only be \l{http://qt-project.org/doc/qt-5.0/qtqml/qtqml-typesystem-basictypes.html}{QML base types}.
 *
 * A custom singl eline input which saves the text, polaceholderText, font and color would look as follows:
 * \qml
 * TextField {
 *     id: input
 *     StateSaver.properties: "text, placeholderText, font, color"
 * }
 * \endqml
 */
QString UCStateSaverAttached::properties() const
{
    return d_func()->m_properties.join(',');
}
void UCStateSaverAttached::setProperties(const QString &list)
{
    QStringList propertyList = list.split(QRegularExpression("\\W+"), QString::SkipEmptyParts);
    Q_D(UCStateSaverAttached);
    if (d->m_properties != propertyList) {
        d->connectChangeSlot(false);
        d->m_properties = propertyList;
        d->connectChangeSlot(true);
        Q_EMIT propertiesChanged();
        d->restore();
    }
}

#include "moc_ucstatesaver.cpp"
