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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#ifndef UCPERFORMANCE_MONITOR_H
#define UCPERFORMANCE_MONITOR_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QElapsedTimer>
#include <QtCore/QLoggingCategory>
#include <QtQuick/QQuickWindow>

class UCPerformanceMonitor : public QObject
{
    Q_OBJECT

public:
    explicit UCPerformanceMonitor(QObject* parent = 0);
    ~UCPerformanceMonitor();

private Q_SLOTS:
    void onApplicationStateChanged(Qt::ApplicationState state);
    void connectToWindow(QQuickWindow* window);
    void startTimer();
    void stopTimer();

private:
    QQuickWindow* findQQuickWindow();

private:
    int m_framesAboveThreshold;
    int m_warningCount;
    QSharedPointer<QElapsedTimer> m_timer;
    QQuickWindow* m_window;
};

Q_DECLARE_LOGGING_CATEGORY(ucPerformance)

#endif // UCPERFORMANCE_MONITOR_H
