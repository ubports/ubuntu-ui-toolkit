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

#ifndef UCPERFORMANCE_METRICS_H
#define UCPERFORMANCE_METRICS_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtCore/QElapsedTimer>

class UCGraphModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY imageChanged)
    Q_PROPERTY(int shift READ shift NOTIFY shiftChanged)
    Q_PROPERTY(int samples READ samples WRITE setSamples NOTIFY samplesChanged)

public:
    explicit UCGraphModel(QObject *parent = 0);

    void appendValue(int width, int value);

    // getters
    QImage image() const;
    int shift() const;
    int samples() const;

    // setters
    void setSamples(int samples);

Q_SIGNALS:
    void imageChanged();
    void shiftChanged();
    void samplesChanged();

private:
    QImage m_image;
    int m_shift;
    int m_samples;
};

class UCRenderingTimes : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(int period READ period WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(int samples READ samples WRITE setSamples NOTIFY samplesChanged)
    Q_PROPERTY(UCGraphModel* graphModel READ graphModel NOTIFY graphModelChanged)

public:
    explicit UCRenderingTimes(QQuickItem* parent = 0);

    // getters
    bool enabled() const;
    int period() const;
    int samples() const;
    UCGraphModel* graphModel() const;

    // setters
    void setEnabled(bool enabled);
    void setPeriod(int period);
    void setSamples(int samples);

Q_SIGNALS:
    void enabledChanged();
    void periodChanged();
    void samplesChanged();
    void graphModelChanged();
    void frameRendered(qint64 renderTime);

protected:
    void itemChange(ItemChange change, const ItemChangeData & value);

private Q_SLOTS:
    void connectToWindow(QQuickWindow* window);
    void onSceneGraphInitialized();
    void onBeforeRendering();
    void onAfterRendering();
    void onFrameSwapped();
    void onFrameRendered(qint64 renderTime);

private:
    void appendRenderTime(qint64 renderTime);
    void updateTimeBetweenSamples();

private:
    bool m_enabled;
    int m_period;
    UCGraphModel* m_graphModel;
    QQuickWindow* m_window;
    QElapsedTimer m_renderingTimer;
    QElapsedTimer m_appendTimer;
    qint64 m_highestTime;
    int m_timeBetweenSamples;
};


#include <QtQuick/QSGTextureProvider>
#include <QtQuick/QQuickItem>

class UCTextureFromImageTextureProvider : public QSGTextureProvider
{
    Q_OBJECT

public:
    explicit UCTextureFromImageTextureProvider();
    virtual ~UCTextureFromImageTextureProvider();
    QSGTexture* texture() const;
    void setTexture(QSGTexture* texture);

private:
    QSGTexture* m_texture;
};


class UCTextureFromImage : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

public:
    explicit UCTextureFromImage(QQuickItem* parent = 0);
    virtual ~UCTextureFromImage();
    bool isTextureProvider() const;
    QSGTextureProvider* textureProvider() const;
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData);

    // getter
    QImage image() const;

    // setters
    void setImage(QImage image);

Q_SIGNALS:
    void imageChanged();

private:
    UCTextureFromImageTextureProvider* m_textureProvider;
    QImage m_image;
    bool m_textureNeedsUpdate;
};

#endif // UCPERFORMANCE_METRICS_H
