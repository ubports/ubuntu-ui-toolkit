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

#include "applicationmonitor_p.h"
#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickWindow>

// FIXME(loicm) When a monitored window is destroyed and if there's a window
//     that's not monitored because the max count was reached, enable monitoring
//     on it if possible.

const int logQueueSize = 16;
const int logQueueAlignment = 64;

LoggingThread::LoggingThread()
    : m_loggerCount(0)
    , m_refCount(1)
    , m_queueIndex(0)
    , m_queueSize(0)
    , m_flags(0)
{
    m_queue = static_cast<UMEvent*>(
        aligned_alloc(logQueueAlignment, logQueueSize * sizeof(UMEvent)));

#if !defined(QT_NO_DEBUG)
    setObjectName(QString::fromLatin1("UbuntuMetrics logging", 21));  // Thread name.
#endif
    start();
}

LoggingThread::~LoggingThread()
{
    m_mutex.lock();
    m_flags |= JoinRequested;
    if (m_flags & Waiting) {
        m_condition.wakeOne();
    }
    m_mutex.unlock();
    wait();

    free(m_queue);
}

#define BREAK_ON_JOIN_REQUEST()                \
    if (Q_UNLIKELY(m_flags & JoinRequested)) { \
        m_mutex.unlock();                      \
        break;                                 \
    }

// Logging thread entry point.
void LoggingThread::run()
{
    DLOG("Entering logging thread.");
    while (true) {
        // Wait for new events in the log queue.
        m_mutex.lock();
        DASSERT(m_queueSize >= 0);
        if (m_queueSize == 0) {
            BREAK_ON_JOIN_REQUEST();
            m_flags |= Waiting;
            m_condition.wait(&m_mutex);
            if (m_queueSize == 0) {
                BREAK_ON_JOIN_REQUEST();
            }
            m_flags &= ~Waiting;
        }

        // Unqueue oldest events from the log queue.
        DASSERT(m_queueSize > 0);
        UMEvent event;
        memcpy(&event, &m_queue[m_queueIndex], sizeof(UMEvent));
        m_queueIndex = (m_queueIndex + 1) % logQueueSize;
        m_queueSize--;

        // Log.
        const int loggerCount = m_loggerCount;
        UMLogger* loggers[UMApplicationMonitor::maxLoggers];
        memcpy(loggers, m_loggers, loggerCount * sizeof(UMLogger*));
        m_mutex.unlock();
        for (int i = 0; i < loggerCount; ++i) {
            loggers[i]->log(event);
        }
    }
    DLOG("Leaving logging thread.");
}

void LoggingThread::push(const UMEvent* event)
{
    // Ensure the log queue is not full.
    m_mutex.lock();
    DASSERT(m_queueSize <= logQueueSize);
    while (m_queueSize == logQueueSize) {
        m_mutex.unlock();
        QThread::yieldCurrentThread();
        m_mutex.lock();
    }

    // Push event to the log queue.
    DASSERT(m_queueSize < logQueueSize);
    memcpy(&m_queue[(m_queueIndex + m_queueSize++) % logQueueSize], event, sizeof(UMEvent));
    if (m_flags & Waiting) {
        m_condition.wakeOne();
    }
    m_mutex.unlock();
}

void LoggingThread::setLoggers(UMLogger** loggers, int count)
{
    DASSERT(count >= 0);
    DASSERT(count <= UMApplicationMonitor::maxLoggers);

    QMutexLocker locker(&m_mutex);
    memcpy(m_loggers, loggers, count * sizeof(UMLogger*));
    m_loggerCount = count;
}

LoggingThread* LoggingThread::ref()
{
    m_refCount.ref();
    return this;
}

void LoggingThread::deref()
{
    if (m_refCount.deref() == 0) {
        if (thread() == QThread::currentThread()) {
            delete this;
        } else {
            deleteLater();
        }
    }
}

bool ShowFilter::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::Show) {
        if (QQuickWindow* window = qobject_cast<QQuickWindow*>(object)) {
            UMApplicationMonitor::startMonitoring(window);
        }
    }
    return QObject::eventFilter(object, event);
}

