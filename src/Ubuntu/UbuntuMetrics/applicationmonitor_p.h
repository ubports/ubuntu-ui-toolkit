// Copyright © 2016 Canonical Ltd.
// Author: Loïc Molinari <loic.molinari@canonical.com>
//
// This file is part of Ubuntu UI Toolkit.
//
// Ubuntu UI Toolkit is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; version 3.
//
// Ubuntu UI Toolkit is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ubuntu UI Toolkit. If not, see <http://www.gnu.org/licenses/>.

#ifndef APPLICATIONMONITOR_P_H
#define APPLICATIONMONITOR_P_H

#include "applicationmonitor.h"
#include "events.h"
#include "overlay_p.h"
#include "gputimer_p.h"
#include "ubuntumetricsglobal_p.h"
#include <QtCore/QHash>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QElapsedTimer>
#include <QtCore/QRunnable>
#include <QtCore/QAtomicInteger>

// Extension to UMApplicationMonitor flags.
enum MonitorFlagEx {
    Started = (1 << 8)
};

class LoggingThread : public QThread
{
public:
    LoggingThread();

    void run() override;
    void push(const UMEvent* event);
    void setLoggers(UMLogger** loggers, int count);
    LoggingThread* ref();
    void deref();

private:
    ~LoggingThread();

    enum {
        Waiting       = (1 << 0),
        JoinRequested = (1 << 1)
    };

    UMEvent* m_queue;
    UMLogger* m_loggers[UMApplicationMonitor::maxLoggers];
    int m_loggerCount;
    QMutex m_mutex;
    QWaitCondition m_condition;
    QAtomicInteger<quint32> m_refCount;
    qint8 m_queueIndex;
    qint8 m_queueSize;
    quint8 m_flags;
};

class ShowFilter : public QObject
{
    Q_OBJECT

private:
    bool eventFilter(QObject* object, QEvent* event) override;
};

class WindowMonitorDeleter : public QRunnable
{
public:
    WindowMonitorDeleter(WindowMonitor* monitor)
        : m_monitor(monitor) { DASSERT(monitor); }
    ~WindowMonitorDeleter();

    void run() override;

private:
    WindowMonitor* m_monitor;
};

class WindowMonitorFlagSetter : public QRunnable
{
public:
    WindowMonitorFlagSetter(WindowMonitor* monitor, quint8 flags)
        : m_monitor(monitor), m_flags(flags) { DASSERT(monitor); }
    ~WindowMonitorFlagSetter();

    void run() override {}

private:
    WindowMonitor* m_monitor;
    quint8 m_flags;
};

class WindowMonitorFilterSetter : public QRunnable
{
public:
    WindowMonitorFilterSetter(WindowMonitor* monitor, quint8 filters)
        : m_monitor(monitor), m_filters(filters) { DASSERT(monitor); }
    ~WindowMonitorFilterSetter();

    void run() override {}

private:
    WindowMonitor* m_monitor;
    quint8 m_filters;
};

class WindowMonitor : public QObject
{
    Q_OBJECT

public:
    WindowMonitor(
        QQuickWindow* window, LoggingThread* loggingThread, quint8 flags, quint8 filters,
        quint32 id);
    ~WindowMonitor();

    QQuickWindow* window() const { return m_window; }
    void setProcessEvent(const UMEvent& event);

private Q_SLOTS:
    void windowSceneGraphInitialised();
    void windowSceneGraphInvalidated();
    void windowBeforeSynchronising();
    void windowAfterSynchronising();
    void windowBeforeRendering();
    void windowAfterRendering();
    void windowFrameSwapped();
    void windowSceneGraphAboutToStop();

private:
    enum {
        GpuResourcesInitialised = (1 << 8),
        GpuTimerAvailable       = (1 << 9),
        SizeChanged             = (1 << 10)
    };

    bool gpuResourcesInitialised() const { return m_flags & GpuResourcesInitialised; }
    void setFlags(quint16 flags) { m_flags = flags | (m_flags & ~0xff); }
    void setFilters(quint8 filters) { m_filters = filters; }
    void initialiseGpuResources();
    void finaliseGpuResources();

    LoggingThread* m_loggingThread;
    QQuickWindow* m_window;
    GPUTimer m_gpuTimer;
    Overlay m_overlay;  // Accessed from different threads (needs locking).
    QMutex m_mutex;
    QElapsedTimer m_sceneGraphTimer;
    QElapsedTimer m_deltaTimer;
    quint32 m_id;
    quint16 m_flags;
    quint8 m_filters;
    QSize m_frameSize;
    UMEvent m_frameEvent;

    friend class WindowMonitorDeleter;
    friend class WindowMonitorFlagSetter;
    friend class WindowMonitorFilterSetter;
};

#endif  // APPLICATIONMONITOR_P_H
