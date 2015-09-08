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
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlProperty>
#include <QtGui/QGuiApplication>
#include "private/qqmldata_p.h"
#include "private/qqmlcontext_p.h"
#include <private/qqmlcomponentattached_p.h>

UCStateSaverAttachedPrivate::UCStateSaverAttachedPrivate(UCStateSaverAttached *qq, QObject *attachee)
    : q_ptr(qq)
    , m_attachee(attachee)
    , m_enabled(false)
{
}

/*
 * Initializer slot.
 */
void UCStateSaverAttachedPrivate::_q_init()
{
    m_id = qmlContext(m_attachee)->nameForObject(m_attachee);
    if (m_id.isEmpty()) {
        qmlInfo(m_attachee) << UbuntuI18n::instance().tr("Warning: attachee must have an ID. State will not be saved.");
        q_func()->setEnabled(false);
        return;
    }
    m_absoluteId = absoluteId(m_id);
    if (m_absoluteId.isEmpty()) {
        q_func()->setEnabled(false);
        return;
    }
    if (!StateSaverBackend::instance().registerId(m_absoluteId)) {
        qmlInfo(m_attachee) << UbuntuI18n::instance().tr("Warning: attachee's UUID is already registered, state won't be saved: %1").arg(m_absoluteId);
        m_absoluteId.clear();
        q_func()->setEnabled(false);
        return;
    }
    restore();
}

/*
 * Force state saver slot.
 */
void UCStateSaverAttachedPrivate::_q_save()
{
    if (m_enabled && StateSaverBackend::instance().enabled() && !m_properties.isEmpty() && !m_absoluteId.isEmpty()) {
        StateSaverBackend::instance().save(m_absoluteId, m_attachee, m_properties);
    }
}

void UCStateSaverAttachedPrivate::_q_globalEnableChanged(bool enabled)
{
    // sync component watchers signals
    watchComponent(enabled);
}

