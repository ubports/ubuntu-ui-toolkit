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

#ifndef UPMPERFORMANCE_METRICS_H
#define UPMPERFORMANCE_METRICS_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtCore/QElapsedTimer>
#include "upmgraphmodel.h"
#include "rendertimer.h"

class UPMRenderingTimes : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(int period READ period WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(int samples READ samples WRITE setSamples NOTIFY samplesChanged)
    Q_PROPERTY(UPMGraphModel* graphModel READ graphModel NOTIFY graphModelChanged)
    Q_PROPERTY(Technique technique READ technique WRITE setTechnique NOTIFY techniqueChanged)

    Q_ENUMS(Technique)

public:
    enum Technique {
        Trivial,
        Fences
    };

    explicit UPMRenderingTimes(QQuickItem* parent = 0);
    ~UPMRenderingTimes();

    // getters
    int period() const;
    int samples() const;
    UPMGraphModel* graphModel() const;
    Technique technique() const;

    // setters
    void setPeriod(int period);
    void setSamples(int samples);
    void setTechnique(Technique technique);

Q_SIGNALS:
    void periodChanged();
    void samplesChanged();
    void graphModelChanged();
    void techniqueChanged();
    void frameRendered(qint64 renderTime);

protected:
    void itemChange(ItemChange change, const ItemChangeData & value);

private Q_SLOTS:
    void connectToWindow(QQuickWindow* window);
    void onSceneGraphInitialized();
    void onSceneGraphInvalidated();
    void onBeforeRendering();
    void onAfterRendering();
    void onFrameSwapped();
    void onFrameRendered(qint64 renderTime);

private:
    void appendRenderTime(qint64 renderTime);
    void updateTimeBetweenSamples();

private:
    int m_period;
    UPMGraphModel* m_graphModel;
    Technique m_technique;
    QQuickWindow* m_window;
    RenderTimer* m_renderingTimer;
    bool m_oddFrame;
    qint64 m_oddFrameRenderTime;
};

#endif // UPMPERFORMANCE_METRICS_H
