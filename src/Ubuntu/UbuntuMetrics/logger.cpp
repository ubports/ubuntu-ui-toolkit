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

#include "logger_p.h"
#include "ubuntumetricsglobal_p.h"
#include "events.h"
#include <QtCore/QDir>
#include <QtCore/QTime>

// LTTng shared object runtime loading.
#include <dlfcn.h>
#define TRACEPOINT_DEFINE
#define TRACEPOINT_PROBE_DYNAMIC_LINKAGE
#include "lttng/lttng_p.h"

UMFileLogger::UMFileLogger(const QString& fileName, bool parsable)
    : d_ptr(new UMFileLoggerPrivate(fileName, parsable))
{
}

UMFileLoggerPrivate::UMFileLoggerPrivate(const QString& fileName, bool parsable)
{
    if (QDir::isRelativePath(fileName)) {
        m_file.setFileName(QString(QDir::currentPath() + QDir::separator() + fileName));
    } else {
        m_file.setFileName(fileName);
    }

    if (m_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Unbuffered)) {
        m_textStream.setDevice(&m_file);
        m_textStream.setCodec("ISO 8859-1");
        m_textStream.setRealNumberPrecision(2);
        m_textStream.setRealNumberNotation(QTextStream::FixedNotation);
        m_flags = Open | Parsable;
        if (parsable) {
            m_flags |= Parsable;
        }
    } else {
        m_flags = 0;
        WARN("FileLogger: Can't open file %s '%s'.", fileName.toLatin1().constData(),
             m_file.errorString().toLatin1().constData());
    }
}

UMFileLogger::UMFileLogger(FILE* fileHandle, bool parsable)
    : d_ptr(new UMFileLoggerPrivate(fileHandle, parsable))
{
}

UMFileLoggerPrivate::UMFileLoggerPrivate(FILE* fileHandle, bool parsable)
{
    if (m_file.open(fileHandle, QIODevice::WriteOnly | QIODevice::Text | QIODevice::Unbuffered)) {
        m_textStream.setDevice(&m_file);
        m_textStream.setCodec("ISO 8859-1");
        m_textStream.setRealNumberPrecision(2);
        m_textStream.setRealNumberNotation(QTextStream::FixedNotation);
        if ((fileHandle == stdout || fileHandle == stderr) &&
            !qEnvironmentVariableIsSet("UM_NO_LOGGER_COLOR")) {
            m_flags = Open | Colored;
        } else {
            m_flags = Open;
        }
        if (parsable) {
            m_flags |= Parsable;
        }
    } else {
        m_flags = 0;
        WARN("FileLogger: Can't open file handle '%s'.",
             m_file.errorString().toLatin1().constData());
    }
}

UMFileLogger::~UMFileLogger()
{
    delete d_ptr;
}

bool UMFileLogger::isOpen()
{
    return !!(d_func()->m_flags & UMFileLoggerPrivate::Open);
}

// FIXME(loicm) We should maybe get rid of QTextStream and directly write to the
//     device for efficiency reasons.

void UMFileLogger::log(const UMEvent& event)
{
    d_func()->log(event);
}

void UMFileLoggerPrivate::log(const UMEvent& event)
{
    if (m_flags & Open) {
        // ANSI/VT100 terminal codes.
        const char* const dim = m_flags & Colored ? "\033[02m" : "";
        const char* const reset = m_flags & Colored ? "\033[00m" : "";
        const char* const dimColon = m_flags & Colored ? "\033[02m:\033[00m" : "=";

        QTime timeStamp = QTime(0, 0).addMSecs(event.timeStamp / 1000000);
        QString timeString = !timeStamp.hour()
            ? timeStamp.toString(QString::fromLatin1("mm:ss:zzz", 9))
            : timeStamp.toString(QString::fromLatin1("hh:mm:ss:zzz", 12));

        switch (event.type) {
        case UMEvent::Process: {
            if (m_flags & Parsable) {
                m_textStream
                    << "P "
                    << event.timeStamp << ' '
                    << event.process.cpuUsage << ' '
                    << event.process.vszMemory << ' '
                    << event.process.rssMemory << ' '
                    << event.process.threadCount << '\n' << flush;
            } else {
                m_textStream
                    << (m_flags & Colored ? "\033[33mP\033[00m " : "P ")
                    << dim << timeString << reset << ' '
                    << "CPU" << dimColon << event.process.cpuUsage << "% "
                    << "VSZ" << dimColon << event.process.vszMemory << "kB "
                    << "RSS" << dimColon << event.process.rssMemory << "kB "
                    << "Threads" << dimColon << event.process.threadCount
                    << '\n' << flush;
            }
            break;
        }

        case UMEvent::Frame:
            if (m_flags & Parsable) {
                m_textStream
                    << "F "
                    << event.timeStamp << ' '
                    << event.frame.window << ' '
                    << event.frame.number << ' '
                    << event.frame.deltaTime << ' '
                    << event.frame.syncTime << ' '
                    << event.frame.renderTime << ' '
                    << event.frame.gpuTime << ' '
                    << event.frame.swapTime << '\n' << flush;
            } else {
                m_textStream
                    << (m_flags & Colored ? "\033[36mF\033[00m " : "F ")
                    << dim << timeString << reset << ' '
                    << "Win" << dimColon << event.frame.window << ' '
                    << "N" << dimColon << event.frame.number << ' '
                    << "Delta" << dimColon << event.frame.deltaTime / 1000000.0f << "ms "
                    << "Sync" << dimColon << event.frame.syncTime / 1000000.0f << "ms "
                    << "Render" << dimColon << event.frame.renderTime / 1000000.0f << "ms "
                    << "GPU" << dimColon << event.frame.gpuTime / 1000000.0f << "ms "
                    << "Swap" << dimColon << event.frame.swapTime / 1000000.0f << "ms\n" << flush;
            }
            break;

        case UMEvent::Window: {
            if (m_flags & Parsable) {
                m_textStream
                    << "W "
                    << event.timeStamp << ' '
                    << event.window.id << ' '
                    << event.window.state << ' '
                    << event.window.width << ' '
                    << event.window.height << '\n' << flush;
            } else {
                const char* const stateString[] = { "Hidden", "Shown", "Resized" };
                Q_STATIC_ASSERT(ARRAY_SIZE(stateString) == UMWindowEvent::StateCount);
                m_textStream
                    << (m_flags & Colored ? "\033[35mW\033[00m " : "W ")
                    << dim << timeString << reset << ' '
                    << "Id" << dimColon << event.window.id << ' '
                    << "State" << dimColon << stateString[event.window.state] << ' '
                    << "Size" << dimColon << event.window.width << 'x' << event.window.height
                    << '\n' << flush;
            }
            break;
        }

        case UMEvent::Generic: {
            if (m_flags & Parsable) {
                m_textStream
                    << "G "
                    << event.timeStamp << ' '
                    << event.generic.id << ' '
                    << event.generic.string << '\n' << flush;
            } else {
                m_textStream
                    << (m_flags & Colored ? "\033[32mG\033[00m " : "G ")
                    << dim << timeString << reset << ' '
                    << "Id" << dimColon << event.generic.id << ' '
                    << "String" << dimColon << '"' << event.generic.string << '"'
                    << '\n' << flush;
            }
            break;
        }

        default:
            DNOT_REACHED();
            break;
        }
    }
}

