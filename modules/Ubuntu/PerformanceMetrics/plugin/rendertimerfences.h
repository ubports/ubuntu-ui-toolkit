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

#ifndef RENDERTIMERFENCES_H
#define RENDERTIMERFENCES_H

#include "rendertimer.h"
#include <QtCore/QElapsedTimer>

#if defined(QT_OPENGL_ES)
#include <EGL/egl.h>
#include <EGL/eglext.h>
#else
#include <QtGui/qopenglfunctions.h>
#endif

class RenderTimerFences : public RenderTimer
{
public:
    RenderTimerFences();

    virtual void setup();
    virtual void teardown();
    virtual void start();
    virtual qint64 stop();

private:
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

#endif // RENDERTIMERFENCES_H
