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

#include <QtCore/QPair>
#include <QtGui/QOpenGLContext>
#include "rendertimer.h"

#if defined(QT_OPENGL_ES)
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#if !defined(GL_TIME_ELAPSED)
#define GL_TIME_ELAPSED 0x88BF
#endif

class RenderTimerPrivate
{
public:

    RenderTimerPrivate() :
        m_type(RenderTimer::Trivial)
    { }

    RenderTimer::TimerType m_type;
    QElapsedTimer m_trivialTimer;

#if defined(QT_OPENGL_ES)
    struct {
        void (QOPENGLF_APIENTRYP genFencesNV)(GLsizei n, GLuint* fences);
        void (QOPENGLF_APIENTRYP deleteFencesNV)(GLsizei n, const GLuint* fences);
        void (QOPENGLF_APIENTRYP setFenceNV)(GLuint fence, GLenum condition);
        void (QOPENGLF_APIENTRYP finishFenceNV)(GLuint fence);
    } m_fenceNV;
    GLuint m_fence[2];

    struct {
        EGLSyncKHR (QOPENGLF_APIENTRYP createSyncKHR)(EGLDisplay dpy, EGLenum type,
                                                      const EGLint* attrib_list);
        EGLBoolean (QOPENGLF_APIENTRYP destroySyncKHR)(EGLDisplay dpy, EGLSyncKHR sync);
        EGLint (QOPENGLF_APIENTRYP clientWaitSyncKHR)(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags,
                                                      EGLTimeKHR timeout);
    } m_fenceSyncKHR;
    EGLSyncKHR m_beforeSync;
#else
    struct {
        void (QOPENGLF_APIENTRYP genQueries)(GLsizei n, GLuint* ids);
        void (QOPENGLF_APIENTRYP deleteQueries)(GLsizei n, const GLuint* ids);
        void (QOPENGLF_APIENTRYP beginQuery)(GLenum target, GLuint id);
        void (QOPENGLF_APIENTRYP endQuery)(GLenum target);
        void (QOPENGLF_APIENTRYP getQueryObjectui64v)(GLuint id, GLenum pname, GLuint64* params);
        void (QOPENGLF_APIENTRYP getQueryObjectui64vExt)(GLuint id, GLenum pname, GLuint64EXT* params);
        void (QOPENGLF_APIENTRYP queryCounter)(GLuint id, GLenum target);
    } m_timerQuery;
    enum { TimerQueryUnavailable, TimerQueryCore, TimerQueryExt } m_timerQueryVersion;
    GLuint m_timer[2];
#endif
};

RenderTimer::RenderTimer(QObject* parent) :
    QObject(parent)
{
    d_ptr = new RenderTimerPrivate;
}

RenderTimer::~RenderTimer()
{
    delete d_ptr;
}

