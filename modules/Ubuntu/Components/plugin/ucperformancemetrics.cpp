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

UCGraphModel::UCGraphModel(QObject *parent) :
    QObject(parent),
    m_shift(0),
    m_samples(100)
{
    m_image = QImage(m_samples, 1, QImage::Format_RGB32);
    m_image.fill(0);
}

void UCGraphModel::appendValue(qint64 value)
{
    /* FIXME: modifying m_image here implicitly triggers a deep copy
       of its data because UCTextureFromImage usually holds a reference
       to that image
    */
    QRgb* line = (QRgb*)m_image.scanLine(0);
    line[m_shift] = qRgb(value, value, value);
    m_shift = (m_shift + 1) % m_samples;

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
    m_accumulatedTime(0),
    m_highestTime(0),
    m_accumulatedFrames(0)
{
    /* Disable synchronization to vertical blank signal on Intel */
    qputenv("vblank_mode", "0");

    /* Forward samplesChanged signal from graphModel */
    QObject::connect(m_graphModel, &UCGraphModel::samplesChanged,
                     this, &UCRenderingTimes::samplesChanged);

    /* Periodically append render time of the most costly frame rendered.
       The period is period / samples */
    QObject::connect(this, &UCRenderingTimes::frameRendered,
                     this, &UCRenderingTimes::accumulateRenderTime);
    QObject::connect(&m_timer, &QTimer::timeout,
                     this, &UCRenderingTimes::onTimerTimeout);
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
    m_elapsedTimer.start();
}

void UCRenderingTimes::onAfterRendering()
{
}

void UCRenderingTimes::onFrameSwapped()
{
    Q_EMIT frameRendered(m_elapsedTimer.nsecsElapsed() / 1000000);
}

void UCRenderingTimes::onTimerTimeout()
{
    if (m_accumulatedFrames > 1) {
        appendRenderTime(m_highestTime);
    }
}

void UCRenderingTimes::accumulateRenderTime(qint64 renderTime)
{
    /* Only append render times after a given period of time.
       Append the highest render time recorded in the past period
    */
    m_highestTime = qMax(renderTime, m_highestTime);
    m_accumulatedTime += renderTime;
    m_accumulatedFrames++;

    float timeout = m_period / m_graphModel->samples();
    if (m_accumulatedTime >= timeout) {
        m_timer.stop();
        appendRenderTime(m_highestTime);
    } else if (!m_timer.isActive()) {
        m_timer.start(timeout);
    }
}

void UCRenderingTimes::appendRenderTime(qint64 renderTime)
{
    m_accumulatedFrames = 0;
    m_accumulatedTime = 0;
    m_highestTime = 0;
    m_graphModel->appendValue(renderTime);
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
    m_textureProvider(new UCTextureFromImageTextureProvider),
    m_textureNeedsUpdate(true)
{
    setFlag(QQuickItem::ItemHasContents);
}

UCTextureFromImage::~UCTextureFromImage()
{
    delete m_textureProvider;
}

bool UCTextureFromImage::isTextureProvider() const
{
    return true;
}

QSGTextureProvider* UCTextureFromImage::textureProvider() const
{
    return m_textureProvider;
}

QSGNode* UCTextureFromImage::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
{
    Q_UNUSED(oldNode)
    Q_UNUSED(updatePaintNodeData)

    if (m_textureNeedsUpdate) {
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
