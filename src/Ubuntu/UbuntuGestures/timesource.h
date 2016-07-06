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

#ifndef UBUNTUGESTURES_TIMESOURCE_H
#define UBUNTUGESTURES_TIMESOURCE_H

#include "ubuntugesturesglobal.h"
#include <QSharedPointer>

UG_NAMESPACE_BEGIN
/*
    Interface for a time source.
 */
class UBUNTUGESTURES_EXPORT TimeSource {
public:
    virtual ~TimeSource() {}
    /* Returns the current time in milliseconds since some reference time in the past. */
    virtual qint64 msecsSinceReference() = 0;
};
typedef QSharedPointer<TimeSource> SharedTimeSource;

/*
    Implementation of a time source
 */
class RealTimeSourcePrivate;
class UBUNTUGESTURES_EXPORT RealTimeSource : public TimeSource {
public:
    RealTimeSource();
    virtual ~RealTimeSource();
    qint64 msecsSinceReference() override;
private:
    RealTimeSourcePrivate *d;
};

/*
    A fake time source, useful for tests
 */
class FakeTimeSource : public TimeSource {
public:
    FakeTimeSource() { m_msecsSinceReference = 0; }
    qint64 msecsSinceReference() override { return m_msecsSinceReference; }
    qint64 m_msecsSinceReference;
};

UG_NAMESPACE_END

#endif // UBUNTUGESTURES_TIMESOURCE_H
