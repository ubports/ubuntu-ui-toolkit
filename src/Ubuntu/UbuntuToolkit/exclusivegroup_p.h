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

#ifndef EXCLUSIVEGROUP_H
#define EXCLUSIVEGROUP_H

#include "actionlist_p.h"
#include "ubuntutoolkitglobal.h"

#include <QMetaMethod>
#include <QPointer>

class QSignalMapper;

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT ExclusiveGroup : public ActionList
{
    Q_OBJECT
    Q_PROPERTY(QObject* current READ current NOTIFY currentChanged)

public:
    explicit ExclusiveGroup(QObject *parent = 0);

    QObject* current() const;

    Q_INVOKABLE void bindCheckable(QObject* object);
    Q_INVOKABLE void unbindCheckable(QObject* object);

Q_SIGNALS:
    void currentChanged();

protected Q_SLOTS:
    void onActionAdded(UCAction* action);
    void onActionRemoved(UCAction* action);

private:
    void setCurrent(QObject* action);

    QSignalMapper* m_signalMapper;
    QPointer<QObject> m_current;
    QMetaMethod m_updateCurrentMethod;
    bool m_entranceGuard;
};

UT_NAMESPACE_END

#endif // EXCLUSIVEGROUP_H
