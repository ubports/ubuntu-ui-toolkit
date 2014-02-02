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

#include "rendertimerfences.h"
#include <QtCore/QDebug>

#if !defined(QT_OPENGL_ES)
#include <QtGui/QOpenGLContext>
#if !defined(GL_TIME_ELAPSED)
#define GL_TIME_ELAPSED 0x88BF
#endif
#endif

RenderTimerFences::RenderTimerFences()
#if defined(QT_OPENGL_ES)
    : beforeSync_(EGL_NO_SYNC_KHR),
      fenceSystem_(ExtensionUnavailable)
#else
    : timerQueryVersion_(TimerQueryUnavailable)
#endif
{
}

void RenderTimerFences::setup()
{
#if defined(QT_OPENGL_ES)
    QList<QByteArray> eglExtensions = QByteArray(
        static_cast<const char*>(eglQueryString(eglGetCurrentDisplay(), EGL_EXTENSIONS))).split(' ');
    QList<QByteArray> glExtensions = QByteArray(
        reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');
    // Note the workaround for PowerVR that declares 'GL_OES_egl_sync'.
    if (eglExtensions.contains("EGL_KHR_fence_sync") &&
        (glExtensions.contains("GL_OES_EGL_sync") || glExtensions.contains("GL_OES_egl_sync"))) {
        setupKHRFence();
    } else if (glExtensions.contains("GL_NV_fence")) {
        setupNVFence();
    } else {
        qCritical("Error: Graphics driver misses some OpenGL ES features.");
    }
#else
    // We could use the thin QOpenGLTimerQuery wrapper from Qt 5.1, but the lack of a method to check
    // the presence of glQueryCounter() would force us to inspect OpenGL version and extensions, which
    // is basically as annoying as doing the whole thing here.
    // TODO(loicm) Add an hasQuerycounter() method to QOpenGLTimerQuery.
    QOpenGLContext* context = QOpenGLContext::currentContext();
    QSurfaceFormat format = context->format();
    if (qMakePair(format.majorVersion(), format.minorVersion()) >= qMakePair(3, 2)
        && context->hasExtension(QByteArrayLiteral("GL_ARB_timer_query"))) {
        setupARBTimerQuery();
    } else if (context->hasExtension(QByteArrayLiteral("GL_EXT_timer_query"))) {
        setupEXTTimerQuery();
    } else {
        qCritical("Error: Graphics driver misses some OpenGL features.");
    }
#endif
}

void RenderTimerFences::teardown()
{
#if defined(QT_OPENGL_ES)
    if (fenceSystem_ == FenceSyncKHR) {
        teardownKHRFence();
    } else if (fenceSystem_ == FenceNV) {
        teardownNVFence();
    }
#else
    if (timerQueryVersion_ == TimerQueryCore) {
        teardownARBTimerQuery();
    } else if (timerQueryVersion_ == TimerQueryExt) {
        teardownEXTTimerQuery();
    }
#endif
}

void RenderTimerFences::start()
{
#if defined(QT_OPENGL_ES)
    if (fenceSystem_ == FenceSyncKHR) {
        startKHRFence();
    } else if (fenceSystem_ == FenceSyncKHR) {
        startNVFence();
    }
#else
    if (timerQueryVersion_ == TimerQueryCore) {
        startARBTimerQuery();
    } else if (timerQueryVersion_ == TimerQueryExt) {
        startEXTTimerQuery();
    }
#endif
}

qint64 RenderTimerFences::stop()
{
#if defined(QT_OPENGL_ES)
    if (fenceSystem_ == FenceSyncKHR) {
        return stopKHRFence();
    } else if (fenceSystem_ == FenceNV) {
        return stopNVFence();
    } else {
        return -1;
    }
#else
    if (timerQueryVersion_ == TimerQueryCore) {
        return stopARBTimerQuery();
    } else if (timerQueryVersion_ == TimerQueryExt) {
        return stopEXTTimerQuery();
    } else {
        return -1;
    }
#endif
}

#if defined(QT_OPENGL_ES)

void RenderTimerFences::setupKHRFence()
{
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
}

void RenderTimerFences::teardownKHRFence()
{
    if (beforeSync_ != EGL_NO_SYNC_KHR) {
        fenceSyncKHR_.destroySyncKHR(eglGetCurrentDisplay(), beforeSync_);
    }
}

void RenderTimerFences::startKHRFence()
{
    beforeSync_ = fenceSyncKHR_.createSyncKHR(eglGetCurrentDisplay(), EGL_SYNC_FENCE_KHR, NULL);
}

qint64 RenderTimerFences::stopKHRFence()
{
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
}

void RenderTimerFences::setupNVFence()
{
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
}

void RenderTimerFences::teardownNVFence()
{
    fenceNV_.deleteFencesNV(2, fence_);
}

void RenderTimerFences::startNVFence()
{
    fenceNV_.setFenceNV(fence_[0], GL_ALL_COMPLETED_NV);
}

qint64 RenderTimerFences::stopNVFence()
{
    QElapsedTimer timer;
    fenceNV_.setFenceNV(fence_[1], GL_ALL_COMPLETED_NV);
    fenceNV_.finishFenceNV(fence_[0]);
    qint64 beforeTime = timer.nsecsElapsed();
    fenceNV_.finishFenceNV(fence_[1]);
    qint64 afterTime = timer.nsecsElapsed();
    return afterTime - beforeTime;
}

#else

void RenderTimerFences::setupARBTimerQuery()
{
    QOpenGLContext* context = QOpenGLContext::currentContext();
    timerQuery_.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
        context->getProcAddress("glGenQueries"));
    timerQuery_.deleteQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
        context->getProcAddress("glDeleteQueries"));
    timerQuery_.getQueryObjectui64v =
    reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64*)>(
        context->getProcAddress("glGetQueryObjectui64v"));
    timerQuery_.queryCounter = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
        context->getProcAddress("glQueryCounter"));
    timerQueryVersion_ = TimerQueryCore;
    timerQuery_.genQueries(2, timer_);
}

