// Copyright © 2016 Canonical Ltd.
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

#include <QtQml/QtQml>
#include <UbuntuMetrics/applicationmonitor.h>

// FIXME(loicm)
//   - Not sure how to add support for the loggers API?
//   - Add support for the generic logging API.

class ApplicationMonitorWrapper : public QObject
{
    Q_OBJECT

    Q_FLAGS(LoggingFilters);
    Q_ENUMS(Event);
    Q_PROPERTY(bool overlay READ overlay WRITE setOverlay NOTIFY overlayChanged)
    Q_PROPERTY(bool logging READ logging WRITE setLogging NOTIFY loggingChanged)
    Q_PROPERTY(LoggingFilters loggingFilter READ loggingFilter WRITE setLoggingFilter
               NOTIFY loggingFilterChanged)
    Q_PROPERTY(int processUpdateInterval READ processUpdateInterval
               WRITE setProcessUpdateInterval NOTIFY processUpdateIntervalChanged)

public:
    ApplicationMonitorWrapper(QObject* parent = 0)
        : QObject(parent)
        , m_applicationMonitor(UMApplicationMonitor::instance())
    {
        QObject::connect(m_applicationMonitor, SIGNAL(overlayChanged()),
                         this, SIGNAL(overlayChanged()));
        QObject::connect(m_applicationMonitor, SIGNAL(loggingChanged()),
                         this, SIGNAL(loggingChanged()));
        QObject::connect(m_applicationMonitor, SIGNAL(loggingFilterChanged()),
                         this, SIGNAL(loggingFilterChanged()));
        QObject::connect(m_applicationMonitor, SIGNAL(updateIntervalChanged(UMEvent::Type)),
                         this, SLOT(updateIntervalChanged(UMEvent::Type)));
    }
    ~ApplicationMonitorWrapper() {}

    enum LoggingFilter {
        ProcessEvent = UMApplicationMonitor::ProcessEvent,
        WindowEvent  = UMApplicationMonitor::WindowEvent,
        FrameEvent   = UMApplicationMonitor::FrameEvent,
        GenericEvent = UMApplicationMonitor::GenericEvent,
        AllEvents    = UMApplicationMonitor::AllEvents
    };
    Q_DECLARE_FLAGS(LoggingFilters, LoggingFilter)

    enum Event {
        UserInterfaceReady = UMApplicationMonitor::UserInterfaceReady
    };

    bool overlay() const { return m_applicationMonitor->overlay(); }
    void setOverlay(bool overlay) { m_applicationMonitor->setOverlay(overlay); }
    bool logging() const { return m_applicationMonitor->logging(); }
    void setLogging(bool logging) { m_applicationMonitor->setLogging(logging); }
    LoggingFilters loggingFilter() const {
        return QFlags<LoggingFilters>::enum_type(
            QFlags<UMApplicationMonitor::LoggingFilters>::Int(
                m_applicationMonitor->loggingFilter())); }
    void setLoggingFilter(LoggingFilters filter) {
        m_applicationMonitor->setLoggingFilter(
            QFlags<UMApplicationMonitor::LoggingFilters>::enum_type(
                QFlags<LoggingFilters>::Int(filter))); }
    int processUpdateInterval() const {
        return m_applicationMonitor->updateInterval(UMEvent::Process); }
    void setProcessUpdateInterval(int interval) {
        m_applicationMonitor->setUpdateInterval(UMEvent::Process, interval); }

    Q_INVOKABLE bool logEvent(Event event) {
        return m_applicationMonitor->logEvent(static_cast<UMApplicationMonitor::Event>(event)); }

Q_SIGNALS:
    void overlayChanged();
    void loggingChanged();
    void loggingFilterChanged();
    void processUpdateIntervalChanged();

private Q_SLOTS:
    void updateIntervalChanged(UMEvent::Type type)
    {
        if (type == UMEvent::Process) {
            Q_EMIT processUpdateIntervalChanged();
        }
    }

private:
    UMApplicationMonitor* m_applicationMonitor;
};

static QObject* applicationMonitorSingletonProvider(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new ApplicationMonitorWrapper();
}

class UbuntuMetricsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")

public:
    ~UbuntuMetricsPlugin() {}

    virtual void registerTypes(const char* uri) override
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Ubuntu.Metrics"));
        qmlRegisterSingletonType<ApplicationMonitorWrapper>(
            uri, 1, 0, "ApplicationMonitor", applicationMonitorSingletonProvider);
    }
};

#include "plugin.moc"
