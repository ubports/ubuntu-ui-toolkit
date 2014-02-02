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

#ifndef UPMPERFORMANCE_METRICS2_H
#define UPMPERFORMANCE_METRICS2_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtCore/QElapsedTimer>
#include "upmgraphmodel.h"
#if defined(QT_OPENGL_ES)
#include <EGL/egl.h>
#include <EGL/eglext.h>
#else
#include <QtGui/qopenglfunctions.h>
#endif

class UPMRenderingTimes2 : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(int period READ period WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(int samples READ samples WRITE setSamples NOTIFY samplesChanged)
    Q_PROPERTY(UPMGraphModel* graphModel READ graphModel NOTIFY graphModelChanged)

public:
    explicit UPMRenderingTimes2(QQuickItem* parent = 0);

    // getters
    int period() const;
    int samples() const;
    UPMGraphModel* graphModel() const;

    // setters
    void setPeriod(int period);
    void setSamples(int samples);

Q_SIGNALS:
    void periodChanged();
    void samplesChanged();
    void graphModelChanged();
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
    void startGpuTimer();
    qint64 stopGpuTimer();
    void appendRenderTime(qint64 renderTime);
    void updateTimeBetweenSamples();

private:
    int m_period;
    UPMGraphModel* m_graphModel;
    QQuickWindow* m_window;
    QElapsedTimer m_renderingTimer;
    QElapsedTimer m_renderingTimer2;

#if defined(QT_OPENGL_ES)
  struct {
    EGLSyncKHR (QOPENGLF_APIENTRYP createSyncKHR)(EGLDisplay dpy, EGLenum type,
                                                  const EGLint* attrib_list);
    EGLBoolean (QOPENGLF_APIENTRYP destroySyncKHR)(EGLDisplay dpy, EGLSyncKHR sync);
    EGLint (QOPENGLF_APIENTRYP clientWaitSyncKHR)(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags,
                                                  EGLTimeKHR timeout);
  } fenceSyncKHR_;
  EGLSyncKHR beforeSync_;
  struct {
    void (QOPENGLF_APIENTRYP genFencesNV)(GLsizei n, GLuint* fences);
    void (QOPENGLF_APIENTRYP deleteFencesNV)(GLsizei n, const GLuint* fences);
    void (QOPENGLF_APIENTRYP setFenceNV)(GLuint fence, GLenum condition);
    void (QOPENGLF_APIENTRYP finishFenceNV)(GLuint fence);
  } fenceNV_;
  GLuint fence_[2];
  enum { ExtensionUnavailable, FenceSyncKHR, FenceNV } fenceSystem_;

#else
  struct {
    void (QOPENGLF_APIENTRYP genQueries)(GLsizei n, GLuint* ids);
    void (QOPENGLF_APIENTRYP deleteQueries)(GLsizei n, const GLuint* ids);
    void (QOPENGLF_APIENTRYP beginQuery)(GLenum target, GLuint id);
    void (QOPENGLF_APIENTRYP endQuery)(GLenum target);
    void (QOPENGLF_APIENTRYP getQueryObjectui64v)(GLuint id, GLenum pname, GLuint64* params);
    void (QOPENGLF_APIENTRYP getQueryObjectui64vExt)(GLuint id, GLenum pname, GLuint64EXT* params);
    void (QOPENGLF_APIENTRYP queryCounter)(GLuint id, GLenum target);
  } timerQuery_;
  enum { TimerQueryUnavailable, TimerQueryCore, TimerQueryExt } timerQueryVersion_;
  GLuint timer_[2];
#endif

};

#endif // UPMPERFORMANCE_METRICS2_H
