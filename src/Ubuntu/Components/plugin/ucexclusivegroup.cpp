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

#include "ucexclusivegroup.h"
#include "ucaction.h"

/*!
 * \qmltype ExclusiveGroup
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief An \l ActionList which controls a set of Actions with mutually exclusive states.
 *
 * The ExclusiveGroup will only allow a single \l Action to have it's state set to "true" at any one time.
 * If the currently selected Action attempts to trigger with a false value, the trigger will be rejected.
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
UCExclusiveGroup::UCExclusiveGroup(QObject *parent)
    : UCActionList(parent)
    , m_entranceGuard(false)
{
    connect(this, &UCActionList::added, this, &UCExclusiveGroup::onActionAdded);
    connect(this, &UCActionList::removed, this, &UCExclusiveGroup::onActionRemoved);
}

void UCExclusiveGroup::onActionAdded(UCAction *action)
{
    action->setExclusiveGroup(this);
    connect(action, &UCAction::stateChanged, this, [this, action]() { stateChanged(action); });
}

void UCExclusiveGroup::onActionRemoved(UCAction *action)
{
    action->setExclusiveGroup(nullptr);
    disconnect(action, &UCAction::stateChanged, this, 0);
}

void UCExclusiveGroup::stateChanged(UCAction* action)
{
    if (m_entranceGuard) return;
    m_entranceGuard = true;

    if (action->state().type() != QVariant::Bool) {
        m_entranceGuard = false;
        return;
    }

    Q_FOREACH(UCAction* fromList, list()) {
        if (fromList->parameterType() != UCAction::Bool) {
            continue;
        }

        if (fromList != action && fromList->state().toBool() == true) {
            fromList->trigger(false);
        }
    }

    if (action->state().toBool() == true) {
        setSelected(action);
    } else {
        setSelected(nullptr);
    }

    m_entranceGuard = false;
}

/*!
 * \qmlproperty Action ExclusiveGroup::selected
 * \default
 * Returns the currently selected action
 */
void UCExclusiveGroup::setSelected(UCAction *action)
{
    if (m_selected != action) {
        if (action) {
            m_selected = action;
        } else {
            m_selected.clear();
        }
        Q_EMIT selectedChanged();
    }
}
UCAction *UCExclusiveGroup::selected() const
{
    return m_selected;
}

bool UCExclusiveGroup::isTriggerValid(UCAction *action, const QVariant &value)
{
    // deselect others.
    if (value.type() == QVariant::Bool && value.toBool() == false) {

        int trueCount = 0;
        Q_FOREACH(UCAction* fromList, list()) {
            if (fromList->parameterType() != UCAction::Bool ||
                action == fromList) {
                continue;
            }
            trueCount += fromList->state().toBool();
        }

        if (trueCount == 0) {
            return false;
        }
    }
    return true;
}
