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

#include "upmrenderingtimes2.h"
#include <QtCore/qmath.h>
#include <QtCore/QDebug>

#if !defined(QT_OPENGL_ES)
#include <QtGui/QOpenGLContext>
#if !defined(GL_TIME_ELAPSED)
#define GL_TIME_ELAPSED 0x88BF
#endif
#endif

UPMRenderingTimes2::UPMRenderingTimes2(QQuickItem* parent) :
    QQuickItem(parent),
    m_period(1000),
    m_graphModel(new UPMGraphModel(this)),
    m_window(NULL)
#if defined(QT_OPENGL_ES)
    , beforeSync_(EGL_NO_SYNC_KHR)
#endif
{
    /* Disable synchronization to vertical blank signal on Intel */
    qputenv("vblank_mode", "0");

    /* Forward samplesChanged signal from graphModel */
    QObject::connect(m_graphModel, &UPMGraphModel::samplesChanged,
                     this, &UPMRenderingTimes2::samplesChanged);

    /* Periodically append render time of the most costly frame rendered.
       The period is period / samples */
    QObject::connect(this, &UPMRenderingTimes2::frameRendered,
                     this, &UPMRenderingTimes2::onFrameRendered);

}

int UPMRenderingTimes2::period() const
{
    return m_period;
}

void UPMRenderingTimes2::setPeriod(int period)
{
    if (period != m_period) {
        m_period = period;
        Q_EMIT periodChanged();
    }
}

int UPMRenderingTimes2::samples() const
{
    return m_graphModel->samples();
}

void UPMRenderingTimes2::setSamples(int samples)
{
    m_graphModel->setSamples(samples);
}

UPMGraphModel* UPMRenderingTimes2::graphModel() const
{
    return m_graphModel;
}

// FIXME: can be replaced with connecting to windowChanged() signal introduced in Qt5.2
void UPMRenderingTimes2::itemChange(ItemChange change, const ItemChangeData & value)
{
    if (change == QQuickItem::ItemSceneChange) {
        connectToWindow(value.window);
    }
    QQuickItem::itemChange(change, value);
}

void UPMRenderingTimes2::connectToWindow(QQuickWindow* window)
{
    if (window != m_window) {
        if (m_window != NULL) {
          QObject::disconnect(m_window, &QQuickWindow::sceneGraphInitialized,
                              this, &UPMRenderingTimes2::onSceneGraphInitialized);
          QObject::disconnect(m_window, &QQuickWindow::sceneGraphInvalidated,
                              this, &UPMRenderingTimes2::onSceneGraphInvalidated);
          QObject::disconnect(m_window, &QQuickWindow::beforeRendering,
                                this, &UPMRenderingTimes2::onBeforeRendering);
            QObject::disconnect(m_window, &QQuickWindow::afterRendering,
                                this, &UPMRenderingTimes2::onAfterRendering);
            QObject::disconnect(m_window, &QQuickWindow::frameSwapped,
                                this, &UPMRenderingTimes2::onFrameSwapped);
        }

        m_window = window;

        if (m_window != NULL) {
            QObject::connect(m_window, &QQuickWindow::sceneGraphInitialized,
                             this, &UPMRenderingTimes2::onSceneGraphInitialized,
                             Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::sceneGraphInvalidated,
                             this, &UPMRenderingTimes2::onSceneGraphInvalidated,
                             Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::beforeRendering,
                                this, &UPMRenderingTimes2::onBeforeRendering, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::afterRendering,
                                this, &UPMRenderingTimes2::onAfterRendering, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::frameSwapped,
                                this, &UPMRenderingTimes2::onFrameSwapped, Qt::DirectConnection);
        }
    }
}

