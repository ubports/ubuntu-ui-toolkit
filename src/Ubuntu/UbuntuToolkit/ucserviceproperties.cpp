/*
 * Copyright 2014 Canonical Ltd.
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

#include "ucserviceproperties.h"
#include "ucserviceproperties_p.h"
#include "i18n.h"
#include <QtQml/QQmlInfo>
#include <QtCore/QMetaProperty>
#include <QtQml/QQmlProperty>
#include <QtQml/private/qqmlproperty_p.h>

UT_NAMESPACE_BEGIN

UCServicePropertiesPrivate::UCServicePropertiesPrivate(UCServiceProperties *qq)
    : q_ptr(qq)
    , ready(false)
    , status(UCServiceProperties::Inactive)
    , type(UCServiceProperties::System)
{
}

UCServicePropertiesPrivate::~UCServicePropertiesPrivate()
{
}

UCServicePropertiesPrivate *UCServicePropertiesPrivate::get(UCServiceProperties *service)
{
    return service->d_func();
}

void UCServicePropertiesPrivate::warning(const QString &message)
{
    // append warning messages to the error string
    if (error.isEmpty()) {
        setError(message);
    } else {
        setError(QString("%1\n%2").arg(error).arg(message));
    }
    QString env = qgetenv("SHOW_SERVICEPROPERTIES_WARNINGS");
    if (!env.isEmpty() && (env == "1" || env.toLower() == "true")) {
        qmlInfo(q_ptr) << message;
    }
}

void UCServicePropertiesPrivate::setError(const QString &msg)
{
    if (error == msg) {
        return;
    }
    error = msg;
    Q_EMIT q_ptr->errorChanged();
}

void UCServicePropertiesPrivate::setStatus(UCServiceProperties::Status status)
{
    if (this->status == status) {
        return;
    }
    this->status = status;
    Q_EMIT q_ptr->statusChanged();
}

void printLocked(UCServiceProperties *owner)
{
    UCServicePropertiesPrivate::get(owner)->
            warning(QStringLiteral("Changing connection parameters forbidden."));
}

/*!
 * \qmltype ServiceProperties
 * \instantiates UbuntuToolkit::UCServiceProperties
 * \inqmlmodule Ubuntu.Components
 * \since Ubuntu.Components 1.1
 * \ingroup ubuntu-services
 * \brief The component enables accessing service properties from QML.
 *
 * The services accessed by the component are ones providing their interfaces
 * through DBus. The component is specialized to read properties exposed by these
 * services, andf to keep these property values up to date. It is not meant to
 * access signals or slots exposed, nor to change the values of the properties
 * watched.
 *
 * Properties watched should be declared within the body of the component like
 * any other QML property, preferably defining a default value for them. The component
 * will enumerate these properties and will ask the service to provide values for
 * those. When enumerating properties, each property will be checked twice, with
 * the case specified as well as with the first letter capitalized.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * ServiceProperties {
 *     service: "org.freenode.AccountsService"
 *     path: "/org/freenode/AccountsService"
 *     serviceInterface: "org.freenode.AccountsService"
 *     adaptorInterface: "com.ubuntu.touch.Accounts.Sound"
 *     // listing properties to watch
 *     // each property name existence will be checked against the current case
 *     // as well as with first character capitalized
 *     property bool incomingCallVibrate: true
 * }
 * \endqml
 *
 * Note that there are few properties which must be set in order the component
 * to work. These are \l service, \l path and \l adaptorInterface. Also, once
 * specified, \l service, \l serviceInterface and \l adaptorInterface values
 * should not be changed as it cannot be guaranteed that properties watched will
 * be available on those service. Therefore any change on these properties after
 * the component completion will be ignored. Property bindings on properties
 * watched will be ignored as well, as service will report changes in these property
 * values.
 *
 * The service is connected once the component gets completed (Component.onCompleted).
 * The \l error property specifies any error occured during connection, and the
 * \l status property notifies whether the connection to the service is active or not.
 *
 * \note Pay attention when chosing the service watched, and set your application's
 * AppArmor rights to ensure a successful service connection.
 */
UCServiceProperties::UCServiceProperties(QObject *parent)
    : QObject(parent)
    , d_ptr(createServicePropertiesAdapter(this))
{
}
UCServiceProperties::~UCServiceProperties()
{
    delete d_ptr;
    d_ptr = 0;
}

void UCServiceProperties::classBegin()
{
}

