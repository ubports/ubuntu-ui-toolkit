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

#ifndef UCSTATESAVER_P_H
#define UCSTATESAVER_P_H

#include "ucstatesaver.h"

#include <QStringList>

namespace UbuntuToolkit {

class UCStateSaverAttachedPrivate
{
    Q_DECLARE_PUBLIC(UCStateSaverAttached)
public:
    UCStateSaverAttachedPrivate(UCStateSaverAttached *qq, QObject *attachee);

    UCStateSaverAttached *q_ptr;
    QObject *m_attachee;
    bool m_enabled:1;
    QString m_id;
    QString m_absoluteId;
    QStringList m_properties;

    QString absoluteId(const QString &id);
    void restore();
    void watchComponent(bool watch);

    void _q_init();
    void _q_save();
    void _q_globalEnableChanged(bool);
};

UT_NAMESPACE_END

#endif // UCSTATESAVER_P_H
