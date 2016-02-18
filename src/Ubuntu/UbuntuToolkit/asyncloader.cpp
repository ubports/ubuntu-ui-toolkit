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
    if (!m_context->parent()) {
        m_context->setParent(object);
    }
    emitStatus(ComponentCreated, object);
}

void AsyncLoader::statusChanged(Status status)
{
    emitStatus((LoadingStatus)status);
    if (status == QQmlIncubator::Error) {
        QList<QQmlError> e = errors();
        for (int i = 0; i < e.size(); i++) {
            qWarning() << e[0].toString();
        }
    }
    if (status != QQmlIncubator::Loading) {
        detachComponent();
    }
}

// procected methods
void AsyncLoader::emitStatus(LoadingStatus status, QObject *object)
{
    if (!object) {
        object = this->object();
    }
    m_status = status;
    Q_EMIT loadingStatus(m_status, object);
}

void AsyncLoader::detachComponent()
{
    if (!m_component) {
        return;
    }

    if (m_ownComponent) {
        delete m_component;
    } else {
        disconnect(m_component, &QQmlComponent::statusChanged,
                   this, &AsyncLoader::onComponentStatusChanged);
    }
    m_component = nullptr;
    m_ownComponent = false;
}

void AsyncLoader::onComponentStatusChanged(QQmlComponent::Status status)
{
    if (status == QQmlComponent::Error) {
        qWarning() << m_component->errorString();
        detachComponent();
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
 * The method initiates the loading of a given \e url within a specific \e context.
 */
void AsyncLoader::load(const QUrl &url, QQmlContext *context)
{
    reset();
    if (url.isEmpty() || !url.isValid() || !context) {
        emitStatus(Ready);
        return;
    }
    m_ownComponent = true;
    load(new QQmlComponent(context->engine(), url, QQmlComponent::Asynchronous), context);
}

/*!
 * \brief AsyncLoader::load
 * \param component
 * \param context
 * The method initiates the loading of a \e component within the given \e context.
 */
void AsyncLoader::load(QQmlComponent *component, QQmlContext *context)
{
    reset();
    if (!component || !context) {
        emitStatus(Ready);
        return;
    }
    m_component = component;
    m_context = context;
    if (m_component->isLoading()) {
        connect(m_component, &QQmlComponent::statusChanged,
                this, &AsyncLoader::onComponentStatusChanged);
    } else {
        onComponentStatusChanged(m_component->status());
    }
}

/*!
 * \brief AsyncLoader::reset
 * Clears the incubator and emits loadingStatus() signal with \c Reset status.
 */
void AsyncLoader::reset()
{
    clear();
    // make sure the listeners are getting the reset so they can delete the object
    emitStatus(Reset);
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
