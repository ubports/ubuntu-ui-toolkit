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
 */

#ifndef ACTIONSPROXY_P_H
#define ACTIONSPROXY_P_H

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QPointer>
#include "ucaction.h"

class UCActionContext;
class ActionProxy : public QObject
{
    Q_OBJECT
public:

    ~ActionProxy();
    static ActionProxy &instance()
    {
        static ActionProxy instance;
        return instance;
    }

    UCActionContext *globalContext;

    static UCActionContext *currentContext();
    static const QSet<UCActionContext*> &localContexts();
    static void publishGlobalContext();
    static void addContext(UCActionContext *context);
    static void removeContext(UCActionContext *context);

protected:
    ActionProxy();

protected Q_SLOTS:
    void watchContextActivation(UCActionContext *context, bool watch);
    void handleContextActivation();
    virtual void clearContextActions(UCActionContext *context);
    virtual void publishContextActions(UCActionContext *context);

private:
    QSet<UCActionContext*> m_localContexts;
    QPointer<UCActionContext> m_activeContext;
};

#endif // ACTIONSPROXY_P_H
