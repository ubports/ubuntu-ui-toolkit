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

#include "rendertimernvfence.h"

RenderTimerNVFence::RenderTimerNVFence()
{
}

bool RenderTimerNVFence::isAvailable()
{
    QList<QByteArray> glExtensions = QByteArray(
        reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');
    return glExtensions.contains("GL_NV_fence");
}

void RenderTimerNVFence::setup()
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

void RenderTimerNVFence::teardown()
{
    fenceNV_.deleteFencesNV(2, fence_);

}

void RenderTimerNVFence::start()
{
    fenceNV_.setFenceNV(fence_[0], GL_ALL_COMPLETED_NV);
}

qint64 RenderTimerNVFence::stop()
{
    QElapsedTimer timer;
    fenceNV_.setFenceNV(fence_[1], GL_ALL_COMPLETED_NV);
    fenceNV_.finishFenceNV(fence_[0]);
    qint64 beforeTime = timer.nsecsElapsed();
    fenceNV_.finishFenceNV(fence_[1]);
    qint64 afterTime = timer.nsecsElapsed();
    return afterTime - beforeTime;
}
