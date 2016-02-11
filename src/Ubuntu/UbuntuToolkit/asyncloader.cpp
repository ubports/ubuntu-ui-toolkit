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
    , m_object(nullptr)
    , m_ownComponent(false)
{    
}

AsyncLoader::~AsyncLoader()
{
    cancel();
}

// incubator methods
void AsyncLoader::setInitialState(QObject *object)
{
    m_object = object;
    Q_EMIT statusChanged(status(), m_object);
}

void AsyncLoader::statusChanged(Status status)
{
    Q_EMIT statusChanged(status, m_object);
    if (status == Error) {
        QList<QQmlError> e = errors();
        for (int i = 0; i < e.size(); i++) {
            qWarning() << e[0].toString();
        }
    }
    if (status != Loading) {
        detachComponent();
    }
}

// procected methods
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

// public API
void AsyncLoader::load(QQmlEngine *engine, const QUrl &url, QQmlContext *context)
{
    cancel();
    if (url.isEmpty() || !url.isValid()) {
        Q_EMIT statusChanged(Ready, nullptr);
        return;
    }
    m_ownComponent = true;
    load(new QQmlComponent(engine, url, QQmlComponent::Asynchronous), context);
}

void AsyncLoader::load(QQmlComponent *component, QQmlContext *context)
{
    cancel();
    if (!component) {
        Q_EMIT statusChanged(Ready, nullptr);
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

void AsyncLoader::cancel()
{
    if (status() == Loading) {
        forceCompletion();
    }
    reset();
}

void AsyncLoader::reset()
{
    delete m_object;
    m_object = nullptr;
}

} // namespace UbuntuToolkit
