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

#include <QDebug>

UnixSignalHandler::UnixSignalHandler(QObject *parent) :
    QObject(parent)
{
}

void UnixSignalHandler::connectSignal(SignalType type)
{
    qDebug() << "connectSignal" << type << 1;
    if (notifiers.contains(type)) {
        return;
    }

    qDebug() << "connectSignal" << 2;
    HandlerType handler = createHandler((int)type);
    qDebug() << "connectSignal" << 3;
    notifiers.insert(type, handler);
    qDebug() << "connectSignal" << 4;
    socketRegister.insert(handler.first[1], type);
    qDebug() << "connectSignal" << 5;
}

UnixSignalHandler::HandlerType UnixSignalHandler::createHandler(int signal)
{
    qDebug() << "createHandler" << signal << 1;
    struct sigaction sigAction;
    sigAction.sa_handler = signalHook;
    ::sigemptyset(&sigAction.sa_mask);
    qDebug() << "createHandler" << 2;
    sigAction.sa_flags = SA_RESTART;
    qDebug() << "createHandler" << 3;

    if (::sigaction(signal, &sigAction, 0) > 0) {
        qFatal("UNIX signal registration error");
    }
    qDebug() << "createHandler" << 4;
    HandlerType handler;
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, handler.first)) {
        qFatal("Cannot create signal socketpair");
    }
    qDebug() << "createHandler" << 5;
    handler.second = new QSocketNotifier(handler.first[1], QSocketNotifier::Read, this);
    qDebug() << "createHandler" << 6;
    QObject::connect(handler.second, SIGNAL(activated(int)), this, SLOT(notifierActivated(int)));
    qDebug() << "createHandler" << 7;

    return handler;
}

void UnixSignalHandler::signalHook(int signal)
{
    qDebug() << "signalHook" << signal << 1;
    HandlerType handler = UnixSignalHandler::instance().notifiers.value((SignalType)signal);
    qDebug() << "signalHook" << 2;
    char value = 1;
    ssize_t size = ::write(handler.first[0], &value, sizeof(value));
    qDebug() << "signalHook" << 3;
    // this is needed to suppress write return value warning
    Q_UNUSED(size)
}

void UnixSignalHandler::notifierActivated(int socket)
{
    // get the socket from the hash
    qDebug() << "notifierActivated" << 1;
    SignalType signal = socketRegister.value(socket);
    qDebug() << "notifierActivated" << 2;
    HandlerType handler = notifiers.value(signal);
    qDebug() << "notifierActivated" << 3;

    handler.second->setEnabled(false);
    qDebug() << "notifierActivated" << 4;
    char value;
    ssize_t size = ::read(handler.first[1], &value, sizeof(value));
    qDebug() << "notifierActivated" << 5;
    // this is needed to suppress read return value warning
    Q_UNUSED(size)

    Q_EMIT signalTriggered(signal);
    qDebug() << "notifierActivated" << 6;

    handler.second->setEnabled(true);
    qDebug() << "notifierActivated" << 7;
}
