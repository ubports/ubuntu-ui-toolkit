/*
 * Copyright 2015 Canonical Ltd.
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
 * Authored by: Daniel d'Andrada <daniel.dandrada@canonical.com>
 */

#include <UbuntuGestures/private/timesource_p.h>

#include <QElapsedTimer>

UG_NAMESPACE_BEGIN

class RealTimeSourcePrivate {
public:
    QElapsedTimer timer;
};

RealTimeSource::RealTimeSource()
    : UG_PREPEND_NAMESPACE(TimeSource())
    , d(new RealTimeSourcePrivate)
{
    d->timer.start();
}

RealTimeSource::~RealTimeSource()
{
    delete d;
}

qint64 RealTimeSource::msecsSinceReference()
{
    return d->timer.elapsed();
}

UG_NAMESPACE_END