void UPMRenderingTimes2::onSceneGraphInitialized() {
#if defined(QT_OPENGL_ES)
  QList<QByteArray> eglExtensions = QByteArray(
      static_cast<const char*>(eglQueryString(eglGetCurrentDisplay(), EGL_EXTENSIONS))).split(' ');
  QList<QByteArray> glExtensions = QByteArray(
      reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');
  // Note the workaround for PowerVR that declares 'GL_OES_egl_sync'.
  if (eglExtensions.contains("EGL_KHR_fence_sync") &&
      (glExtensions.contains("GL_OES_EGL_sync") || glExtensions.contains("GL_OES_egl_sync"))) {
    fenceSyncKHR_.createSyncKHR = reinterpret_cast<
        EGLSyncKHR (QOPENGLF_APIENTRYP)(EGLDisplay, EGLenum, const EGLint*)>(
            eglGetProcAddress("eglCreateSyncKHR"));
    fenceSyncKHR_.destroySyncKHR = reinterpret_cast<
        EGLBoolean (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR)>(
            eglGetProcAddress("eglDestroySyncKHR"));
    fenceSyncKHR_.clientWaitSyncKHR = reinterpret_cast<
        EGLint (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR, EGLint, EGLTimeKHR)>(
            eglGetProcAddress("eglClientWaitSyncKHR"));
    fenceSystem_ = FenceSyncKHR;
  } else if (glExtensions.contains("GL_NV_fence")) {
    fenceNV_.genFencesNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
        eglGetProcAddress("glGenFencesNV"));
    fenceNV_.deleteFencesNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
        eglGetProcAddress("glDeleteFencesNV"));
    fenceNV_.setFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
        eglGetProcAddress("glSetFenceNV"));
    fenceNV_.finishFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint)>(
        eglGetProcAddress("glFinishFenceNV"));
    fenceSystem_ = FenceNV;
    fenceNV_.genFencesNV(2, fence_);
  } else {
    fenceSystem_ = ExtensionUnavailable;
    fprintf(stderr, "Error: Graphics driver misses some OpenGL ES features.\n");
    QGuiApplication::quit();
  }

#else
  // We could use the thin QOpenGLTimerQuery wrapper from Qt 5.1, but the lack of a method to check
  // the presence of glQueryCounter() would force us to inspect OpenGL version and extensions, which
  // is basically as annoying as doing the whole thing here.
  // TODO(loicm) Add an hasQuerycounter() method to QOpenGLTimerQuery.
  QOpenGLContext* context = QOpenGLContext::currentContext();
  timerQuery_.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
      context->getProcAddress("glGenQueries"));
  timerQuery_.deleteQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
      context->getProcAddress("glDeleteQueries"));
  timerQuery_.beginQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum, GLuint)>(
      context->getProcAddress("glBeginQuery"));
  timerQuery_.endQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum)>(
      context->getProcAddress("glEndQuery"));
  QSurfaceFormat format = context->format();
  if ((format.majorVersion() > 3)
      || ((format.majorVersion() == 3) && (format.minorVersion() > 2))
      || ((format.majorVersion() == 3) && (format.minorVersion() == 2)
          && context->hasExtension(QByteArrayLiteral("GL_ARB_timer_query")))) {
    timerQuery_.getQueryObjectui64v =
        reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64*)>(
            context->getProcAddress("glGetQueryObjectui64v"));
    timerQuery_.queryCounter = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
        context->getProcAddress("glQueryCounter"));
    timerQueryVersion_ = TimerQueryCore;
    timerQuery_.genQueries(2, timer_);
  } else if (context->hasExtension(QByteArrayLiteral("GL_EXT_timer_query"))) {
    timerQuery_.getQueryObjectui64vExt =
        reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64EXT*)>(
            context->getProcAddress("glGetQueryObjectui64vEXT"));
    timerQueryVersion_ = TimerQueryExt;
    timerQuery_.genQueries(1, timer_);
  } else {
    timerQueryVersion_ = TimerQueryUnavailable;
    fprintf(stderr, "Error: Graphics driver misses some OpenGL features.\n");
  }
#endif
}

void UPMRenderingTimes2::onSceneGraphInvalidated() {
#if defined(QT_OPENGL_ES)
  if (fenceSystem_ == FenceSyncKHR && beforeSync_ != EGL_NO_SYNC_KHR) {
    fenceSyncKHR_.destroySyncKHR(eglGetCurrentDisplay(), beforeSync_);
  } else if (fenceSystem_ == FenceNV) {
    fenceNV_.deleteFencesNV(2, fence_);
  }

#else
  if (timerQueryVersion_ == TimerQueryCore) {
    timerQuery_.deleteQueries(2, timer_);
  } else if (timerQueryVersion_ == TimerQueryExt) {
    timerQuery_.deleteQueries(1, timer_);
  }
#endif
}

