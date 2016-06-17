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

#include "ucperformancemonitor.h"
#include <QtGui/QGuiApplication>

Q_LOGGING_CATEGORY(ucPerformance, "[PERFORMANCE]")

UT_NAMESPACE_BEGIN

static int singleFrameThreshold = 32;
static int multipleFrameThreshold = 17;
static int framesCountThreshold = 10;
static int warningCountThreshold = 30;

// TODO Qt 5.5. switch to qEnvironmentVariableIntValue
static int getenvInt(const char* name, int defaultValue)
{
    if (qEnvironmentVariableIsSet(name)) {
        QByteArray stringValue = qgetenv(name);
        bool ok;
        int value = stringValue.toFloat(&ok);
        return ok ? value : defaultValue;
    } else {
        return defaultValue;
    }
}

UCPerformanceMonitor::UCPerformanceMonitor(QObject* parent) :
    QObject(parent),
    m_framesAboveThreshold(0),
    m_warningCount(0),
    m_window(NULL)
{
    QObject::connect((QGuiApplication*)QGuiApplication::instance(), &QGuiApplication::applicationStateChanged,
                     this, &UCPerformanceMonitor::onApplicationStateChanged);

    singleFrameThreshold = getenvInt("UC_PERFORMANCE_MONITOR_SINGLE_FRAME_THRESHOLD", singleFrameThreshold);
    multipleFrameThreshold = getenvInt("UC_PERFORMANCE_MONITOR_MULTIPLE_FRAME_THRESHOLD", multipleFrameThreshold);
    framesCountThreshold = getenvInt("UC_PERFORMANCE_MONITOR_FRAMES_COUNT_THRESHOLD", framesCountThreshold);
    warningCountThreshold = getenvInt("UC_PERFORMANCE_MONITOR_WARNING_COUNT_THRESHOLD", warningCountThreshold);
}

UCPerformanceMonitor::~UCPerformanceMonitor()
{
}

QQuickWindow* UCPerformanceMonitor::findQQuickWindow()
{
    Q_FOREACH (QWindow *w, QGuiApplication::topLevelWindows()) {
        QQuickWindow* rootWindow = qobject_cast<QQuickWindow*>(w);
        if (rootWindow) {
            return rootWindow;
        }
    }
    return NULL;
}

void UCPerformanceMonitor::onApplicationStateChanged(Qt::ApplicationState state)
{
    if (m_warningCount >= warningCountThreshold && warningCountThreshold != -1) {
        // do not monitor performance if the warning count threshold was reached
        return;
    }

    if (state == Qt::ApplicationActive) {
        connectToWindow(findQQuickWindow());
    } else {
        connectToWindow(NULL);
    }
}

void UCPerformanceMonitor::connectToWindow(QQuickWindow* window)
{
    if (window != m_window) {
        if (m_window != NULL) {
            QObject::disconnect(m_window, &QQuickWindow::beforeSynchronizing,
                                this, &UCPerformanceMonitor::startTimer);
            QObject::disconnect(m_window, &QQuickWindow::afterRendering,
                                this, &UCPerformanceMonitor::stopTimer);
            QObject::disconnect(m_window, &QWindow::destroyed,
                                this, &UCPerformanceMonitor::windowDestroyed);
        }

        m_window = window;

        if (m_window != NULL) {
            QObject::connect(m_window, &QQuickWindow::beforeSynchronizing,
                             this, &UCPerformanceMonitor::startTimer,
                             Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::afterRendering,
                             this, &UCPerformanceMonitor::stopTimer,
                             Qt::DirectConnection);
            QObject::connect(m_window, &QWindow::destroyed,
                             this, &UCPerformanceMonitor::windowDestroyed);
        }
    }
}

void UCPerformanceMonitor::startTimer()
{
    m_timer.start();
}

void UCPerformanceMonitor::stopTimer()
{
    if (!m_timer.isValid()) {
        return;
    }

    const int totalTimeInMs = m_timer.elapsed();
    m_timer.invalidate();

    if (totalTimeInMs >= singleFrameThreshold) {
        qCWarning(ucPerformance, "Last frame took %d ms to render.", totalTimeInMs);
        m_warningCount++;
    }

    if (totalTimeInMs >= multipleFrameThreshold) {
        m_framesAboveThreshold++;
        if (m_framesAboveThreshold >= framesCountThreshold) {
            qCWarning(ucPerformance,
                      "Last %d frames took over %d ms to render (last frame: %d ms)",
                      m_framesAboveThreshold, multipleFrameThreshold, totalTimeInMs);
            m_framesAboveThreshold = 0;
            m_warningCount++;
        }
    } else {
        m_framesAboveThreshold = 0;
    }

    if (m_warningCount >= warningCountThreshold && warningCountThreshold != -1) {
        qCWarning(ucPerformance, "Too many warnings were given. Performance monitoring stops.");
        connectToWindow(NULL);
    }
}

void UCPerformanceMonitor::windowDestroyed()
{
    connectToWindow(NULL);
}

UT_NAMESPACE_END
