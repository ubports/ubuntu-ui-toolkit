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

#ifndef UCEXCLUSIVEGROUP_H
#define UCEXCLUSIVEGROUP_H

#include "ucactionlist.h"
#include "ucaction.h"
#include <QQmlParserStatus>

class UCExclusiveGroup : public UCActionList
{
    Q_OBJECT
    Q_PROPERTY(UCAction* selected READ selected NOTIFY selectedChanged)

public:
    explicit UCExclusiveGroup(QObject *parent = 0);

    UCAction* selected() const;
    bool isTriggerValid(UCAction* action, const QVariant& value);

Q_SIGNALS:
    void selectedChanged();

protected Q_SLOTS:
    void onActionAdded(UCAction* action);
    void onActionRemoved(UCAction* action);

private:
    void stateChanged(UCAction* action);
    void setSelected(UCAction* action);

    QPointer<UCAction> m_selected;
    bool m_entranceGuard;
};

#endif // UCEXCLUSIVEGROUP_H
