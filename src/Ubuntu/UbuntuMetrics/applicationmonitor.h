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

#ifndef APPLICATIONMONITOR_H
#define APPLICATIONMONITOR_H

#include <UbuntuMetrics/ubuntumetricsglobal.h>
#include <UbuntuMetrics/logger.h>
#include <UbuntuMetrics/events.h>
#include <QtCore/QList>
#include <QtCore/QMutex>

class QTimer;
class QQuickWindow;
class LoggingThread;
class ShowFilter;
class UMEventUtils;
class WindowMonitor;

// Monitor a QtQuick application by automatically tracking QtQuick windows and
// process metrics. The metrics gathered can be logged and displayed by an
// overlay rendered on top of each frame.
class UBUNTU_METRICS_EXPORT UMApplicationMonitor
{
public:
    enum MonitoringFlag {
        // Render an overlay of real-time metrics on top of each QtQuick frame.
        Overlay = (1 << 0),
        // Pass all the events to the loggers.
        Logging = (1 << 1)
    };
    Q_DECLARE_FLAGS(MonitoringFlags, MonitoringFlag)

    enum LoggingFilter {
        // Allow process events logging.
        ProcessEvent = (1 << 0),
        // Allow window events logging.
        WindowEvent  = (1 << 1),
        // Allow frame events logging.
        FrameEvent   = (1 << 2),
        // Allow all events logging.
        AllEvents    = (ProcessEvent | WindowEvent | FrameEvent)
    };
    Q_DECLARE_FLAGS(LoggingFilters, LoggingFilter)

    // Start/Stop tracking of QtQuick windows and process metrics. start()
    // returns false in case there's no QCoreApplication instance running or if
    // the monitor has already been started. stop() is automatically called when
    // the QCoreApplication instance is about to be stopped. isActive() returns
    // whether the monitoring is running or not.
    static bool start();
    static void stop();
    static bool isActive();

    // Set flags. Default is Overlay.
    static void setFlags(MonitoringFlags flags);
    static MonitoringFlags flags();

    // Set the time in milliseconds between two updates of events of a given
    // type. -1 to disable updates. Only UMEvent::Process is accepted so far as
    // event type with a default value of 1000. Note that when the overlay is
    // enabled, a process update triggers a frame update.
    static void setUpdateInterval(UMEvent::Type type, int interval);
    static int updateInterval(UMEvent::Type type);

    // Set the loggers. Empty by default, max number of loggers is 8.
    static QList<UMLogger*> loggers();
    static bool installLogger(UMLogger* logger);
    static bool removeLogger(UMLogger* logger, bool free = true);
    static void clearLoggers(bool free = true);

    // Set the logging filter. All events are logged by default.
    static void setLoggingFilter(LoggingFilters filter);
    static LoggingFilters loggingFilter();

    static const int maxMonitors = 16;
    static const int maxLoggers = 8;

private:
    static void startMonitoring(QQuickWindow* window);
    static void startMonitoringLocked(QQuickWindow* window);
    static void stopMonitoring(WindowMonitor* monitor);
    static bool hasMonitor(WindowMonitor* monitor);
    static bool removeMonitor(WindowMonitor* monitor);
    static void update();

    static LoggingThread* m_loggingThread;
    static UMEventUtils* m_eventUtils;
    static QTimer* m_processTimer;
    static ShowFilter* m_showFilter;
    static WindowMonitor* m_monitors[maxMonitors];
    static UMLogger* m_loggers[maxLoggers];
    static QMutex m_monitorsMutex;
    static QMetaObject::Connection m_aboutToQuitConnection;
    static QMetaObject::Connection m_lastWindowClosedConnection;
    static int m_monitorCount;
    static int m_loggerCount;
    static int m_updateInterval[UMEvent::TypeCount];
    static quint16 m_flags;
    static quint8 m_filter;
    static UMEvent m_processEvent;

    // Prevent instantiation, copy and assign.
    UMApplicationMonitor();
    Q_DISABLE_COPY(UMApplicationMonitor);

    void* __reserved;

    friend class WindowMonitor;
    friend class WindowMonitorDeleter;
    friend class WindowMonitorFlagSetter;
    friend class WindowMonitorFilterSetter;
    friend class ShowFilter;
};

#endif  // APPLICATIONMONITOR_H