void UCServiceProperties::componentComplete()
{
    Q_D(UCServiceProperties);
    d->ready = true;
    // enumerate properties
    const QMetaObject *mo = metaObject();
    for (int i = mo->propertyOffset(); i < mo->propertyCount(); i++) {
        const QMetaProperty prop = mo->property(i);
        QString property(prop.name());

        // check the binding on the property and warn if there is one.
        QQmlProperty qmlProperty(this, property);
        if (QQmlPropertyPrivate::binding(qmlProperty)) {
            d->warning(QStringLiteral("Binding detected on property '%1' will be removed by the service updates.").
                       arg(property));
        }
        // insert both the declared and capitalized first character properties
        d->properties << property;
        property[0] = property[0].toUpper();
        d->properties << property;
    }
    // initialize DBus
    if (d->init()) {
        d->setStatus(UCServiceProperties::Synchronizing);
        d->fetchPropertyValues();
    }
}

/*!
 * \qmlproperty enum ServiceProperties::type
 * Specifies the DBus connection session type. It can get the following values:
 * \list
 *  \li - \e ServiceProperties.System when system bus is used (default)
 *  \li - \e ServiceProperties.Session when session bus is used
 * \endlist
 */
UCServiceProperties::ServiceType UCServiceProperties::type() const
{
    Q_D(const UCServiceProperties);
    return d->type;
}
void UCServiceProperties::setType(ServiceType type)
{
    Q_D(UCServiceProperties);
    if (d->type == type) {
        return;
    }
    if (d->ready) {
        printLocked(this);
        return;
    }
    d->type = type;
    Q_EMIT typeChanged();
}

/*!
 * \qmlproperty string ServiceProperties::service
 * The proeprty specifies the DBus service URI. It is mandatory to be specified.
 */
QString UCServiceProperties::service() const
{
    Q_D(const UCServiceProperties);
    return d->service;
}
void UCServiceProperties::setService(const QString &value)
{
    Q_D(UCServiceProperties);
    if (d->service == value) {
        return;
    }
    if (d->ready) {
        printLocked(this);
        return;
    }
    d->service = value;
    Q_EMIT serviceChanged();
}

/*!
 * \qmlproperty string ServiceProperties::path
 * The property specifies the DBus service connection path. It is mandatory to be
 * specified.
 */
QString UCServiceProperties::path() const
{
    Q_D(const UCServiceProperties);
    return d->path;
}
void UCServiceProperties::setPath(const QString &value)
{
    Q_D(UCServiceProperties);
    if (d->path == value) {
        return;
    }
    d->path = value;
    Q_EMIT pathChanged();
    if (d->ready) {
        // need to re-initialize connections
        d->init();
    }
}

/*!
 * \qmlproperty string ServiceProperties::serviceInterface
 * The property specifies the service intertface. If it is an empty string, the
 * component will refer to the merging of all interfaces found in the service.
 */
QString UCServiceProperties::interface() const
{
    Q_D(const UCServiceProperties);
    return d->interface;
}
void UCServiceProperties::setInterface(const QString &value)
{
    Q_D(UCServiceProperties);
    if (d->interface == value) {
        return;
    }
    if (d->ready) {
        printLocked(this);
        return;
    }
    d->interface = value;
    Q_EMIT serviceInterfaceChanged();
}
/*!
 * \qmlproperty string ServiceProperties::adaptorInterface
 * The proeprty specifies the dbus adaptor interface which provides the properties
 * watched. This can be a different interface that the one specified in \l serviceInterface,
 * and in the same way, it can be empty, in which case all the properties from all
 * interfaces of the service will be watched.
 */
QString UCServiceProperties::adaptor() const
{
    Q_D(const UCServiceProperties);
    return d->adaptor;
}
void UCServiceProperties::setAdaptor(const QString &value)
{
    Q_D(UCServiceProperties);
    if (d->adaptor == value) {
        return;
    }
    if (d->ready) {
        printLocked(this);
        return;
    }
    d->adaptor = value;
    Q_EMIT adaptorInterfaceChanged();
}

/*!
 * \qmlproperty string ServiceProperties::error
 * The property is set with a human readablestring each time an error occurrs
 * during the service connection. Empty string means no error.
 */
QString UCServiceProperties::error()
{
    Q_D(UCServiceProperties);
    return d->error;
}

/*!
 * \qmlproperty enum ServiceProperties::status
 * The property presents the status of the component.
 * \list
 *  \li - \e ServiceProperties.Inactive - the component is inactive, initial state
 *  \li - \e ServiceProperties.ConnectionError - there was a connection error, the
 *      \l error contains the error string.
 *  \li - \e ServiceProperties.Synchronizing - the connection to the service succeeded,
 *      and the properties are being synchronized;
 *  \li - \e ServiceProperties.Active - the service watcher is active and initial
 *      property synchronization completed.
 * \endlist
 * \note While the status is set to \e Synchronizing, the properties are checked
 * against their existence in the service. Each proeprty will be checked as declared
 * as well with capital first letter. If neither of these exists in the service,
 * it will be reported in the \l error property separately.
 */
UCServiceProperties::Status UCServiceProperties::status() const
{
    Q_D(const UCServiceProperties);
    return d->status;
}

UT_NAMESPACE_END

#include "moc_ucserviceproperties.cpp"