LoggingThread* UMApplicationMonitor::m_loggingThread = nullptr;
UMEventUtils* UMApplicationMonitor::m_eventUtils = nullptr;
QTimer* UMApplicationMonitor::m_updateTimer = nullptr;
ShowFilter* UMApplicationMonitor::m_showFilter = nullptr;
WindowMonitor* UMApplicationMonitor::m_monitors[UMApplicationMonitor::maxMonitors];
UMLogger* UMApplicationMonitor::m_loggers[UMApplicationMonitor::maxLoggers];
QMetaObject::Connection UMApplicationMonitor::m_aboutToQuitConnection;
QMetaObject::Connection UMApplicationMonitor::m_lastWindowClosedConnection;
QMutex UMApplicationMonitor::m_monitorsMutex;
int UMApplicationMonitor::m_monitorCount = 0;
int UMApplicationMonitor::m_loggerCount = 0;
int UMApplicationMonitor::m_updateInterval = 1000;
quint16 UMApplicationMonitor::m_flags =
    UMApplicationMonitor::Overlay | UMApplicationMonitor::Logging;
quint8 UMApplicationMonitor::m_filters = UMApplicationMonitor::AllEvents;
alignas(64) UMEvent UMApplicationMonitor::m_processEvent;

// static.
void UMApplicationMonitor::startMonitoring(QQuickWindow* window)
{
    DASSERT(window);
    DASSERT(m_loggingThread);

    if (m_monitorCount < maxMonitors) {
        DASSERT(m_monitors[m_monitorCount] == nullptr);
        static quint32 id = 0;
        // flags() used here instead of m_flags to clean up extended flags.
        m_monitors[m_monitorCount] =
            new WindowMonitor(window, m_loggingThread->ref(), flags(), m_filters, ++id);
        m_monitors[m_monitorCount]->setProcessEvent(m_processEvent);
        m_monitorCount++;
    } else {
        WARN("ApplicationMonitor: Can't monitor more than %d QQuickWindows.", maxMonitors);
    }
}

// static.
bool UMApplicationMonitor::start()
{
    if (!QCoreApplication::instance()) {
        WARN("ApplicationMonitor: There's no QCoreApplication instance running!");
        return false;
    }
    if (m_flags & Started) {
        WARN("ApplicationMonitor: Already started!");
        return false;
    }

    m_lastWindowClosedConnection = QObject::connect(
        static_cast<QGuiApplication*>(QCoreApplication::instance()),
        &QGuiApplication::lastWindowClosed, &stop);

    // So that we don't crash when QCoreApplication::quit() is called.
    m_aboutToQuitConnection = QObject::connect(
        static_cast<QGuiApplication*>(QCoreApplication::instance()),
        &QCoreApplication::aboutToQuit, &stop);

    DASSERT(!m_loggingThread);
    m_loggingThread = new LoggingThread;
    m_loggingThread->setLoggers(m_loggers, m_loggerCount);

    QWindowList windows = QGuiApplication::allWindows();
    const int size = windows.size();
    m_monitorsMutex.lock();
    for (int i = 0; i < size; ++i) {
        if (QQuickWindow* window = qobject_cast<QQuickWindow*>(windows[i])) {
            if (window->isVisible()) {
                startMonitoring(window);
            }
        }
    }
    m_monitorsMutex.unlock();

    DASSERT(!m_showFilter);
    m_showFilter = new ShowFilter;
    QGuiApplication::instance()->installEventFilter(m_showFilter);

    DASSERT(!m_eventUtils);
    m_eventUtils = new UMEventUtils;

    DASSERT(!m_updateTimer);
    m_updateTimer = new QTimer;
    QObject::connect(m_updateTimer, &QTimer::timeout, &update);
    m_updateTimer->setInterval(m_updateInterval);
    if (m_flags & (Logging | Overlay)) {
        m_updateTimer->start();
    }
    memset(&m_processEvent, 0, sizeof(UMEvent));
    update();

    m_flags |= Started;
    return true;
}

WindowMonitorDeleter::~WindowMonitorDeleter()
{
    // Remove the monitor from the list making sure it's not been removed
    // (window going hidden) after this runnable was scheduled.
    if (UMApplicationMonitor::removeMonitor(m_monitor)) {
        delete m_monitor;
    }
}

void WindowMonitorDeleter::run()
{
    // run() guarantees a valid context.
    if (m_monitor->gpuResourcesInitialised()) {
        m_monitor->finaliseGpuResources();
    }
}

