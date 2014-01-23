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
#include <QtCore/qmath.h>

UPMRenderingTimes::UPMRenderingTimes(QQuickItem* parent) :
    QQuickItem(parent),
    m_period(1000),
    m_graphModel(new UPMGraphModel(this)),
    m_window(NULL)
{
    /* Disable synchronization to vertical blank signal on Intel */
    qputenv("vblank_mode", "0");

    /* Forward samplesChanged signal from graphModel */
    QObject::connect(m_graphModel, &UPMGraphModel::samplesChanged,
                     this, &UPMRenderingTimes::samplesChanged);

    /* Periodically append render time of the most costly frame rendered.
       The period is period / samples */
    QObject::connect(this, &UPMRenderingTimes::frameRendered,
                     this, &UPMRenderingTimes::onFrameRendered);
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
            QObject::disconnect(m_window, &QQuickWindow::beforeRendering,
                                this, &UPMRenderingTimes::onBeforeRendering);
            QObject::disconnect(m_window, &QQuickWindow::afterRendering,
                                this, &UPMRenderingTimes::onAfterRendering);
            QObject::disconnect(m_window, &QQuickWindow::frameSwapped,
                                this, &UPMRenderingTimes::onFrameSwapped);
        }

        m_window = window;

        if (m_window != NULL) {
            QObject::connect(m_window, &QQuickWindow::beforeRendering,
                                this, &UPMRenderingTimes::onBeforeRendering, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::afterRendering,
                                this, &UPMRenderingTimes::onAfterRendering, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::frameSwapped,
                                this, &UPMRenderingTimes::onFrameSwapped, Qt::DirectConnection);
        }
    }
}

void UPMRenderingTimes::onBeforeRendering()
{
    m_renderingTimer.start();
}

void UPMRenderingTimes::onAfterRendering()
{
    Q_EMIT frameRendered(m_renderingTimer.nsecsElapsed());
}

void UPMRenderingTimes::onFrameSwapped()
{
}

void UPMRenderingTimes::onFrameRendered(qint64 renderTime)
{
    static bool oddFrame = false;
    static qint64 oddFrameRenderTime = 0;

    if (!oddFrame) {
        appendRenderTime(oddFrameRenderTime);
        appendRenderTime(renderTime);
    } else {
        oddFrameRenderTime = renderTime;
    }
    oddFrame = !oddFrame;
}

void UPMRenderingTimes::appendRenderTime(qint64 renderTime)
{
    int renderTimeInMs = qCeil((qreal)renderTime / 1000000);
    int width = ((qreal)samples() / m_period) * renderTimeInMs;
    m_graphModel->appendValue(width, renderTimeInMs);
}
