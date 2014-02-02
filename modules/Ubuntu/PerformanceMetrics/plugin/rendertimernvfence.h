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

#ifndef RENDERTIMERNVFENCE_H
#define RENDERTIMERNVFENCE_H

#include "rendertimer.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>

class RenderTimerNVFence : public RenderTimer
{
public:
    RenderTimerNVFence();

    static bool isAvailable();
    virtual void setup();
    virtual void teardown();
    virtual void start();
    virtual qint64 stop();

private:
    struct {
        void (QOPENGLF_APIENTRYP genFencesNV)(GLsizei n, GLuint* fences);
        void (QOPENGLF_APIENTRYP deleteFencesNV)(GLsizei n, const GLuint* fences);
        void (QOPENGLF_APIENTRYP setFenceNV)(GLuint fence, GLenum condition);
        void (QOPENGLF_APIENTRYP finishFenceNV)(GLuint fence);
    } fenceNV_;
    GLuint fence_[2];
};

#endif // RENDERTIMERNVFENCE_H