void RenderTimerFences::teardownARBTimerQuery()
{
    timerQuery_.deleteQueries(2, timer_);
}

void RenderTimerFences::startARBTimerQuery()
{
    timerQuery_.queryCounter(timer_[0], GL_TIMESTAMP);
}

qint64 RenderTimerFences::stopARBTimerQuery()
{
    GLuint64 time[2] = { 0, 0 };
    timerQuery_.queryCounter(timer_[1], GL_TIMESTAMP);
    timerQuery_.getQueryObjectui64v(timer_[0], GL_QUERY_RESULT, &time[0]);
    timerQuery_.getQueryObjectui64v(timer_[1], GL_QUERY_RESULT, &time[1]);
    if (time[0] != 0 && time[1] != 0) {
        return time[1] - time[0];
    } else {
        return -1;
    }
}

void RenderTimerFences::setupEXTTimerQuery()
{
    QOpenGLContext* context = QOpenGLContext::currentContext();
    timerQuery_.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
        context->getProcAddress("glGenQueries"));
    timerQuery_.deleteQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
        context->getProcAddress("glDeleteQueries"));
    timerQuery_.beginQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum, GLuint)>(
        context->getProcAddress("glBeginQuery"));
    timerQuery_.endQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum)>(
        context->getProcAddress("glEndQuery"));
    timerQuery_.getQueryObjectui64vExt =
    reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64EXT*)>(
        context->getProcAddress("glGetQueryObjectui64vEXT"));
    timerQueryVersion_ = TimerQueryExt;
    timerQuery_.genQueries(1, timer_);
}

void RenderTimerFences::teardownEXTTimerQuery()
{
    timerQuery_.deleteQueries(1, timer_);
}

void RenderTimerFences::startEXTTimerQuery()
{
    timerQuery_.beginQuery(GL_TIME_ELAPSED, timer_[0]);
}

qint64 RenderTimerFences::stopEXTTimerQuery()
{
    GLuint64EXT time;
    timerQuery_.endQuery(GL_TIME_ELAPSED);
    timerQuery_.getQueryObjectui64vExt(timer_[0], GL_QUERY_RESULT, &time);
    return static_cast<qint64>(time);
}

#endif