void UMFileLogger::setParsable(bool parsable)
{
    Q_D(UMFileLogger);

    if (parsable) {
        d->m_flags |= UMFileLoggerPrivate::Parsable;
    } else {
        d->m_flags &= ~UMFileLoggerPrivate::Parsable;
    }
}

bool UMFileLogger::parsable()
{
    return !!(d_func()->m_flags & UMFileLoggerPrivate::Parsable);
}

UMLTTNGPlugin* UMLTTNGLogger::m_plugin = nullptr;
bool UMLTTNGLogger::m_error = false;

UMLTTNGLogger::UMLTTNGLogger()
{
    // The LTTng tracepoints are dlopen'd so that we don't directly link to
    // liblttng-ust which spawns two threads when the lib is loaded. That allows
    // to avoid the cost of it at startup when the user doesn't use LTTng (as
    // well as not showing them in the metrics).

    if (!m_error && !m_plugin) {
        // Ensure the plugin is first loaded from the build path to ease
        // development on the toolkit from uninstalled sources.
        // FIXME(loicm) Security concerns?
        void* handle = dlopen(LTTNG_PLUGIN_BUILD_PATH, RTLD_LAZY);
        if (!handle) {
            handle = dlopen(LTTNG_PLUGIN_INSTALL_PATH, RTLD_LAZY);
            if (!handle) {
                WARN("ApplicationMonitor: %s", dlerror());
                m_error = true;
                return;
            }
        }
        m_plugin = static_cast<UMLTTNGPlugin*>(dlsym(handle, "umLttngPlugin"));
        if (!m_plugin) {
            WARN("ApplicationMonitor: %s", dlerror());
            m_error = true;
        }
    }
}

void UMLTTNGLogger::log(const UMEvent& event)
{
    if (Q_LIKELY(m_plugin)) {
        switch (event.type) {

        case UMEvent::Process: {
            UMLTTNGProcessEvent processEvent = {
                .vszMemory = event.process.vszMemory,
                .rssMemory = event.process.rssMemory,
                .cpuUsage = event.process.cpuUsage,
                .threadCount = event.process.threadCount
            };
            m_plugin->logProcessEvent(&processEvent);
            break;
        }

        case UMEvent::Frame: {
            UMLTTNGFrameEvent frameEvent = {
                .window = event.frame.window,
                .number = event.frame.number,
                .deltaTime = event.frame.deltaTime * 0.000001f,
                .syncTime = event.frame.syncTime * 0.000001f,
                .renderTime = event.frame.renderTime * 0.000001f,
                .gpuTime = event.frame.gpuTime * 0.000001f,
                .swapTime = event.frame.swapTime * 0.000001f
            };
            m_plugin->logFrameEvent(&frameEvent);
            break;
        }

        case UMEvent::Window: {
            const char* stateString[] = { "Hidden", "Shown", "Resized" };
            Q_STATIC_ASSERT(ARRAY_SIZE(stateString) == UMWindowEvent::StateCount);
            UMLTTNGWindowEvent windowEvent = {
                .state = stateString[event.window.state],
                .id = event.window.id,
                .width = event.window.width,
                .height = event.window.height
            };
            m_plugin->logWindowEvent(&windowEvent);
            break;
        }

        case UMEvent::Generic: {
            UMLTTNGGenericEvent genericEvent;
            genericEvent.id = event.generic.id;
            DASSERT(event.generic.stringSize < UMGenericEvent::maxStringSize);
            memcpy(genericEvent.string, event.generic.string, event.generic.stringSize);
            m_plugin->logGenericEvent(&genericEvent);
            break;
        }

        default:
            DNOT_REACHED();
            break;
        }
    }
}
