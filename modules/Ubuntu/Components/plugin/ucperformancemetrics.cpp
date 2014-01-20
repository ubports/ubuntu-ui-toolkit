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

#include "ucperformancemetrics.h"
#include <QtCore/qmath.h>

UCGraphModel::UCGraphModel(QObject *parent) :
    QObject(parent),
    m_shift(0),
    m_samples(100)
{
    m_image = QImage(m_samples, 1, QImage::Format_RGB32);
    m_image.fill(0);
}

void UCGraphModel::appendValue(int width, int value)
{
    /* FIXME: modifying m_image here implicitly triggers a deep copy
       of its data because UCTextureFromImage usually holds a reference
       to that image
    */
    width = qMax(1, width);
    QRgb* line = (QRgb*)m_image.scanLine(0);

    if (width >= m_image.width()) {
        memset(&line[0], value, m_image.width() * 3);
    } else if (m_shift + width > m_image.width()) {
        int after = m_image.width() - m_shift;
        int before = width - after;
        memset(&line[m_shift], value, after * 3);
        memset(&line[0], value, before * 3);
    } else {
        memset(&line[m_shift], value, width * 3);
    }
    m_shift = (m_shift + width) % m_samples;

    Q_EMIT imageChanged();
    Q_EMIT shiftChanged();
}

QImage UCGraphModel::image() const
{
    return m_image;
}

int UCGraphModel::shift() const
{
    return m_shift;
}

int UCGraphModel::samples() const
{
    return m_samples;
}

void UCGraphModel::setSamples(int samples)
{
    if (samples != m_samples) {
        m_samples = samples;
        m_image = QImage(m_samples, 1, QImage::Format_RGB32);
        m_image.fill(0);
        Q_EMIT samplesChanged();
        Q_EMIT imageChanged();
    }
}


/*!
 * \qmltype RenderingTimes
 * \instantiates UCRenderingTimes
 * \inqmlmodule Ubuntu.Components 0.1
 * \brief DOCME
 *
 * Example of use:
 *
 * \qml
 * RenderingTimes {
 * }
 * \endqml
 *
 */

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


UCTextureFromImageTextureProvider::UCTextureFromImageTextureProvider() :
    QSGTextureProvider(),
    m_texture(NULL)
{
}

UCTextureFromImageTextureProvider::~UCTextureFromImageTextureProvider()
{
    if (m_texture != NULL) {
        delete m_texture;
    }
}

QSGTexture* UCTextureFromImageTextureProvider::texture() const
{
    return m_texture;
}

void UCTextureFromImageTextureProvider::setTexture(QSGTexture* texture)
{
    if (m_texture != NULL) {
        delete m_texture;
    }
    m_texture = texture;

    // FIXME: hardcoded flag
    if (m_texture != NULL) {
        m_texture->setHorizontalWrapMode(QSGTexture::Repeat);
    }
}



UCTextureFromImage::UCTextureFromImage(QQuickItem* parent) :
    QQuickItem(parent),
    m_textureProvider(NULL),
    m_textureNeedsUpdate(true)
{
    setFlag(QQuickItem::ItemHasContents);
}

UCTextureFromImage::~UCTextureFromImage()
{
    if (m_textureProvider != NULL) {
        m_textureProvider->deleteLater();
    }
}

bool UCTextureFromImage::isTextureProvider() const
{
    return true;
}

QSGTextureProvider* UCTextureFromImage::textureProvider() const
{
    if (m_textureProvider == NULL) {
        const_cast<UCTextureFromImage*>(this)->m_textureProvider = new UCTextureFromImageTextureProvider;
        m_textureProvider->setTexture(window()->createTextureFromImage(m_image));
    }
    return m_textureProvider;
}

QSGNode* UCTextureFromImage::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
{
    Q_UNUSED(oldNode)
    Q_UNUSED(updatePaintNodeData)

    if (m_textureNeedsUpdate && m_textureProvider != NULL) {
        m_textureProvider->setTexture(window()->createTextureFromImage(m_image));
        m_textureNeedsUpdate = false;
    }
    return NULL;
}

QImage UCTextureFromImage::image() const
{
    return m_image;
}

void UCTextureFromImage::setImage(QImage image)
{
    if (image != m_image) {
        m_image = image;
        Q_EMIT imageChanged();
        m_textureNeedsUpdate = true;
        update();
    }
}
