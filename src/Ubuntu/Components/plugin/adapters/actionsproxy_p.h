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
class UCPopupContext;
class ActionProxy
{
public:

    ~ActionProxy();
    static ActionProxy &instance()
    {
        static ActionProxy instance;
        return instance;
    }

    UCActionContext *globalContext;

    static const QSet<UCActionContext*> &localContexts();
    static void publishGlobalContext();
    static void addContext(UCActionContext *context);
    static void removeContext(UCActionContext *context);
    static void activateContext(UCActionContext *context);

protected:
    ActionProxy();

protected:
    virtual void clearContextActions(UCActionContext *context);
    virtual void publishContextActions(UCActionContext *context);

private:
    QSet<UCActionContext*> m_localContexts;
    QStack<UCPopupContext*> m_popupContexts;

    void addPopupContext(UCPopupContext *context);
    void removePopupContext(UCPopupContext *context);
};

#endif // ACTIONSPROXY_P_H