// static.
void UMApplicationMonitor::stopMonitoring(WindowMonitor* monitor)
{
    DASSERT(monitor);
    DASSERT(monitor->window());

    // To ensure there's no signals emitted on the render thread while and after
    // window monitor deletion, we schedule a render job that disconnects the
    // signals on the render thread and then request deletion on the right
    // thread with deleteLater().
    monitor->window()->scheduleRenderJob(new WindowMonitorDeleter(monitor),
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
        QQuickWindow::NoStage);
#else
        QQuickWindow::BeforeSynchronizingStage);
    monitor->window()->update();  // Wake up the render loop.
#endif
}

// static.
bool UMApplicationMonitor::hasMonitor(WindowMonitor* monitor)
{
    DASSERT(monitor);
    DASSERT(monitor->window());

    m_monitorsMutex.lock();
    for (int i = 0; i < m_monitorCount; ++i) {
        if (m_monitors[i] == monitor) {
            m_monitorsMutex.unlock();
            return true;
        }
    }
    m_monitorsMutex.unlock();

    return false;
}

// static.
bool UMApplicationMonitor::removeMonitor(WindowMonitor* monitor)
{
    DASSERT(monitor);
    DASSERT(monitor->window());

    m_monitorsMutex.lock();
    for (int i = 0; i < m_monitorCount; ++i) {
        if (m_monitors[i] == monitor) {
            m_monitorCount--;
            if (i < m_monitorCount) {
                memmove(&m_monitors[i], &m_monitors[i+1],
                        (m_monitorCount-i) * sizeof(WindowMonitor*));
            }
#if !defined(QT_NO_DEBUG)
            m_monitors[m_monitorCount] = nullptr;
#endif
            m_monitorsMutex.unlock();
            return true;
        }
    }
    m_monitorsMutex.unlock();

    return false;
}

// static.
void UMApplicationMonitor::stop()
{
    if (!QCoreApplication::instance()) {
        WARN("ApplicationMonitor: There's no QCoreApplication instance running!");
        return;
    }
    if (!(m_flags & Started)) {
        WARN("ApplicationMonitor: Already stopped!");
        return;
    }

    m_monitorsMutex.lock();
    for (int i = 0; i < m_monitorCount; ++i) {
        stopMonitoring(m_monitors[i]);
    }
    m_monitorsMutex.unlock();

    QObject::disconnect(m_aboutToQuitConnection);
    QObject::disconnect(m_lastWindowClosedConnection);

    DASSERT(m_updateTimer);
    delete m_updateTimer;
    m_updateTimer = nullptr;

    DASSERT(m_eventUtils);
    delete m_eventUtils;
    m_eventUtils = nullptr;

    DASSERT(m_showFilter);
    QGuiApplication::instance()->removeEventFilter(m_showFilter);
    delete m_showFilter;
    m_showFilter = nullptr;

    DASSERT(m_loggingThread);
    m_loggingThread->deref();
    m_loggingThread = nullptr;

    // Wait for completion.
    m_monitorsMutex.lock();
    while (m_monitorCount > 0) {
        // FIXME(loicm) We could yield the thread here in the case of a threaded
        //     QtQuick renderer.
        m_monitorsMutex.unlock();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        m_monitorsMutex.lock();
    }
    m_monitorsMutex.unlock();

    m_flags &= ~Started;
}

// static.
bool UMApplicationMonitor::isActive()
{
    return !!(m_flags & Started);
}

WindowMonitorFlagSetter::~WindowMonitorFlagSetter()
{
    // Make sure it's not been removed (window going hidden) after this runnable
    // was scheduled.
    if (UMApplicationMonitor::hasMonitor(m_monitor)) {
        m_monitor->setFlags(m_flags);
    }
}

// static.
void UMApplicationMonitor::setFlags(MonitorFlags flags)
{
    m_flags = flags | (m_flags & ~0xff);  // Keep extended flags.
    if (!(m_flags & Started)) {
        return;
    }

    if (m_updateInterval >= 0 && flags & (Overlay | Logging)) {
        m_updateTimer->start();
    } else {
        m_updateTimer->stop();
    }

    m_monitorsMutex.lock();
    for (int i = 0; i < m_monitorCount; ++i) {
        DASSERT(m_monitors[i]);
        DASSERT(m_monitors[i]->window());
        m_monitors[i]->window()->scheduleRenderJob(
            new WindowMonitorFlagSetter(m_monitors[i], flags),
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
            QQuickWindow::NoStage);
        if (m_flags & ContinuousUpdates) {
            m_monitors[i]->window()->update();
        }
#else
            QQuickWindow::BeforeSynchronizingStage);
        m_monitors[i]->window()->update();  // Wake up the render loop.
#endif
    }
    m_monitorsMutex.unlock();
}

