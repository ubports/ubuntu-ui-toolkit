/*
 * Copyright 2014 Canonical Ltd.
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
 */

#include "unixsignalhandler_p.h"
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <QtCore/QSocketNotifier>

UT_NAMESPACE_BEGIN

UnixSignalHandler::UnixSignalHandler(QObject *parent) :
    QObject(parent)
{
}

void UnixSignalHandler::connectSignal(SignalType type)
{
    if (notifiers.contains(type)) {
        return;
    }

    HandlerType handler = createHandler((int)type);
    notifiers.insert(type, handler);
    socketRegister.insert(handler.first[1], type);
}

UnixSignalHandler::HandlerType UnixSignalHandler::createHandler(int signal)
{
    struct sigaction sigAction;
    sigAction.sa_handler = signalHook;
    ::sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = SA_RESTART;

    if (::sigaction(signal, &sigAction, 0) > 0) {
        qFatal("UNIX signal registration error");
    }
    HandlerType handler;
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, handler.first.data())) {
        qFatal("Cannot create signal socketpair");
    }
    handler.second = new QSocketNotifier(handler.first[1], QSocketNotifier::Read, this);
    QObject::connect(handler.second, SIGNAL(activated(int)), this, SLOT(notifierActivated(int)));

    return handler;
}

void UnixSignalHandler::signalHook(int signal)
{
    HandlerType handler = UnixSignalHandler::instance().notifiers.value((SignalType)signal);
    char value = 1;
    ssize_t size = ::write(handler.first[0], &value, sizeof(value));
    // this is needed to suppress write return value warning
    Q_UNUSED(size)
}

void UnixSignalHandler::notifierActivated(int socket)
{
    // get the socket from the hash
    SignalType signal = socketRegister.value(socket);
    HandlerType handler = notifiers.value(signal);

    handler.second->setEnabled(false);
    char value;
    ssize_t size = ::read(handler.first[1], &value, sizeof(value));
    // this is needed to suppress read return value warning
    Q_UNUSED(size)

    Q_EMIT signalTriggered(signal);

    handler.second->setEnabled(true);
}

UT_NAMESPACE_END
