// Copyright © 2016 Canonical Ltd.
// Author: Loïc Molinari <loic.molinari@canonical.com>
//
// This file is part of Ubuntu UI Toolkit.
//
// Ubuntu UI Toolkit is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; version 3.
//
// Ubuntu UI Toolkit is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ubuntu UI Toolkit. If not, see <http://www.gnu.org/licenses/>.

#include "gputimer_p.h"
#include "ubuntumetricsglobal_p.h"
#include <QtCore/QElapsedTimer>

#if !defined(QT_OPENGL_ES) && !defined(GL_TIME_ELAPSED)
#define GL_TIME_ELAPSED 0x88BF  // For GL_EXT_timer_query.
#endif

void GPUTimer::initialize()
{
    DASSERT(QOpenGLContext::currentContext());
    DASSERT(m_type == Unset);

#if !defined QT_NO_DEBUG
    m_context = QOpenGLContext::currentContext();
#endif

#if defined(QT_OPENGL_ES)
    QList<QByteArray> eglExtensions = QByteArray(
        static_cast<const char*>(
            eglQueryString(eglGetCurrentDisplay(), EGL_EXTENSIONS))).split(' ');
    QList<QByteArray> glExtensions = QByteArray(
        reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');

    // KHRFence.
    if (eglExtensions.contains("EGL_KHR_fence_sync")
        && (glExtensions.contains("GL_OES_EGL_sync")
            || glExtensions.contains("GL_OES_egl_sync") /*PowerVR fix*/)) {
        m_fenceSyncKHR.createSyncKHR = reinterpret_cast<
            EGLSyncKHR (QOPENGLF_APIENTRYP)(EGLDisplay, EGLenum, const EGLint*)>(
                eglGetProcAddress("eglCreateSyncKHR"));
        m_fenceSyncKHR.destroySyncKHR = reinterpret_cast<
            EGLBoolean (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR)>(
                eglGetProcAddress("eglDestroySyncKHR"));
        m_fenceSyncKHR.clientWaitSyncKHR = reinterpret_cast<
            EGLint (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR, EGLint, EGLTimeKHR)>(
                eglGetProcAddress("eglClientWaitSyncKHR"));
        m_type = KHRFence;
        DLOG("GPUTimer is based on GL_OES_EGL_sync");

    // NVFence.
    } else if (glExtensions.contains("GL_NV_fence")) {
        m_fenceNV.genFencesNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            eglGetProcAddress("glGenFencesNV"));
        m_fenceNV.deleteFencesNV =
            reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
                eglGetProcAddress("glDeleteFencesNV"));
        m_fenceNV.setFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
            eglGetProcAddress("glSetFenceNV"));
        m_fenceNV.finishFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint)>(
            eglGetProcAddress("glFinishFenceNV"));
        m_fenceNV.genFencesNV(2, m_fence);
        m_type = NVFence;
        DLOG("GPUTimer is based on GL_NV_fence");
    }
#else
    // We could use the thin QOpenGLTimerQuery wrapper from Qt 5.1, but the lack
    // of a method to check the presence of glQueryCounter() would force us to
    // inspect OpenGL version and extensions, which is basically as annoying as
    // doing the whole thing here.
    // TODO(loicm) Add an hasQuerycounter() method to QOpenGLTimerQuery.
    QOpenGLContext* context = QOpenGLContext::currentContext();
    QSurfaceFormat format = context->format();

    // ARBTimerQuery.
    if (qMakePair(format.majorVersion(), format.minorVersion()) >= qMakePair(3, 2)
        && context->hasExtension(QByteArrayLiteral("GL_ARB_timer_query"))) {
        m_timerQuery.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            context->getProcAddress("glGenQueries"));
        m_timerQuery.deleteQueries =
            reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
                context->getProcAddress("glDeleteQueries"));
        m_timerQuery.getQueryObjectui64v =
            reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64*)>(
                context->getProcAddress("glGetQueryObjectui64v"));
        m_timerQuery.queryCounter = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
            context->getProcAddress("glQueryCounter"));
        m_timerQuery.genQueries(2, m_timer);
        m_type = ARBTimerQuery;
        DLOG("GPUTimer is based on GL_ARB_timer_query");

    // EXTTimerQuery.
    } else if (context->hasExtension(QByteArrayLiteral("GL_EXT_timer_query"))) {
        m_timerQuery.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            context->getProcAddress("glGenQueries"));
        m_timerQuery.deleteQueries =
            reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
                context->getProcAddress("glDeleteQueries"));
        m_timerQuery.beginQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum, GLuint)>(
            context->getProcAddress("glBeginQuery"));
        m_timerQuery.endQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum)>(
            context->getProcAddress("glEndQuery"));
        m_timerQuery.getQueryObjectui64vExt =
            reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64EXT*)>(
                context->getProcAddress("glGetQueryObjectui64vEXT"));
        m_timerQuery.genQueries(1, m_timer);
        m_type = EXTTimerQuery;
        DLOG("GPUTimer is based on GL_EXT_timer_query");
    }
#endif

    else {
        m_type = Finish;
        DLOG("GPUTimer is based on glFinish");
    }
}