// static.
UMApplicationMonitor::MonitorFlags UMApplicationMonitor::flags()
{
    return static_cast<MonitorFlags>(m_flags & 0xff);  // Reset extended flags.
}

// static.
void UMApplicationMonitor::setUpdateInterval(int interval)
{
    interval = qBound(100, interval, 10000);
    if (interval != m_updateInterval) {
        if (interval >= 0) {
            if (m_flags & Started && m_flags & (Overlay | Logging)) {
                m_updateTimer->setInterval(interval);
                if (m_updateInterval < 0) {
                    m_updateTimer->start();
                }
            }
        } else if (m_updateInterval >= 0 && m_flags & Started && m_flags & (Overlay | Logging)) {
            m_updateTimer->stop();
        }
        m_updateInterval = interval;
    }
}

// static.
int UMApplicationMonitor::updateInterval()
{
    return m_updateInterval;
}

// static.
QList<UMLogger*> UMApplicationMonitor::loggers()
{
    QList<UMLogger*> list;
    for (int i = 0; i < m_loggerCount; ++i) {
        list.append(m_loggers[i]);
    }
    return list;
}

// static.
bool UMApplicationMonitor::installLogger(UMLogger* logger)
{
    if (m_loggerCount < maxLoggers && logger) {
        m_loggers[m_loggerCount++] = logger;
        if (m_flags & Started) {
            DASSERT(m_loggingThread);
            m_loggingThread->setLoggers(m_loggers, m_loggerCount);
        }
        return true;
    } else {
        return false;
    }
}

// static.
bool UMApplicationMonitor::removeLogger(UMLogger* logger, bool free)
{
    if (!logger) {
        return false;
    }

    for (int i = m_loggerCount; i > 0; --i) {
        if (m_loggers[i] == logger) {
            if (i < --m_loggerCount) {
                m_loggers[i] = m_loggers[m_loggerCount];
            }
            if (m_flags & Started) {
                DASSERT(m_loggingThread);
                m_loggingThread->setLoggers(m_loggers, m_loggerCount);
            }
            if (free) {
                delete logger;
            }
            return true;
        }
    }
    return false;
}

// static.
void UMApplicationMonitor::clearLoggers(bool free)
{
    if (free) {
        for (int i = 0; i < m_loggerCount; ++i) {
            delete m_loggers[i];
        }
    }
    m_loggerCount = 0;
}

WindowMonitorFilterSetter::~WindowMonitorFilterSetter()
{
    // Make sure it's not been removed (window going hidden) after this runnable
    // was scheduled.
    if (UMApplicationMonitor::hasMonitor(m_monitor)) {
        m_monitor->setFilters(m_filters);
    }
}

// static.
void UMApplicationMonitor::setLoggingFilters(UMApplicationMonitor::LoggingFilters filters)
{
    m_filters = filters;
    if (!(m_flags & Started)) {
        return;
    }

    m_monitorsMutex.lock();
    for (int i = 0; i < m_monitorCount; ++i) {
        DASSERT(m_monitors[i]);
        DASSERT(m_monitors[i]->window());
        m_monitors[i]->window()->scheduleRenderJob(
            new WindowMonitorFilterSetter(m_monitors[i], filters),
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
            QQuickWindow::NoStage);
#else
            QQuickWindow::BeforeSynchronizingStage);
        m_monitors[i]->window()->update();  // Wake up the render loop.
#endif
    }
    m_monitorsMutex.unlock();
}

// static.
UMApplicationMonitor::LoggingFilters UMApplicationMonitor::loggingFilters()
{
    return static_cast<LoggingFilters>(m_filters);
}

// static.
void UMApplicationMonitor::update()
{
    DASSERT(m_eventUtils);
    DASSERT(m_loggingThread);

    const bool processLogging = (m_flags & Logging) && (m_filters & ProcessEvent);
    const bool overlay = m_flags & Overlay;

    if (processLogging || overlay) {
        m_eventUtils->updateProcessEvent(&m_processEvent);
        if (processLogging) {
            m_loggingThread->push(&m_processEvent);
        }
        if (overlay) {
            m_monitorsMutex.lock();
            for (int i = 0; i < m_monitorCount; ++i) {
                DASSERT(m_monitors[i]);
                m_monitors[i]->setProcessEvent(m_processEvent);
            }
            m_monitorsMutex.unlock();
        }
    }
}