QString UCStateSaverAttachedPrivate::absoluteId(const QString &id)
{
    QQmlContext *attacheeContext = qmlContext(m_attachee);
    QQmlContextData *cdata = QQmlContextData::get(attacheeContext);
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    QUrl url(cdata->url());
#else
    QUrl url(cdata->url);
#endif
    QQmlData *ddata = QQmlData::get(m_attachee);
    QString path = url.path().replace('/', '_') + ':'
            + QString::number(ddata->lineNumber) + ':'
            + QString::number(ddata->columnNumber) + ':' + id;
    QObject *parent = m_attachee->parent();

    // check whether we have an "index" context property defined
    QVariant indexValue = attacheeContext->contextProperty("index");
    if (indexValue.isValid() && (indexValue.type() == QVariant::Int)) {
        path += indexValue.toString();
    }

    while (parent) {
        QString parentId = qmlContext(parent)->nameForObject(parent);
        QString className = QuickUtils::instance().className(parent);
        if (!parentId.isEmpty()) {
            path.prepend(className + '-' + parentId + ":");
        } else {
            qmlInfo(parent) << UbuntuI18n::instance().tr("All the parents must have an id.\nState saving disabled for %1, class %2").
                               arg(path).arg(className);
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
        StateSaverBackend::instance().load(m_absoluteId, m_attachee, m_properties);
    }
}

/*
 *
 */
void UCStateSaverAttachedPrivate::watchComponent(bool watch)
{
    Q_Q(UCStateSaverAttached);
    if (!watch) {
        // disconnect to save processing time if no state save is needed
        QQmlComponentAttached *componentAttached = QQmlComponent::qmlAttachedProperties(m_attachee);
        QObject::disconnect(componentAttached, SIGNAL(completed()), q, SLOT(_q_init()));
        QObject::disconnect(&StateSaverBackend::instance(), SIGNAL(initiateStateSaving()), q, SLOT(_q_save()));
    } else {
        // re-connect to proceed with saving
        QQmlComponentAttached *componentAttached = QQmlComponent::qmlAttachedProperties(m_attachee);
        QObject::connect(componentAttached, SIGNAL(completed()), q, SLOT(_q_init()));
        QObject::connect(&StateSaverBackend::instance(), SIGNAL(initiateStateSaving()), q, SLOT(_q_save()));
    }
}


/*!
 * \qmltype StateSaver
 * \instantiates UCStateSaverAttached
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu-services
 * \brief Attached properties to save component property states.
 *
 * StateSaver attached object provides the ability to save component property values
 * that can be restored after an inproper application close. The properties subject
 * of serialization must be given in the \l properties as a string, separated with
 * commas. The serialization will happen automatically on component's completion
 * time, as well as when the application is deactivated. Automatic serialization
 * of a component can be turned off by simply setting false to \l enabled property.
 *
 * \note The application name must be set correctly to the package name so that
 * state saving can work (e.g. com.ubuntu.calendar) through \l MainViewBase::applicationName.
 *
 * States saved are discarded when the application is closed properly. The state
 * loading is ignored (but not discarded) when the application is launched through
 * UriHandler.
 *
 * Example:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
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
 * Group properties can also be serialized by specifying the path to their individual
 * properties.
 * \qml
 * Rectangle {
 *     id: rect
 *     color: "gray"
 *     border {
 *         color: "blue"
 *         width: units.gu(1)
 *     }
 *     StateSaver.properties: "color, border.color, border.width"
 * }
 * \endqml
 *
 * StateSaver computes a unique identifier for the attachee using the component's
 * and all its parents' \a{id}. Therefore attachee component as well as all its
 * parents must have a valid ID set.
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
 * When used with Repeater, each created item from the Repeater's delegate will
 * be saved separately. Note that due to the way Repeater works, Repeaters do not
 * need to have id specified.
 *
 * \qml
 * Item {
 *     id: root
 *     // [...]
 *     Repeater {
 *         model: 10
 *         Rectangle {
 *             id: rect
 *             width: 50; height: 50
 *             StateSaver.properties: "width, height"
 *         }
 *     }
 *     // [...]
 * }
 * \endqml
 *
 * It can be used in the same way in ListView or GridView, except that both ListView
 * and GridView must have an id set.
 *
 * The StateSaver can save all \l{http://qt-project.org/doc/qt-5.0/qtqml/qtqml-typesystem-basictypes.html}{QML base types},
 * Objects, list of objects or variants containing any of these cannot be saved.
 */

UCStateSaverAttached::UCStateSaverAttached(QObject *attachee)
    : QObject(attachee)
    , d_ptr(new UCStateSaverAttachedPrivate(this, attachee))
{
    setEnabled(true);
    // connect to StateSaverBackend's enabledChanged signal to sync when the state saver is globally disabled/enabled
    connect(&StateSaverBackend::instance(), SIGNAL(enabledChanged(bool)), this, SLOT(_q_globalEnableChanged(bool)));
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
        if (StateSaverBackend::instance().enabled()) {
            d->watchComponent(d->m_enabled);
        }
        Q_EMIT enabledChanged();
    }
}

/*!
 * \qmlproperty string StateSaver::properties
 * List of properties to be serialized, separated with commas. Properties must be
 * writable and can only be \l{http://qt-project.org/doc/qt-5.0/qtqml/qtqml-typesystem-basictypes.html}{QML base types}.
 *
 * A custom single line input which saves the text, placeholderText, font and color would look as follows:
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
    QStringList propertyList = list.split(',', QString::SkipEmptyParts);
    for (int i = 0; i < propertyList.count(); i++) {
        propertyList[i] = propertyList.at(i).trimmed();
    }
    Q_D(UCStateSaverAttached);
    if (d->m_properties != propertyList) {
        d->m_properties = propertyList;
        Q_EMIT propertiesChanged();
        d->restore();
    }
}

#include "moc_ucstatesaver.cpp"
