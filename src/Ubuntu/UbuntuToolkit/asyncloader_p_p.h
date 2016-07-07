/*
 * Copyright 2016 Canonical Ltd.
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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef ASYNCLOADER_P_H
#define ASYNCLOADER_P_H

#include <QtCore/private/qobject_p.h>
#include <QtQml/QQmlIncubator>
#include <ubuntutoolkitglobal.h>
#include <UbuntuToolkit/private/asyncloader_p.h>
#include <UbuntuToolkit/private/asyncloader_p_p.h>

UT_NAMESPACE_BEGIN

class AsyncLoaderPrivate : public QObjectPrivate, public QQmlIncubator
{
    Q_DECLARE_PUBLIC(AsyncLoader)
public:
    AsyncLoaderPrivate()
        : QObjectPrivate()
        , QQmlIncubator(Asynchronous)
    {}

    QSharedPointer<QMetaObject::Connection> componentHandler;
    QQmlComponent *component = nullptr;
    QQmlContext *context = nullptr;
    AsyncLoader::LoadingStatus status = AsyncLoader::Ready;
    bool ownComponent = false;

    void setInitialState(QObject *object) override;
    void statusChanged(Status status) override;

    void emitStatus(AsyncLoader::LoadingStatus status, QObject *object = 0);
    void onComponentStatusChanged(QQmlComponent::Status status);
    void detachComponent();
};

UT_NAMESPACE_END

#endif // ASYNCLOADER_P_H