static const char* const defaultOverlayText =
    "%qtVersion (%qtPlatform) - %glVersion\n"
    "%cpuModel\n"  // FIXME(loicm) Should be included by default?
    "%gpuModel\r"  // FIXME(loicm) Should be included by default?
    "    Window : %9windowId   \n"
    "      Size : %9windowSize px\r"
    "     Frame : %9frameNumber   \n"
    // FIXME(loicm) should be removed once we have a timing histogram with swap included.
    " Delta n-1 : %9deltaTime ms\n"
    "  SG sync. : %9syncTime ms\n"
    " SG render : %9renderTime ms\n"
    "       GPU : %9gpuTime ms\n"
    "     Total : %9totalTime ms\r"
    "  VSZ mem. : %9vszMemory kB\n"
    "  RSS mem. : %9rssMemory kB\n"
    "   Threads : %9threadCount   \n"
    " CPU usage : %9cpuUsage %% ";

WindowMonitor::WindowMonitor(
    QQuickWindow* window, LoggingThread* loggingThread, quint8 flags, quint8 filters, quint32 id)
    : m_loggingThread(loggingThread)
    , m_window(window)
    , m_overlay(defaultOverlayText, id)
    , m_id(id)
    , m_flags(flags)
    , m_filters(filters)
    , m_frameSize(window->width(), window->height())
{
    DASSERT(window);
    DASSERT(loggingThread);

    moveToThread(nullptr);

    QObject::connect(window, SIGNAL(sceneGraphInitialized()), this,
                     SLOT(windowSceneGraphInitialised()), Qt::DirectConnection);
    QObject::connect(window, SIGNAL(sceneGraphInvalidated()), this,
                     SLOT(windowSceneGraphInvalidated()), Qt::DirectConnection);
    QObject::connect(window, SIGNAL(beforeSynchronizing()), this,
                     SLOT(windowBeforeSynchronising()), Qt::DirectConnection);
    QObject::connect(window, SIGNAL(afterSynchronizing()), this,
                     SLOT(windowAfterSynchronising()), Qt::DirectConnection);
    QObject::connect(window, SIGNAL(beforeRendering()), this, SLOT(windowBeforeRendering()),
                     Qt::DirectConnection);
    QObject::connect(window, SIGNAL(afterRendering()), this, SLOT(windowAfterRendering()),
                     Qt::DirectConnection);
    QObject::connect(window, SIGNAL(frameSwapped()), this, SLOT(windowFrameSwapped()),
                     Qt::DirectConnection);
    QObject::connect(window, SIGNAL(sceneGraphAboutToStop()), this,
                     SLOT(windowSceneGraphAboutToStop()), Qt::DirectConnection);

    memset(&m_frameEvent, 0, sizeof(m_frameEvent));
    m_frameEvent.type = UMEvent::Frame;
    m_frameEvent.frame.window = id;

    if ((flags & UMApplicationMonitor::Logging) &&
        (filters & UMApplicationMonitor::WindowEvent)) {
        UMEvent event;
        event.type = UMEvent::Window;
        event.timeStamp = UMEventUtils::timeStamp();
        event.window.id = id;
        event.window.width = m_frameSize.width();
        event.window.height = m_frameSize.height();
        event.window.state = UMWindowEvent::Shown;
        loggingThread->push(&event);
    }
}

WindowMonitor::~WindowMonitor()
{
    DASSERT(!(m_flags & GpuResourcesInitialised));

    if ((m_flags & UMApplicationMonitor::Logging) &&
        (m_filters & UMApplicationMonitor::WindowEvent)) {
        UMEvent event;
        event.type = UMEvent::Window;
        event.timeStamp = UMEventUtils::timeStamp();
        event.window.id = m_id;
        event.window.width = m_frameSize.width();
        event.window.height = m_frameSize.height();
        event.window.state = UMWindowEvent::Hidden;
        m_loggingThread->push(&event);
    }

    m_loggingThread->deref();
}

void WindowMonitor::initialiseGpuResources()
{
    DASSERT(!(m_flags & GpuResourcesInitialised));

    // FIXME(loicm) We should actually provide an API call to let the user set
    //     that behaviour programmatically.
    static bool noGpuTimer = qEnvironmentVariableIsSet("UM_NO_GPU_TIMER");

    m_overlay.initialise();
    m_gpuTimer.initialise();
    m_frameEvent.frame.number = 0;
    m_flags |= GpuResourcesInitialised | (!noGpuTimer ? GpuTimerAvailable : 0);
}

