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

#include "upmperformancemonitor.h"
#include <QtCore/qmath.h>
#include <QtGui/QGuiApplication>

Q_LOGGING_CATEGORY(ucPerformance, "[PERFORMANCE]")

UPMPerformanceMonitor::UPMPerformanceMonitor(QObject* parent) :
    QObject(parent),
    m_framesAboveThreshold(0),
    m_warningCount(0),
    m_timer(NULL),
    m_window(NULL)
{
    QObject::connect((QGuiApplication*)QGuiApplication::instance(), &QGuiApplication::applicationStateChanged,
                     this, &UPMPerformanceMonitor::onApplicationStateChanged);
}

QQuickWindow* UPMPerformanceMonitor::findQQuickWindow()
{
    Q_FOREACH (QWindow *w, QGuiApplication::topLevelWindows()) {
        QQuickWindow* rootWindow = qobject_cast<QQuickWindow*>(w);
        if (rootWindow) {
            return rootWindow;
        }
    }
    return NULL;
}

void UPMPerformanceMonitor::onApplicationStateChanged(Qt::ApplicationState state)
{
    if (state == Qt::ApplicationActive) {
        connectToWindow(findQQuickWindow());
    } else {
        connectToWindow(NULL);
    }
}

void UPMPerformanceMonitor::connectToWindow(QQuickWindow* window)
{
    if (window != m_window) {
        if (m_window != NULL) {
            QObject::disconnect(m_window, &QQuickWindow::beforeSynchronizing,
                                this, &UPMPerformanceMonitor::onBeforeSynchronizing);
            QObject::disconnect(m_window, &QQuickWindow::afterRendering,
                                this, &UPMPerformanceMonitor::onAfterRendering);
        }

        m_window = window;

        if (m_window != NULL) {
            QObject::connect(m_window, &QQuickWindow::beforeSynchronizing,
                             this, &UPMPerformanceMonitor::onBeforeSynchronizing,
                             Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::afterRendering,
                             this, &UPMPerformanceMonitor::onAfterRendering,
                             Qt::DirectConnection);
        }
    }
}

void UPMPerformanceMonitor::onBeforeSynchronizing()
{
    if (!m_timer) {
        m_timer.reset(new QElapsedTimer);
    }
    m_timer->start();
}

void UPMPerformanceMonitor::onAfterRendering()
{
    int singleFrameThreshold = 50;
    int multipleFrameThreshold = 10;
    int framesCountThreshold = 10;
    int warningCountThreshold = 30;

    int totalTimeInMs = m_timer->elapsed();
    m_timer->invalidate();

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

    if (m_warningCount >= warningCountThreshold) {
        qCWarning(ucPerformance, "Too many warnings were given. Performance monitoring stops.");
        connectToWindow(NULL);
    }
}
