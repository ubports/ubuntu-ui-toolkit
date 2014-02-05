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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "upmrenderingtimes.h"
#include "rendertimertrivial.h"
#if defined(QT_OPENGL_ES)
#include "rendertimerkhrfence.h"
#include "rendertimernvfence.h"
#else
#include "rendertimerarbquery.h"
#include "rendertimerextquery.h"
#endif
#include <QtCore/qmath.h>

UPMRenderingTimes::UPMRenderingTimes(QQuickItem* parent) :
    QQuickItem(parent),
    m_period(1000),
    m_graphModel(new UPMGraphModel(this)),
    m_timerType(UPMRenderingTimes::Automatic),
    m_needsNewTimer(true),
    m_window(NULL),
    m_renderingTimer(NULL),
    m_oddFrame(false),
    m_oddFrameRenderTime(0)
{
    /* Forward samplesChanged signal from graphModel */
    QObject::connect(m_graphModel, &UPMGraphModel::samplesChanged,
                     this, &UPMRenderingTimes::samplesChanged);

    /* Periodically append render time of the most costly frame rendered.
       The period is period / samples */
    QObject::connect(this, &UPMRenderingTimes::frameRendered,
                     this, &UPMRenderingTimes::onFrameRendered);
}

UPMRenderingTimes::~UPMRenderingTimes()
{
    if (m_renderingTimer != NULL) {
        delete m_renderingTimer;
    }
}

int UPMRenderingTimes::period() const
{
    return m_period;
}

void UPMRenderingTimes::setPeriod(int period)
{
    if (period != m_period) {
        m_period = period;
        Q_EMIT periodChanged();
    }
}

int UPMRenderingTimes::samples() const
{
    return m_graphModel->samples();
}

void UPMRenderingTimes::setSamples(int samples)
{
    m_graphModel->setSamples(samples);
}

UPMGraphModel* UPMRenderingTimes::graphModel() const
{
    return m_graphModel;
}

UPMRenderingTimes::TimerType UPMRenderingTimes::timerType() const
{
    return m_timerType;
}

void UPMRenderingTimes::setTimerType(UPMRenderingTimes::TimerType timerType)
{
    if (timerType != m_timerType) {
        m_timerType = timerType;
        // setup/teardown of RenderTimers need to happen in the rendering thread
        m_needsNewTimer = true;
        Q_EMIT timerTypeChanged();
    }
}

// FIXME: can be replaced with connecting to windowChanged() signal introduced in Qt5.2
void UPMRenderingTimes::itemChange(ItemChange change, const ItemChangeData & value)
{
    if (change == QQuickItem::ItemSceneChange) {
        connectToWindow(value.window);
    }
    QQuickItem::itemChange(change, value);
}

void UPMRenderingTimes::connectToWindow(QQuickWindow* window)
{
    if (window != m_window) {
        if (m_window != NULL) {
            QObject::disconnect(m_window, &QQuickWindow::sceneGraphInitialized,
                                this, &UPMRenderingTimes::onSceneGraphInitialized);
            QObject::disconnect(m_window, &QQuickWindow::sceneGraphInvalidated,
                                this, &UPMRenderingTimes::onSceneGraphInvalidated);
            QObject::disconnect(m_window, &QQuickWindow::beforeRendering,
                                this, &UPMRenderingTimes::onBeforeRendering);
            QObject::disconnect(m_window, &QQuickWindow::afterRendering,
                                this, &UPMRenderingTimes::onAfterRendering);
            QObject::disconnect(m_window, &QQuickWindow::frameSwapped,
                                this, &UPMRenderingTimes::onFrameSwapped);
        }

        m_window = window;

        if (m_window != NULL) {
            QObject::connect(m_window, &QQuickWindow::sceneGraphInitialized,
                             this, &UPMRenderingTimes::onSceneGraphInitialized,
                             Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::sceneGraphInvalidated,
                             this, &UPMRenderingTimes::onSceneGraphInvalidated,
                             Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::beforeRendering,
                                this, &UPMRenderingTimes::onBeforeRendering, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::afterRendering,
                                this, &UPMRenderingTimes::onAfterRendering, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::frameSwapped,
                                this, &UPMRenderingTimes::onFrameSwapped, Qt::DirectConnection);
        }
    }
}

void UPMRenderingTimes::onSceneGraphInitialized()
{
    if (m_renderingTimer) {
        m_renderingTimer->setup();
    }
}

void UPMRenderingTimes::onSceneGraphInvalidated()
{
    if (m_renderingTimer) {
        m_renderingTimer->teardown();
    }
}

void UPMRenderingTimes::onBeforeRendering()
{
    if (m_needsNewTimer) {
        setupNewTimer();
        m_needsNewTimer = false;
    }
    m_renderingTimer->start();
}

void UPMRenderingTimes::onAfterRendering()
{
    Q_EMIT frameRendered(m_renderingTimer->stop());
}

void UPMRenderingTimes::onFrameSwapped()
{
}

void UPMRenderingTimes::onFrameRendered(qint64 renderTime)
{
    if (!m_oddFrame) {
        appendRenderTime(m_oddFrameRenderTime);
        appendRenderTime(renderTime);
    } else {
        m_oddFrameRenderTime = renderTime;
    }
    m_oddFrame = !m_oddFrame;
}

void UPMRenderingTimes::setupNewTimer()
{
    if (m_renderingTimer != NULL) {
        m_renderingTimer->teardown();
        delete m_renderingTimer;
    }
    switch (m_timerType) {
    case UPMRenderingTimes::Trivial:
        m_renderingTimer = new RenderTimerTrivial;
        break;
#if defined(QT_OPENGL_ES)
    case UPMRenderingTimes::KHRFence:
        m_renderingTimer = new RenderTimerKHRFence;
        break;
    case UPMRenderingTimes::NVFence:
        m_renderingTimer = new RenderTimerNVFence;
        break;
#else
    case UPMRenderingTimes::ARBTimerQuery:
        m_renderingTimer = new RenderTimerARBQuery;
        break;
    case UPMRenderingTimes::EXTTimerQuery:
        m_renderingTimer = new RenderTimerEXTQuery;
        break;
#endif
    case UPMRenderingTimes::Automatic:
#if defined(QT_OPENGL_ES)
        if (RenderTimerKHRFence::isAvailable()) {
            m_renderingTimer = new RenderTimerKHRFence;
        } else if (RenderTimerNVFence::isAvailable()) {
            m_renderingTimer = new RenderTimerNVFence;
        } else {
            m_renderingTimer = new RenderTimerTrivial;
        }
#else
        if (RenderTimerARBQuery::isAvailable()) {
            m_renderingTimer = new RenderTimerARBQuery;
        } else if (RenderTimerEXTQuery::isAvailable()) {
            m_renderingTimer = new RenderTimerEXTQuery;
        } else {
            m_renderingTimer = new RenderTimerTrivial;
        }
#endif
        break;
    }

    m_renderingTimer->setup();
}


void UPMRenderingTimes::appendRenderTime(qint64 renderTime)
{
    int renderTimeInMs = qCeil((qreal)renderTime / 1000000);
    int width = ((qreal)samples() / m_period) * renderTimeInMs;
    m_graphModel->appendValue(width, renderTimeInMs);
}
