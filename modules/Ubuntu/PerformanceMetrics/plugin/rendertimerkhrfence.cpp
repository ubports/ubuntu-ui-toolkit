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

#include "rendertimerkhrfence.h"

RenderTimerKHRFence::RenderTimerKHRFence()
{
}

bool RenderTimerKHRFence::isAvailable()
{
    QList<QByteArray> eglExtensions = QByteArray(
        static_cast<const char*>(eglQueryString(eglGetCurrentDisplay(), EGL_EXTENSIONS))).split(' ');
    QList<QByteArray> glExtensions = QByteArray(
        reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');
    // Note the workaround for PowerVR that declares 'GL_OES_egl_sync'.
    return (eglExtensions.contains("EGL_KHR_fence_sync") &&
            (glExtensions.contains("GL_OES_EGL_sync") || glExtensions.contains("GL_OES_egl_sync"));
}

void RenderTimerKHRFence::setup()
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

void RenderTimerKHRFence::teardown()
{
    if (beforeSync_ != EGL_NO_SYNC_KHR) {
        fenceSyncKHR_.destroySyncKHR(eglGetCurrentDisplay(), beforeSync_);
    }
}

void RenderTimerKHRFence::start()
{
    beforeSync_ = fenceSyncKHR_.createSyncKHR(eglGetCurrentDisplay(), EGL_SYNC_FENCE_KHR, NULL);
}

qint64 RenderTimerKHRFence::stop()
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