void GPUTimer::finalize()
{
    DASSERT(m_context == QOpenGLContext::currentContext());
    DASSERT(m_type != Unset);

#if !defined QT_NO_DEBUG
    m_context = nullptr;
#endif

#if defined(QT_OPENGL_ES)
    // KHRFence.
    if (m_type == KHRFence) {
        if (m_beforeSync != EGL_NO_SYNC_KHR) {
            m_fenceSyncKHR.destroySyncKHR(eglGetCurrentDisplay(), m_beforeSync);
        }
        m_type = Unset;

    // NVFence.
    } else if (m_type == NVFence) {
        m_fenceNV.deleteFencesNV(2, m_fence);
        m_type = Unset;
    }
#else
    // ARBTimerQuery.
    if (m_type == ARBTimerQuery) {
        m_timerQuery.deleteQueries(2, m_timer);
        m_type = Unset;

    // EXTTimerQuery.
    } else if (m_type == EXTTimerQuery) {
        m_timerQuery.deleteQueries(1, m_timer);
        m_type = Unset;
    }
#endif
}

void GPUTimer::start()
{
    DASSERT(m_context == QOpenGLContext::currentContext());
    DASSERT(m_type != Unset);
    DASSERT(!m_started);

#if !defined QT_NO_DEBUG
    m_started = true;
#endif

#if defined(QT_OPENGL_ES)
    // KHRFence.
    if (m_type == KHRFence) {
        m_beforeSync = m_fenceSyncKHR.createSyncKHR(
            eglGetCurrentDisplay(), EGL_SYNC_FENCE_KHR, NULL);

    // NVFence.
    } else if (m_type == NVFence) {
        m_fenceNV.setFenceNV(m_fence[0], GL_ALL_COMPLETED_NV);
    }
#else
    // ARBTimerQuery.
    if (m_type == ARBTimerQuery) {
        m_timerQuery.queryCounter(m_timer[0], GL_TIMESTAMP);

    // EXTTimerQuery.
    } else if (m_type == EXTTimerQuery) {
        m_timerQuery.beginQuery(GL_TIME_ELAPSED, m_timer[0]);
    }
#endif
}

quint64 GPUTimer::stop()
{
    DASSERT(m_context == QOpenGLContext::currentContext());
    DASSERT(m_type != Unset);
    DASSERT(m_started);

#if !defined QT_NO_DEBUG
    m_started = false;
#endif

#if defined(QT_OPENGL_ES)
    // KHRFence.
    if (m_type == KHRFence) {
        QElapsedTimer timer;
        EGLDisplay dpy = eglGetCurrentDisplay();
        EGLSyncKHR afterSync = m_fenceSyncKHR.createSyncKHR(dpy, EGL_SYNC_FENCE_KHR, NULL);
        EGLint beforeSyncValue =
            m_fenceSyncKHR.clientWaitSyncKHR(dpy, m_beforeSync, 0, EGL_FOREVER_KHR);
        quint64 beforeTime = timer.nsecsElapsed();
        EGLint afterSyncValue =
            m_fenceSyncKHR.clientWaitSyncKHR(
                dpy, afterSync, EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, EGL_FOREVER_KHR);
        quint64 afterTime = timer.nsecsElapsed();
        m_fenceSyncKHR.destroySyncKHR(dpy, afterSync);
        m_fenceSyncKHR.destroySyncKHR(dpy, m_beforeSync);
        m_beforeSync = EGL_NO_SYNC_KHR;
        if (beforeSyncValue == EGL_CONDITION_SATISFIED_KHR
            && afterSyncValue == EGL_CONDITION_SATISFIED_KHR) {
            return afterTime - beforeTime;
        } else {
            return 0;
        }

    // NVFence.
    } else if (m_type == NVFence) {
        QElapsedTimer timer;
        m_fenceNV.setFenceNV(m_fence[1], GL_ALL_COMPLETED_NV);
        m_fenceNV.finishFenceNV(m_fence[0]);
        quint64 beforeTime = timer.nsecsElapsed();
        m_fenceNV.finishFenceNV(m_fence[1]);
        quint64 afterTime = timer.nsecsElapsed();
        return afterTime - beforeTime;
    }
#else
    // ARBTimerQuery.
    if (m_type == ARBTimerQuery) {
        GLuint64 time[2] = { 0, 0 };
        m_timerQuery.queryCounter(m_timer[1], GL_TIMESTAMP);
        m_timerQuery.getQueryObjectui64v(m_timer[0], GL_QUERY_RESULT, &time[0]);
        m_timerQuery.getQueryObjectui64v(m_timer[1], GL_QUERY_RESULT, &time[1]);
        if (time[0] != 0 && time[1] != 0) {
            return time[1] - time[0];
        } else {
            return -1;
        }

    // EXTTimerQuery.
    } else if (m_type == EXTTimerQuery) {
        GLuint64EXT time;
        m_timerQuery.endQuery(GL_TIME_ELAPSED);
        m_timerQuery.getQueryObjectui64vExt(m_timer[0], GL_QUERY_RESULT, &time);
        return time;
    }
#endif
    // Finish.
    else {
        QOpenGLFunctions* functions = QOpenGLContext::currentContext()->functions();
        QElapsedTimer timer;
        timer.start();
        functions->glFinish();
        return static_cast<quint64>(timer.nsecsElapsed());
    }

    DNOT_REACHED();
    return 0;
}