bool RenderTimer::isAvailable(TimerType type)
{
    if (type == RenderTimer::Trivial) {
        return true;

#if defined(QT_OPENGL_ES)
    } else if (type == RenderTimer::KHRFence) {
        QList<QByteArray> eglExtensions = QByteArray(
            static_cast<const char*>(eglQueryString(eglGetCurrentDisplay(), EGL_EXTENSIONS))).split(' ');
        QList<QByteArray> glExtensions = QByteArray(
            reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');
        // Note the workaround for PowerVR that declares 'GL_OES_egl_sync'.
        return eglExtensions.contains("EGL_KHR_fence_sync") &&
                (glExtensions.contains("GL_OES_EGL_sync") || glExtensions.contains("GL_OES_egl_sync"));

    } else if (type == RenderTimer::NVFence) {
        QList<QByteArray> glExtensions = QByteArray(
            reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');
        return glExtensions.contains("GL_NV_fence");
#else
    } else if (type == RenderTimer::ARBTimerQuery) {
        // We could use the thin QOpenGLTimerQuery wrapper from Qt 5.1, but the lack of a method to check
        // the presence of glQueryCounter() would force us to inspect OpenGL version and extensions, which
        // is basically as annoying as doing the whole thing here.
        // TODO(loicm) Add an hasQuerycounter() method to QOpenGLTimerQuery.
        QOpenGLContext* context = QOpenGLContext::currentContext();
        QSurfaceFormat format = context->format();
        return qMakePair(format.majorVersion(), format.minorVersion()) >= qMakePair(3, 2)
                && context->hasExtension(QByteArrayLiteral("GL_ARB_timer_query"));

    } else if (type == RenderTimer::EXTTimerQuery) {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        return context->hasExtension(QByteArrayLiteral("GL_EXT_timer_query"));
#endif
    }

    return false;
}

RenderTimer::TimerType RenderTimer::optimalTimerType()
{
    QList<TimerType> types;
#if defined(QT_OPENGL_ES)
    types << RenderTimer::KHRFence << RenderTimer::NVFence;
#else
    types << RenderTimer::ARBTimerQuery << RenderTimer::EXTTimerQuery;
#endif

    Q_FOREACH(TimerType type, types) {
        if (RenderTimer::isAvailable(type)) {
            return type;
        }
    }

    return RenderTimer::Trivial;
}

void RenderTimer::setup(TimerType type)
{
    Q_D(RenderTimer);
    if (type == RenderTimer::Automatic) {
        type = RenderTimer::optimalTimerType();
    }

    d->m_type = type;

    if (d->m_type == RenderTimer::Trivial) {
#if defined(QT_OPENGL_ES)
    } else if (d->m_type == RenderTimer::KHRFence) {
        d->m_fenceSyncKHR.createSyncKHR = reinterpret_cast<
            EGLSyncKHR (QOPENGLF_APIENTRYP)(EGLDisplay, EGLenum, const EGLint*)>(
            eglGetProcAddress("eglCreateSyncKHR"));
        d->m_fenceSyncKHR.destroySyncKHR = reinterpret_cast<
            EGLBoolean (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR)>(
            eglGetProcAddress("eglDestroySyncKHR"));
        d->m_fenceSyncKHR.clientWaitSyncKHR = reinterpret_cast<
            EGLint (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR, EGLint, EGLTimeKHR)>(
            eglGetProcAddress("eglClientWaitSyncKHR"));

    } else if (d->m_type == RenderTimer::NVFence) {
        d->m_fenceNV.genFencesNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            eglGetProcAddress("glGenFencesNV"));
        d->m_fenceNV.deleteFencesNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
            eglGetProcAddress("glDeleteFencesNV"));
        d->m_fenceNV.setFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
            eglGetProcAddress("glSetFenceNV"));
        d->m_fenceNV.finishFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint)>(
            eglGetProcAddress("glFinishFenceNV"));
        d->m_fenceNV.genFencesNV(2, d->m_fence);
#else
    } else if (d->m_type == RenderTimer::ARBTimerQuery) {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        d->m_timerQuery.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            context->getProcAddress("glGenQueries"));
        d->m_timerQuery.deleteQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
            context->getProcAddress("glDeleteQueries"));
        d->m_timerQuery.getQueryObjectui64v =
        reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64*)>(
            context->getProcAddress("glGetQueryObjectui64v"));
        d->m_timerQuery.queryCounter = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
            context->getProcAddress("glQueryCounter"));
        d->m_timerQueryVersion = RenderTimerPrivate::TimerQueryCore;
        d->m_timerQuery.genQueries(2, d->m_timer);

    } else if (d->m_type == RenderTimer::EXTTimerQuery) {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        d->m_timerQuery.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
            context->getProcAddress("glGenQueries"));
        d->m_timerQuery.deleteQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
            context->getProcAddress("glDeleteQueries"));
        d->m_timerQuery.beginQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum, GLuint)>(
            context->getProcAddress("glBeginQuery"));
        d->m_timerQuery.endQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum)>(
            context->getProcAddress("glEndQuery"));
        d->m_timerQuery.getQueryObjectui64vExt =
        reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64EXT*)>(
            context->getProcAddress("glGetQueryObjectui64vEXT"));
        d->m_timerQueryVersion = RenderTimerPrivate::TimerQueryExt;
        d->m_timerQuery.genQueries(1, d->m_timer);
#endif
    }
}

void RenderTimer::teardown()
{
    teardown(d_ptr->m_type);
}

void RenderTimer::teardown(TimerType type)
{
    Q_D(RenderTimer);
    if (type == RenderTimer::Trivial) {
#if defined(QT_OPENGL_ES)
    } else if (type == RenderTimer::KHRFence) {
        if (d->m_beforeSync != EGL_NO_SYNC_KHR) {
            d->m_fenceSyncKHR.destroySyncKHR(eglGetCurrentDisplay(), d->m_beforeSync);
        }

    } else if (type == RenderTimer::NVFence) {
        d->m_fenceNV.deleteFencesNV(2, d->m_fence);
#else
    } else if (type == RenderTimer::ARBTimerQuery) {
        d->m_timerQuery.deleteQueries(2, d->m_timer);

    } else if (type == RenderTimer::EXTTimerQuery) {
        d->m_timerQuery.deleteQueries(1, d->m_timer);
#endif
    }
}

