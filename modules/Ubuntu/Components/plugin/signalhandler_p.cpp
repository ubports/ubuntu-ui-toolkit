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

#include "signalhandler_p.h"
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <QtCore/QSocketNotifier>

SignalHandler::SignalHandler(QObject *parent) :
    QObject(parent)
{
}

void SignalHandler::connectSignal(SignalType type)
{
    if (notifiers.contains(type)) {
        return;
    }

    HandlerType handler;
    if (type == Interrupt) {
        handler = createHandler(SIGINT, signalHook);
    } else if (type == Terminate) {
        handler = createHandler(SIGTERM, signalHook);
    } else {
        qFatal("Invalid signal type");
    }

    notifiers.insert(type, handler);
    SocketRecord record(type, handler.second);
    socketRegister.insert(handler.first[1], record);
}

SignalHandler::HandlerType SignalHandler::createHandler(int signal, SignalHook hook)
{
    struct sigaction sigAction;
    sigAction.sa_handler = hook;
    ::sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = SA_RESTART;

    if (::sigaction(signal, &sigAction, 0) > 0) {
        qFatal("UNIX signal registration error");
    }
    HandlerType handler;
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, handler.first)) {
        qFatal("Cannot create signal socketpair");
    }
    handler.second = new QSocketNotifier(handler.first[1], QSocketNotifier::Read, this);
    QObject::connect(handler.second, SIGNAL(activated(int)), this, SLOT(notifierActivated(int)));

    return handler;
}

void SignalHandler::signalHook(int signal)
{
    HandlerType handler;
    switch (signal) {
    case SIGINT:
        handler = SignalHandler::instance().notifiers.value(Interrupt);
        break;
    case SIGTERM:
        handler = SignalHandler::instance().notifiers.value(Terminate);
        break;
    }
    if (handler.second) {
        char value = 1;
        ::write(handler.first[0], &value, sizeof(value));
    }
}

void SignalHandler::notifierActivated(int socket)
{
    // get the socket from the hash
    SocketRecord record = socketRegister.value(socket);
    QSocketNotifier *notifier = record.second;
    HandlerType handler = notifiers.value(record.first);

    notifier->setEnabled(false);
    char value;
    ::read(handler.first[1], &value, sizeof(value));

    Q_EMIT signalTriggered(record.first);

    notifier->setEnabled(true);
}