void WindowMonitor::windowSceneGraphInitialised()
{
    if (!(m_flags & GpuResourcesInitialised)) {
        initialiseGpuResources();
    }
}

void WindowMonitor::finaliseGpuResources()
{
    DASSERT(m_flags & GpuResourcesInitialised);

    if (m_flags & GpuTimerAvailable) {
        m_gpuTimer.finalise();
    }
    m_overlay.finalise();

    m_frameEvent.frame.number = 0;
    m_flags &= ~(GpuResourcesInitialised | GpuTimerAvailable);
}

void WindowMonitor::windowSceneGraphInvalidated()
{
    if (m_flags & GpuResourcesInitialised) {
        finaliseGpuResources();
    }
}

void WindowMonitor::windowBeforeSynchronising()
{
    if (m_flags & GpuResourcesInitialised) {
        m_sceneGraphTimer.start();
    }
}

void WindowMonitor::windowAfterSynchronising()
{
    if (m_flags & GpuResourcesInitialised) {
        m_frameEvent.frame.syncTime = m_sceneGraphTimer.nsecsElapsed();
    }
}

void WindowMonitor::windowBeforeRendering()
{
    const QSize frameSize = m_window->size();
    if (frameSize != m_frameSize) {
        m_frameSize = frameSize;
        if ((m_flags & UMApplicationMonitor::Logging) &&
            (m_filters & UMApplicationMonitor::WindowEvent)) {
            UMEvent event;
            event.type = UMEvent::Window;
            event.timeStamp = UMEventUtils::timeStamp();
            event.window.id = m_id;
            event.window.width = frameSize.width();
            event.window.height = frameSize.height();
            event.window.state = UMWindowEvent::Resized;
            m_loggingThread->push(&event);
        }
    }

    if (m_flags & GpuResourcesInitialised) {
        m_sceneGraphTimer.start();
        if (m_flags & GpuTimerAvailable) {
            m_gpuTimer.start();
        }
    }
}

void WindowMonitor::windowAfterRendering()
{
    if (m_flags & GpuResourcesInitialised) {
        m_frameEvent.frame.renderTime = m_sceneGraphTimer.nsecsElapsed();
        m_frameEvent.frame.gpuTime = (m_flags & GpuTimerAvailable) ? m_gpuTimer.stop() : 0;
        m_frameEvent.frame.number++;
        if (m_flags & UMApplicationMonitor::Overlay) {
            m_mutex.lock();
            m_overlay.render(m_frameEvent, m_frameSize);
            m_mutex.unlock();
        }
        m_sceneGraphTimer.start();
    }
}

void WindowMonitor::windowFrameSwapped()
{
    if (m_flags & GpuResourcesInitialised) {
        m_frameEvent.frame.deltaTime = m_deltaTimer.isValid() ? m_deltaTimer.nsecsElapsed() : 0;
        m_deltaTimer.start();
        if ((m_flags & UMApplicationMonitor::Logging) &&
            (m_filters & UMApplicationMonitor::FrameEvent)) {
            m_frameEvent.frame.swapTime = m_sceneGraphTimer.nsecsElapsed();
            m_frameEvent.timeStamp = UMEventUtils::timeStamp();
            m_loggingThread->push(&m_frameEvent);
        }
    } else {
        initialiseGpuResources();  // Get everything ready for the next frame.
        if (m_flags & UMApplicationMonitor::Overlay) {
            m_window->update();
        }
    }

    if (m_flags & UMApplicationMonitor::ContinuousUpdates) {
        m_window->update();
    }
}

void WindowMonitor::windowSceneGraphAboutToStop()
{
#if !defined(QT_NO_DEBUG)
    ASSERT(UMApplicationMonitor::removeMonitor(this));
#else
    UMApplicationMonitor::removeMonitor(this);
#endif

    if (m_flags & GpuResourcesInitialised) {
        finaliseGpuResources();
    }
    delete this;
}

void WindowMonitor::setProcessEvent(const UMEvent& event)
{
    DASSERT(event.type == UMEvent::Process);

    if (m_flags & UMApplicationMonitor::Overlay) {
        m_mutex.lock();
        m_overlay.setProcessEvent(event);
        m_mutex.unlock();
        m_window->update();
    }
}
