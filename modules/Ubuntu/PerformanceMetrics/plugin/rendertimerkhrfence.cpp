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
#include <QtCore/QElapsedTimer>

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
    return eglExtensions.contains("EGL_KHR_fence_sync") &&
            (glExtensions.contains("GL_OES_EGL_sync") || glExtensions.contains("GL_OES_egl_sync"));
}

void RenderTimerKHRFence::setup()
{
    m_fenceSyncKHR.createSyncKHR = reinterpret_cast<
        EGLSyncKHR (QOPENGLF_APIENTRYP)(EGLDisplay, EGLenum, const EGLint*)>(
        eglGetProcAddress("eglCreateSyncKHR"));
    m_fenceSyncKHR.destroySyncKHR = reinterpret_cast<
        EGLBoolean (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR)>(
        eglGetProcAddress("eglDestroySyncKHR"));
    m_fenceSyncKHR.clientWaitSyncKHR = reinterpret_cast<
        EGLint (QOPENGLF_APIENTRYP)(EGLDisplay, EGLSyncKHR, EGLint, EGLTimeKHR)>(
        eglGetProcAddress("eglClientWaitSyncKHR"));
}

void RenderTimerKHRFence::teardown()
{
    if (m_beforeSync != EGL_NO_SYNC_KHR) {
        m_fenceSyncKHR.destroySyncKHR(eglGetCurrentDisplay(), m_beforeSync);
    }
}

void RenderTimerKHRFence::start()
{
    m_beforeSync = m_fenceSyncKHR.createSyncKHR(eglGetCurrentDisplay(), EGL_SYNC_FENCE_KHR, NULL);
}

qint64 RenderTimerKHRFence::stop()
{
    QElapsedTimer timer;
    EGLDisplay dpy = eglGetCurrentDisplay();
    EGLSyncKHR afterSync = m_fenceSyncKHR.createSyncKHR(dpy, EGL_SYNC_FENCE_KHR, NULL);
    EGLint beforeSyncValue = m_fenceSyncKHR.clientWaitSyncKHR(dpy, m_beforeSync, 0, EGL_FOREVER_KHR);
    qint64 beforeTime = timer.nsecsElapsed();
    EGLint afterSyncValue = m_fenceSyncKHR.clientWaitSyncKHR(dpy, afterSync, 0, EGL_FOREVER_KHR);
    qint64 afterTime = timer.nsecsElapsed();
    m_fenceSyncKHR.destroySyncKHR(dpy, afterSync);
    m_fenceSyncKHR.destroySyncKHR(dpy, m_beforeSync);
    m_beforeSync = EGL_NO_SYNC_KHR;
    if (beforeSyncValue == EGL_CONDITION_SATISFIED_KHR
    && afterSyncValue == EGL_CONDITION_SATISFIED_KHR) {
        return afterTime - beforeTime;
    } else {
        return -1;
    }
}
