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

#include "ucrenderingtimes.h"
#include <QtCore/qmath.h>

UCRenderingTimes::UCRenderingTimes(QQuickItem* parent) :
    QQuickItem(parent),
    m_enabled(true),
    m_period(1000),
    m_graphModel(new UCGraphModel(this)),
    m_window(NULL),
    m_highestTime(0)
{
    updateTimeBetweenSamples();

    /* Disable synchronization to vertical blank signal on Intel */
    qputenv("vblank_mode", "0");

    /* Forward samplesChanged signal from graphModel */
    QObject::connect(m_graphModel, &UCGraphModel::samplesChanged,
                     this, &UCRenderingTimes::samplesChanged);

    /* Periodically append render time of the most costly frame rendered.
       The period is period / samples */
    QObject::connect(this, &UCRenderingTimes::frameRendered,
                     this, &UCRenderingTimes::onFrameRendered);
}

/*!
 * \qmlproperty bool PerformanceMetrics::enabled
 *
 * Whether or not performance metric gathering is enabled.
 *
 */
bool UCRenderingTimes::enabled() const
{
    return m_enabled;
}

void UCRenderingTimes::setEnabled(bool enabled)
{
    if (enabled != m_enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged();
    }
}

int UCRenderingTimes::period() const
{
    return m_period;
}

void UCRenderingTimes::setPeriod(int period)
{
    if (period != m_period) {
        m_period = period;
        updateTimeBetweenSamples();
        Q_EMIT periodChanged();
    }
}

int UCRenderingTimes::samples() const
{
    return m_graphModel->samples();
}

void UCRenderingTimes::setSamples(int samples)
{
    m_graphModel->setSamples(samples);
    updateTimeBetweenSamples();
}

UCGraphModel* UCRenderingTimes::graphModel() const
{
    return m_graphModel;
}

// FIXME: can be replaced with connecting to windowChanged() signal introduced in Qt5.2
void UCRenderingTimes::itemChange(ItemChange change, const ItemChangeData & value)
{
    if (change == QQuickItem::ItemSceneChange) {
        connectToWindow(value.window);
    }
    QQuickItem::itemChange(change, value);
}

void UCRenderingTimes::connectToWindow(QQuickWindow* window)
{
    if (window != m_window) {
        if (m_window != NULL) {
            QObject::disconnect(m_window, &QQuickWindow::sceneGraphInitialized,
                                this, &UCRenderingTimes::onSceneGraphInitialized);
            QObject::disconnect(m_window, &QQuickWindow::beforeRendering,
                                this, &UCRenderingTimes::onBeforeRendering);
            QObject::disconnect(m_window, &QQuickWindow::afterRendering,
                                this, &UCRenderingTimes::onAfterRendering);
            QObject::disconnect(m_window, &QQuickWindow::frameSwapped,
                                this, &UCRenderingTimes::onFrameSwapped);
        }

        m_window = window;

        if (m_window != NULL) {
            QObject::connect(m_window, &QQuickWindow::sceneGraphInitialized,
                                this, &UCRenderingTimes::onSceneGraphInitialized, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::beforeRendering,
                                this, &UCRenderingTimes::onBeforeRendering, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::afterRendering,
                                this, &UCRenderingTimes::onAfterRendering, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::frameSwapped,
                                this, &UCRenderingTimes::onFrameSwapped, Qt::DirectConnection);
        }
    }
}

void UCRenderingTimes::onSceneGraphInitialized()
{
}

void UCRenderingTimes::onBeforeRendering()
{
    if (!m_appendTimer.isValid()) {
        m_appendTimer.start();
    }
    m_renderingTimer.start();
}

void UCRenderingTimes::onAfterRendering()
{
    Q_EMIT frameRendered(m_renderingTimer.nsecsElapsed());
}

void UCRenderingTimes::onFrameSwapped()
{
}

void UCRenderingTimes::onFrameRendered(qint64 renderTime)
{
    m_highestTime = qMax(renderTime, m_highestTime);

    /* Only append a render time if enough time (m_timeBetweenSamples) has passed.
       This ensures that updates to the data are regular.
    */
    if (renderTime >= m_timeBetweenSamples || m_appendTimer.nsecsElapsed() >= m_timeBetweenSamples) {
        // Append the highest render time recorded in the past period
        appendRenderTime(m_highestTime);
    }
}

void UCRenderingTimes::appendRenderTime(qint64 renderTime)
{
    const int maximumSyncTime = 16000000; // 16 ms
    int width;
    int renderTimeInMs = qCeil((qreal)renderTime / 1000000);

    if (m_appendTimer.nsecsElapsed() - renderTime > maximumSyncTime) {
        width = ((qreal)samples() / m_period) * renderTimeInMs;
    } else {
        width = ((qreal)samples() / m_period) * m_appendTimer.elapsed();
    }

    m_graphModel->appendValue(width, renderTimeInMs);

    // reset values
    m_highestTime = 0;
    m_appendTimer.start();
}

void UCRenderingTimes::updateTimeBetweenSamples()
{
    m_timeBetweenSamples = (qreal)m_period * 1000000 / samples();
}
