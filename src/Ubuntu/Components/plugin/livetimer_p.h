/*
 * Copyright 2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIVETIMER_P_H
#define LIVETIMER_P_H

#include "livetimer.h"

#include <QTimer>

class SharedLiveTimer : public QObject
{
    Q_OBJECT
public:
    SharedLiveTimer(QObject* parent = NULL);

    static SharedLiveTimer& instance()
    {
        static SharedLiveTimer instance;
        return instance;
    }

    void registerTimer(LiveTimer* timer);
    void unregisterTimer(LiveTimer* timer);

private Q_SLOTS:
    void timeout();
    void timedate1PropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList&);

Q_SIGNALS:
    void trigger();

private:
    void updateFrequency();
    void reInitTimer();

    QList<LiveTimer*> m_liveTimers;
    QTimer m_timer;
    LiveTimer::Frequency m_frequency;

    QDateTime m_nextUpdate;
    QDateTime m_lastUpdate;
};

#endif // LIVETIMER_P_H
