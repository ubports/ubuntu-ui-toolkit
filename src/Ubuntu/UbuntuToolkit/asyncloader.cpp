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
 */

#include <asyncloader.h>

#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>

namespace UbuntuToolkit {

AsyncLoader::AsyncLoader(QObject *parent)
    : QObject(parent)
    , QQmlIncubator(Asynchronous)
    , m_component(nullptr)
    , m_context(nullptr)
    , m_ownComponent(false)
{    
}

AsyncLoader::~AsyncLoader()
{
    reset();
}

// incubator methods
void AsyncLoader::setInitialState(QObject *object)
{
    emitStatus(Initializing, object);
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

void AsyncLoader::statusChanged(Status status)
{
    if (status == QQmlIncubator::Error) {
        QList<QQmlError> e = errors();
        for (int i = 0; i < e.size(); i++) {
            qWarning() << e[0].toString();
        }
    }
    if (status != QQmlIncubator::Loading) {
        detachComponent();
    }
    // we should emit the status change only after we do the cleanup
    emitStatus(incubatorToLoadingStatus(status));
}

// procected methods
void AsyncLoader::emitStatus(LoadingStatus status, QObject *object)
{
    if (m_status == status) {
        return;
    }

    m_status = status;
    if (!object) {
        object = this->object();
    }
    Q_EMIT loadingStatus(m_status, object);
}

void AsyncLoader::detachComponent()
{
    if (!m_component) {
        return;
    }

    disconnect(m_component, &QQmlComponent::statusChanged,
               this, &AsyncLoader::onComponentStatusChanged);
    if (m_ownComponent) {
        m_component->deleteLater();
    }
    m_component = nullptr;
    m_ownComponent = false;
}

void AsyncLoader::onComponentStatusChanged(QQmlComponent::Status status)
{
    if (status == QQmlComponent::Error) {
        QString error = m_component->errorString();
        // remove any trailing LF
        qWarning().noquote() << error.trimmed();
        detachComponent();
        emitStatus(Error);
        return;
    }
    if (status == QQmlComponent::Ready) {
        m_component->create(*this, m_context);
    }
}

/*!
 * \brief AsyncLoader::load
 * \param url
 * \param context
 * \return bool
 * The method initiates the loading of a given \e url within a specific \e context.
 */
bool AsyncLoader::load(const QUrl &url, QQmlContext *context)
{
    if (!reset()) {
        return false;
    }
    if (url.isEmpty() || !url.isValid() || !context) {
        emitStatus(Ready);
        return true;
    }
    m_ownComponent = true;
    return load(new QQmlComponent(context->engine(), url, QQmlComponent::Asynchronous), context);
}

/*!
 * \brief AsyncLoader::load
 * \param component
 * \param context
 * \return bool
 * The method initiates the loading of a \e component within the given \e context.
 */
bool AsyncLoader::load(QQmlComponent *component, QQmlContext *context)
{
    if (!reset()) {
        return false;
    }
    if (!component || !context) {
        emitStatus(Ready);
        return true;
    }
    m_component = component;
    m_context = context;
    if (m_component->isLoading()) {
        emitStatus(Compiling);
        connect(m_component, &QQmlComponent::statusChanged,
                this, &AsyncLoader::onComponentStatusChanged, Qt::DirectConnection);
    } else {
        onComponentStatusChanged(m_component->status());
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
    if (m_status < Loading) {
        return false;
    }
    if (m_status >= Ready) {
        return true;
    }
    clear();
    // make sure the listeners are getting the reset so they can delete the object
    emitStatus(Reset);
    return true;
}

/*!
 * \brief AsyncLoader::status
 * \return LoadingStatus
 * Returns the status of the loader.
 */
AsyncLoader::LoadingStatus AsyncLoader::status()
{
    return m_status;
}

/*!
 * \brief AsyncLoader::forceCompletion
 * Forces loading completion.
 */
void AsyncLoader::forceCompletion()
{
    QQmlIncubator::forceCompletion();
}

} // namespace UbuntuToolkit
