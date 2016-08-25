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
 *
 */

#include "exclusivegroup_p.h"
#include "ucaction_p.h"

#include <QSignalMapper>

#define CHECKED_PROPERTY "checked"

UT_NAMESPACE_BEGIN

static const char *checkableSignals[] = {
    "toggled(bool)",
    0
};

static bool isChecked(const QObject *o)
{
    if (!o) return false;
    QVariant checkedVariant = o->property(CHECKED_PROPERTY);
    return checkedVariant.isValid() && checkedVariant.toBool();
}

/*!
 * \qmltype ExclusiveGroup
 * \inqmlmodule Ubuntu.Components
 * \since Ubuntu.Components 1.3
 * \ingroup ubuntu
 * \inherits ActionList
 * \brief ExclusiveGroup provides a way to declare several checkable controls as mutually exclusive.
 *
 * The ExclusiveGroup will only allow a single object to have it's checkable property set to "true"
 * at any one time. The exclusive group accepts child Actions, but objects other than Actions can be
 * used by using the \l bindCheckable function as long as they support one of the required signals,
 * and a "checked" property.
 * \qml
 * ExclusiveGroup {
 *     Action {
 *         parameterType: Action.Bool
 *         state: true
 *     }
 *     Action {
 *         parameterType: Action.Bool
 *         state: false
 *     }
 * }
 * \endqml
 */
ExclusiveGroup::ExclusiveGroup(QObject *parent)
    : ActionList(parent)
    , m_signalMapper(new QSignalMapper(this))
    , m_entranceGuard(false)
{
    connect(this, &ActionList::added, this, &ExclusiveGroup::onActionAdded);
    connect(this, &ActionList::removed, this, &ExclusiveGroup::onActionRemoved);

    int index = m_signalMapper->metaObject()->indexOfMethod("map()");
    m_updateCurrentMethod = m_signalMapper->metaObject()->method(index);
    connect(m_signalMapper, static_cast<void(QSignalMapper::*)(QObject *)>(&QSignalMapper::mapped), this, [this](QObject *object) {
        if (isChecked(object)) {
            setCurrent(object);
        }
    });
}

void ExclusiveGroup::onActionAdded(UCAction *action)
{
    action->setExclusiveGroup(this);
}

void ExclusiveGroup::onActionRemoved(UCAction *action)
{
    action->setExclusiveGroup(nullptr);
}

/*!
 * \qmlproperty Action ExclusiveGroup::current
 * Returns the currently checked action
 */
void ExclusiveGroup::setCurrent(QObject *object)
{
    if (m_current == object)
        return;

    if (m_current)
        m_current->setProperty(CHECKED_PROPERTY, QVariant(false));
    m_current = object;
    if (m_current)
        m_current->setProperty(CHECKED_PROPERTY, QVariant(true));
    Q_EMIT currentChanged();
}

QObject *ExclusiveGroup::current() const
{
    return m_current.data();
}

/*!
 * \qmlmethod void ExclusiveGroup::bindCheckable(object object)
 * Explicitly bind an objects checkability to this exclusive group.
 * \note This only works with objects which support the following signals signals:
 * \list
 *  \li \b toggled(bool)
 * \endlist
 * \qml
 * Item {
 *     ExclusiveGroup {
 *         id: exclusiveGroup
 *     }
 *     Instantiator {
 *         model: 4
 *         onObjectAdded: exclusiveGroup.bindCheckable(object)
 *         onObjectRemoved: exclusiveGroup.unbindCheckable(object)
 *
 *         Action {
 *             checkable: true
 *         }
 *     }
 * }
 * \endqml
 * \sa ExclusiveGroup::unbindCheckable
 */
void ExclusiveGroup::bindCheckable(QObject *object)
{
    for (const char **signalName = checkableSignals; *signalName; signalName++) {
        int signalIndex = object->metaObject()->indexOfSignal(*signalName);
        if (signalIndex != -1) {
            QMetaMethod signalMethod = object->metaObject()->method(signalIndex);
            connect(object, signalMethod, m_signalMapper, m_updateCurrentMethod, Qt::UniqueConnection);
            m_signalMapper->setMapping(object, object);
            connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(unbindCheckable(QObject*)), Qt::UniqueConnection);
            if (!m_current && isChecked(object))
                setCurrent(object);
            break;
        }
    }
}

/*!
 * \qmlmethod void ExclusiveGroup::unbindCheckable(object object)
 * Explicitly unbind an objects checkability from this exclusive group.
 * \sa ExclusiveGroup::bindCheckable
 */
void ExclusiveGroup::unbindCheckable(QObject *object)
{
    if (m_current == object)
        setCurrent(0);

    disconnect(object, 0, m_signalMapper, 0);
    disconnect(object, SIGNAL(destroyed(QObject*)), this, SLOT(unbindCheckable(QObject*)));
}

UT_NAMESPACE_END