void RenderTimer::start()
{
    Q_D(RenderTimer);
    if (d->m_type == RenderTimer::Trivial) {
        d-> m_trivialTimer.start();
#if defined(QT_OPENGL_ES)
    } else if (d->m_type == RenderTimer::KHRFence) {
        d->m_beforeSync = d->m_fenceSyncKHR.createSyncKHR(eglGetCurrentDisplay(), EGL_SYNC_FENCE_KHR, NULL);

    } else if (d->m_type == RenderTimer::NVFence) {
        d->m_fenceNV.setFenceNV(d->m_fence[0], GL_ALL_COMPLETED_NV);
#else
    } else if (d->m_type == RenderTimer::ARBTimerQuery) {
        d->m_timerQuery.queryCounter(d->m_timer[0], GL_TIMESTAMP);

    } else if (d->m_type == RenderTimer::EXTTimerQuery) {
        d->m_timerQuery.beginQuery(GL_TIME_ELAPSED, d->m_timer[0]);
#endif
    }
}

qint64 RenderTimer::stop()
{
    Q_D(RenderTimer);
    if (d->m_type == RenderTimer::Trivial) {
        glFinish();
        return d-> m_trivialTimer.nsecsElapsed();
#if defined(QT_OPENGL_ES)
    } else if (d->m_type == RenderTimer::KHRFence) {
        QElapsedTimer timer;
        EGLDisplay dpy = eglGetCurrentDisplay();
        EGLSyncKHR afterSync = d->m_fenceSyncKHR.createSyncKHR(dpy, EGL_SYNC_FENCE_KHR, NULL);
        EGLint beforeSyncValue = d->m_fenceSyncKHR.clientWaitSyncKHR(dpy, d->m_beforeSync, 0, EGL_FOREVER_KHR);
        qint64 beforeTime = timer.nsecsElapsed();
        EGLint afterSyncValue = d->m_fenceSyncKHR.clientWaitSyncKHR(dpy, afterSync, 0, EGL_FOREVER_KHR);
        qint64 afterTime = timer.nsecsElapsed();
        d->m_fenceSyncKHR.destroySyncKHR(dpy, afterSync);
        d->m_fenceSyncKHR.destroySyncKHR(dpy, d->m_beforeSync);
        d->m_beforeSync = EGL_NO_SYNC_KHR;
        if (beforeSyncValue == EGL_CONDITION_SATISFIED_KHR
        && afterSyncValue == EGL_CONDITION_SATISFIED_KHR) {
            return afterTime - beforeTime;
        } else {
            return -1;
        }

    } else if (d->m_type == RenderTimer::NVFence) {
        QElapsedTimer timer;
        d->m_fenceNV.setFenceNV(d->m_fence[1], GL_ALL_COMPLETED_NV);
        d->m_fenceNV.finishFenceNV(d->m_fence[0]);
        qint64 beforeTime = timer.nsecsElapsed();
        d->m_fenceNV.finishFenceNV(d->m_fence[1]);
        qint64 afterTime = timer.nsecsElapsed();
        return afterTime - beforeTime;
#else
    } else if (d->m_type == RenderTimer::ARBTimerQuery) {
        GLuint64 time[2] = { 0, 0 };
        d->m_timerQuery.queryCounter(d->m_timer[1], GL_TIMESTAMP);
        d->m_timerQuery.getQueryObjectui64v(d->m_timer[0], GL_QUERY_RESULT, &time[0]);
        d->m_timerQuery.getQueryObjectui64v(d->m_timer[1], GL_QUERY_RESULT, &time[1]);
        if (time[0] != 0 && time[1] != 0) {
            return time[1] - time[0];
        } else {
            return -1;
        }

    } else if (d->m_type == RenderTimer::EXTTimerQuery) {
        GLuint64EXT time;
        d->m_timerQuery.endQuery(GL_TIME_ELAPSED);
        d->m_timerQuery.getQueryObjectui64vExt(d->m_timer[0], GL_QUERY_RESULT, &time);
        return static_cast<qint64>(time);
#endif
    }

    return 0;
}
