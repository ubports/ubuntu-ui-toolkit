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

#include "asyncloader_p.h"

#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>

namespace UbuntuToolkit {

AsyncLoader::AsyncLoader(QObject *parent)
    : QObject(*(new AsyncLoaderPrivate), parent)
{    
}

AsyncLoader::~AsyncLoader()
{
    reset();
}

// incubator methods
void AsyncLoaderPrivate::setInitialState(QObject *object)
{
    emitStatus(AsyncLoader::Initializing, object);
}

AsyncLoader::LoadingStatus incubatorToLoadingStatus(QQmlIncubator::Status status)
{
    switch (status) {
    case QQmlIncubator::Null: return AsyncLoader::Null;
    case QQmlIncubator::Ready: return AsyncLoader::Ready;
    case QQmlIncubator::Loading: return AsyncLoader::Loading;
    case QQmlIncubator::Error: return AsyncLoader::Error;
    }
    // unlikely to be reached, but must satisfy compiler
    return AsyncLoader::Null;
}

void AsyncLoaderPrivate::statusChanged(Status status)
{
    if (status == QQmlIncubator::Error) {
        QList<QQmlError> e = errors();
        for (int i = 0; i < e.size(); i++) {
            // remove quotes and any leading/trailing whitespace
            qWarning().noquote() << e[0].toString().trimmed();
        }
    }
    if (status != QQmlIncubator::Loading) {
        detachComponent();
    }
    // we should emit the status change only after we do the cleanup
    emitStatus(incubatorToLoadingStatus(status));
}

// procected methods
void AsyncLoaderPrivate::emitStatus(AsyncLoader::LoadingStatus status, QObject *object)
{
    if (this->status == status) {
        return;
    }

    this->status = status;
    if (!object) {
        object = this->object();
    }
    Q_EMIT q_func()->loadingStatus(this->status, object);
}

void AsyncLoaderPrivate::detachComponent()
{
    if (!component) {
        return;
    }

    if (componentHandler) {
        QObject::disconnect(*componentHandler);
    }
    componentHandler.reset();
    if (ownComponent) {
        component->deleteLater();
    }
    component = nullptr;
    ownComponent = false;
}

void AsyncLoaderPrivate::onComponentStatusChanged(QQmlComponent::Status status)
{
    if (status == QQmlComponent::Error) {
        QString error = component->errorString();
        // remove quotes and any leading/trailing whitespace
        qWarning().noquote() << error.trimmed();
        detachComponent();
        emitStatus(AsyncLoader::Error);
        return;
    }
    if (status == QQmlComponent::Ready) {
        component->create(*this, context);
    }
}

/*!
 * \brief AsyncLoader::load
 * \param url
 * \param context
 * \return bool
 * The method initiates the loading of a given \e url within a specific \e context.
 * Returns true on success.
 * \note If the loading is initiated while there is a previous loading in place,
 * you must make sure you delete the object from the previous loading before you
 * trigger the new load.
 */
bool AsyncLoader::load(const QUrl &url, QQmlContext *context)
{
    if (!reset() || !context) {
        return false;
    }
    Q_D(AsyncLoader);
    if (url.isEmpty() || !url.isValid()) {
        d->emitStatus(Ready);
        return false;
    }
    d->ownComponent = true;
    return load(new QQmlComponent(context->engine(), url, QQmlComponent::Asynchronous), context);
}

/*!
 * \brief AsyncLoader::load
 * \param component
 * \param context
 * \return bool
 * The method initiates the loading of a \e component within the given \e context.
 * Returns true on success.
 * \note If the loading is initiated while there is a previous loading in place,
 * you must make sure you delete the object from the previous loading before you
 * trigger the new load.
 */
bool AsyncLoader::load(QQmlComponent *component, QQmlContext *context)
{
    if (!reset() || !context) {
        return false;
    }
    Q_D(AsyncLoader);
    if (!component) {
        d->emitStatus(Ready);
        return false;
    }
    d->component = component;
    d->context = context;
    if (d->component->isLoading()) {
        d->emitStatus(Compiling);
        auto callback = [d] (QQmlComponent::Status status) {
            d->onComponentStatusChanged(status);
        };

        d->componentHandler.reset(new QMetaObject::Connection);
        *(d->componentHandler) = QObject::connect(d->component, &QQmlComponent::statusChanged, callback);
    } else {
        d->onComponentStatusChanged(d->component->status());
    }
    return true;
}

/*!
 * \brief AsyncLoader::reset
 * \return bool
 * Clears the incubator and emits loadingStatus() signal with \c Reset status.
 * The loader can be reset only if the status passed \c Loading. Returns true
 * if the reset was successful, or when the loader status is \c Ready or
 * \c Error.
 */
bool AsyncLoader::reset()
{
    Q_D(AsyncLoader);
    if (d->status < Loading) {
        return false;
    }
    if (d->status >= Ready) {
        return true;
    }
    d->clear();
    // make sure the listeners are getting the reset so they can delete the object
    d->emitStatus(Reset);
    return true;
}

/*!
 * \brief AsyncLoader::status
 * \return LoadingStatus
 * Returns the status of the loader.
 */
AsyncLoader::LoadingStatus AsyncLoader::status()
{
    return d_func()->status;
}

/*!
 * \brief AsyncLoader::forceCompletion
 * Forces loading completion.
 */
void AsyncLoader::forceCompletion()
{
    d_func()->forceCompletion();
}

} // namespace UbuntuToolkit
