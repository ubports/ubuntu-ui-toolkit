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
#ifndef SIGNALHANDLER_P_H
#define SIGNALHANDLER_P_H

#include <QtCore/QObject>
#include <QtCore/QHash>

class QSocketNotifier;
typedef void (*SignalHook)(int);

class SignalHandler : public QObject
{
    Q_OBJECT
public:
    enum SignalType {
        Interrupt,
        Terminate
    };

    typedef QPair<int[2], QSocketNotifier*> HandlerType;
    typedef QPair<SignalType, QSocketNotifier*> SocketRecord;

    static SignalHandler &instance()
    {
        static SignalHandler instance;
        return instance;
    }

    void connectSignal(SignalType type);

Q_SIGNALS:
    void signalTriggered(int type);

private Q_SLOTS:
    void notifierActivated(int socket);
private:
    explicit SignalHandler(QObject *parent = 0);

    static void signalHook(int);
    HandlerType createHandler(int signal, SignalHook hook);
    QHash<SignalType, HandlerType > notifiers;
    QHash<int, SocketRecord> socketRegister;
};

#endif // SIGNALHANDLER_P_H
