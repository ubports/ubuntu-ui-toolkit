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

#ifndef RENDERTIMERKHRFENCE_H
#define RENDERTIMERKHRFENCE_H

#include "rendertimer.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <QtGui/qopenglfunctions.h>

class RenderTimerKHRFence : public RenderTimer
{
public:
    RenderTimerKHRFence();

    static bool isAvailable();
    virtual void setup();
    virtual void teardown();
    virtual void start();
    virtual qint64 stop();

private:
    struct {
        EGLSyncKHR (QOPENGLF_APIENTRYP createSyncKHR)(EGLDisplay dpy, EGLenum type,
                                                      const EGLint* attrib_list);
        EGLBoolean (QOPENGLF_APIENTRYP destroySyncKHR)(EGLDisplay dpy, EGLSyncKHR sync);
        EGLint (QOPENGLF_APIENTRYP clientWaitSyncKHR)(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags,
                                                      EGLTimeKHR timeout);
    } m_fenceSyncKHR;
    EGLSyncKHR m_beforeSync;
};

#endif // RENDERTIMERKHRFENCE_H