void UPMRenderingTimes2::startGpuTimer() {
  static bool once = false;
  if (!once) {
    onSceneGraphInitialized();
    once = true;
  }
#if defined(QT_OPENGL_ES)
  if (fenceSystem_ == FenceSyncKHR) {
    beforeSync_ = fenceSyncKHR_.createSyncKHR(eglGetCurrentDisplay(), EGL_SYNC_FENCE_KHR, NULL);
  } else if (fenceSystem_ == FenceSyncKHR) {
    fenceNV_.setFenceNV(fence_[0], GL_ALL_COMPLETED_NV);
  }

#else
  if (timerQueryVersion_ == TimerQueryCore) {
    timerQuery_.queryCounter(timer_[0], GL_TIMESTAMP);
  } else if (timerQueryVersion_ == TimerQueryExt) {
    timerQuery_.beginQuery(GL_TIME_ELAPSED, timer_[0]);
  }
#endif
}

qint64 UPMRenderingTimes2::stopGpuTimer() {
#if defined(QT_OPENGL_ES)
  if (fenceSystem_ == FenceSyncKHR) {
    QElapsedTimer timer;
    EGLDisplay dpy = eglGetCurrentDisplay();
    EGLSyncKHR afterSync = fenceSyncKHR_.createSyncKHR(dpy, EGL_SYNC_FENCE_KHR, NULL);
    EGLint beforeSyncValue = fenceSyncKHR_.clientWaitSyncKHR(dpy, beforeSync_, 0, EGL_FOREVER_KHR);
    qint64 beforeTime = timer.nsecsElapsed();
    EGLint afterSyncValue = fenceSyncKHR_.clientWaitSyncKHR(dpy, afterSync, 0, EGL_FOREVER_KHR);
    qint64 afterTime = timer.nsecsElapsed();
    fenceSyncKHR_.destroySyncKHR(dpy, afterSync);
    fenceSyncKHR_.destroySyncKHR(dpy, beforeSync_);
    beforeSync_ = EGL_NO_SYNC_KHR;
    if (beforeSyncValue == EGL_CONDITION_SATISFIED_KHR
        && afterSyncValue == EGL_CONDITION_SATISFIED_KHR) {
      return afterTime - beforeTime;
    } else {
      return -1;
    }
  } else if (fenceSystem_ == FenceNV) {
    QElapsedTimer timer;
    fenceNV_.setFenceNV(fence_[1], GL_ALL_COMPLETED_NV);
    fenceNV_.finishFenceNV(fence_[0]);
    qint64 beforeTime = timer.nsecsElapsed();
    fenceNV_.finishFenceNV(fence_[1]);
    qint64 afterTime = timer.nsecsElapsed();
    return afterTime - beforeTime;
  } else {
    return -1;
  }

#else
  if (timerQueryVersion_ == TimerQueryCore) {
    GLuint64 time[2] = { 0, 0 };
    timerQuery_.queryCounter(timer_[1], GL_TIMESTAMP);
    timerQuery_.getQueryObjectui64v(timer_[0], GL_QUERY_RESULT, &time[0]);
    timerQuery_.getQueryObjectui64v(timer_[1], GL_QUERY_RESULT, &time[1]);
    if (time[0] != 0 && time[1] != 0) {
      return time[1] - time[0];
    } else {
      return -1;
    }
  } else if (timerQueryVersion_ == TimerQueryExt) {
    GLuint64EXT time;
    timerQuery_.endQuery(GL_TIME_ELAPSED);
    timerQuery_.getQueryObjectui64vExt(timer_[0], GL_QUERY_RESULT, &time);
    return static_cast<qint64>(time);
  } else {
    return -1;
  }
#endif
}

void UPMRenderingTimes2::onBeforeRendering()
{
  // m_renderingTimer.start();
    // m_renderingTimer2.start();
  startGpuTimer();
}

void UPMRenderingTimes2::onAfterRendering()
{
  // qint64 time = m_renderingTimer.nsecsElapsed();
  //   qDebug() << "1: " << time;
  qint64 time = stopGpuTimer();
  if (time == -1) {
    fprintf(stderr, "Error: GPU timing failed.\n");
  }
  //appendRenderTime(time);
  Q_EMIT frameRendered(time);
  //update();
}

void UPMRenderingTimes2::onFrameSwapped()
{

  // qDebug() << "2: " << m_renderingTimer2.nsecsElapsed();
}

void UPMRenderingTimes2::onFrameRendered(qint64 renderTime)
{
  //qDebug() << renderTime;
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

void UPMRenderingTimes2::appendRenderTime(qint64 renderTime)
{
    int renderTimeInMs = qCeil((qreal)renderTime / 1000000);
    int width = ((qreal)samples() / m_period) * renderTimeInMs;
    m_graphModel->appendValue(width, renderTimeInMs);
}
